/* Exercise the real game code with a simulated tile map and shadow OAM.
 * This checks logic and memory safety; it does not emulate SM83/PPU timing. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <gbdk/metasprites.h>
#include "player.h"
#include "mobs.h"
#include "boss.h"
#include "../res/bigShip.h"

volatile OAM_item_t shadow_OAM[40];
uint8_t SCX_REG, SCY_REG, DIV_REG;
static uint8_t keys, randomByte;
static uint8_t tilemap[32 * 32];
static uint8_t inVblank, transfers;
static uint8_t initializing;
static uint16_t transferBytes;

uint8_t joypad(void) { return keys; }
void vsync(void) { inVblank = TRUE; }
void initrand(uint16_t seed) { randomByte = (uint8_t)seed; }
uint8_t test_rand(void) { randomByte += 37; return randomByte; }
void set_sprite_data(uint8_t first, uint8_t count, const uint8_t *data) {
    (void)first; (void)count; (void)data;
}
void set_bkg_data(uint8_t first, uint8_t count, const uint8_t *data) {
    assert((uint16_t)first + count <= 256);
    (void)data;
}
void fill_bkg_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t tile) {
    for (uint8_t row = 0; row < h; ++row)
        for (uint8_t col = 0; col < w; ++col)
            tilemap[((y + row) & 31) * 32 + ((x + col) & 31)] = tile;
}

void set_bkg_submap(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *map, uint8_t map_w) {
    assert(x + w <= Scene1Width && y + h <= Scene1Height);
    for (uint8_t row = 0; row < h; ++row)
        for (uint8_t col = 0; col < w; ++col)
            tilemap[((y + row) & 31) * 32 + ((x + col) & 31)] = map[(y + row) * map_w + x + col];
}
void set_bkg_tiles(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *data) {
    assert(initializing || inVblank);
    assert(x < 32 && y < 32 && w <= 32 && h <= 32);
    if (!initializing) {
        assert(w == 1 || h == 1);
        ++transfers;
        transferBytes += w * h;
    }
    for (uint8_t row = 0; row < h; ++row)
        for (uint8_t col = 0; col < w; ++col)
            tilemap[((y + row) & 31) * 32 + ((x + col) & 31)] = *data++;
}
uint8_t test_metasprite(const metasprite_t *frame, uint8_t first, uint8_t x, uint8_t y, uint8_t props) {
    uint8_t count = 0;
    while (frame->dy != -128) {
        assert(count < ENTITY_SPRITES && first < 40);
        assert((frame->tile & 1u) == 0); /* 8x16 sprites use even tile indices. */
        shadow_OAM[first].tile = frame->tile;
        shadow_OAM[first].prop = props;
        shadow_OAM[first].x = x;
        shadow_OAM[first].y = y;
        ++first; ++frame; ++count;
    }
    return count;
}

static void reset(void) {
    memset((void *)shadow_OAM, 0, sizeof(shadow_OAM));
    memset(tilemap, 0xff, sizeof(tilemap));
    keys = 0;
    setupScene();
    initPlayer();
    initAliens();
    memset(aliens, 0, sizeof(aliens));
    memset(alienBullets, 0, sizeof(alienBullets));
}

static void check_view(void) {
    uint16_t firstX = scene.camera_x / 8;
    uint8_t firstY = scene.camera_y / 8;
    uint8_t width = 20 + ((scene.camera_x & 7) != 0);
    uint8_t height = 18 + ((scene.camera_y & 7) != 0);
    for (uint8_t y = 0; y < height; ++y)
        for (uint8_t x = 0; x < width; ++x)
            assert(tilemap[((firstY + y) & 31) * 32 + ((firstX + x) & 31)] == Scene1[(firstY + y) * Scene1Width + firstX + x]);
}
static void frame_view(void) {
    inVblank = FALSE;
    transfers = 0;
    transferBytes = 0;
    updateView();
    assert(transfers <= 2 && transferBytes <= 40);
    assert(SCX_REG == (uint8_t)scene.camera_x && SCY_REG == scene.camera_y);
    check_view();
}

static void test_scrolling(void) {
    reset();
    check_view();
    /* Exercise the full map, 32-tile ring wrap, diagonal motion and reversal. */
    for (uint16_t x = 1; x <= CAMERA_MAX_X; ++x) {
        scene.camera_x = x;
        if (scene.camera_y < CAMERA_MAX_Y) ++scene.camera_y;
        frame_view();
    }
    while (scene.camera_x) {
        --scene.camera_x;
        if (scene.camera_y) --scene.camera_y;
        frame_view();
    }
    puts("OK: streaming diagonal et inversé, limites de carte et transferts VBlank");
}

static void test_projectiles(void) {
    reset();
    aliens[0] = (Entity_t){.X = 80, .Y = 72, .active = TRUE};
    aliens[1] = (Entity_t){.X = 120, .Y = 72, .active = TRUE};
    /* Both shots must restart the target scan; one collision cannot kill the other shot. */
    playerBullets[0] = (Bullet_t){.X = 72, .Y = 72, .active = TRUE};
    playerBullets[1] = (Bullet_t){.X = 112, .Y = 72, .active = TRUE};
    playerBullets[2] = (Bullet_t){.X = 150, .Y = 100, .active = TRUE};
    moveBullets();
    assert(aliens[0].overlapped && aliens[1].overlapped);
    assert(!playerBullets[0].active && !playerBullets[1].active && playerBullets[2].active);
    assert(shadow_OAM[PLAYER_BULLET_SPRITE].y == 0);

    /* Offscreen 16-bit coordinates must not wrap into OAM. */
    scene.camera_x = 512;
    playerBullets[2].X = 250;
    moveBullets();
    assert(!playerBullets[2].active && !shadow_OAM[PLAYER_BULLET_SPRITE + 2].y);

    /* A shot touches only when its 8x8 box overlaps the 16x16 enemy. */
    reset();
    aliens[0] = (Entity_t){.X = 80, .Y = 72, .active = TRUE};
    playerBullets[0] = (Bullet_t){.X = 88, .Y = 72, .active = TRUE};
    moveBullets();
    assert(!aliens[0].overlapped);
    playerBullets[0].X = 87;
    moveBullets();
    assert(aliens[0].overlapped);
    puts("OK: collisions indépendantes, boîtes de collision et masquage hors écran");
}

static void test_pool_and_flip(void) {
    reset();
    keys = J_A;
    for (uint8_t i = 0; i < MAX_SHOOT_NUM; ++i) {
        player.shootCooldown = 0;
        playerMove();
        assert(playerBullets[i].active);
    }
    player.shootCooldown = 0;
    playerMove();
    assert(!player.shootCooldown); /* Exhaustion does not overwrite a live shot. */
    playerBullets[MAX_SHOOT_NUM - 1].active = FALSE;
    player.direction = -1;
    playerMove();
    assert(playerBullets[MAX_SHOOT_NUM - 1].active && playerBullets[MAX_SHOOT_NUM - 1].speedX == -SHOOT_SPEED);
    assert(shadow_OAM[PLAYER_BULLET_SPRITE + MAX_SHOOT_NUM - 1].prop == S_FLIPX);
    playerBullets[MAX_SHOOT_NUM - 1].active = FALSE;
    player.shootCooldown = 0;
    player.direction = 1;
    playerMove();
    assert(shadow_OAM[PLAYER_BULLET_SPRITE + MAX_SHOOT_NUM - 1].prop == 0);
    puts("OK: pool borné, réutilisation et remise à zéro du flip des tirs");
}

static void test_player_and_aliens(void) {
    reset();
    player.speedX = MAX_SHIP_SPEED;
    keys = J_LEFT;
    playerMove();
    assert(player.turning && player.speedX == MAX_SHIP_SPEED);
    reset();
    player.direction = -1;
    player.speedX = -MAX_SHIP_SPEED;
    keys = J_RIGHT;
    playerMove();
    assert(player.turning && player.speedX == -MAX_SHIP_SPEED);

    reset();
    keys = J_RIGHT | J_DOWN;
    for (uint8_t i = 0; i < 100; ++i) playerMove();
    assert(player.speedX == MAX_SHIP_SPEED && player.speedY <= MAX_SHIP_SPEED);
    assert(player.entity.Y <= (int16_t)(SCENE_HEIGHT - SCENE_BORDER_SIZE));
    assert(scene.camera_x <= CAMERA_MAX_X && scene.camera_y <= CAMERA_MAX_Y);
    keys = J_SELECT;
    playerMove();
    assert(!player.speedX && !player.speedY);

    /* Vertical input during a turn must never index outside the 24-frame turn. */
    keys = J_LEFT | J_UP;
    for (uint8_t i = 0; i < 24; ++i) playerMove();
    assert(!player.turning && player.direction == -1);
    player.entity.X = SCENE_BORDER_SIZE;
    player.upscaledX = SCENE_BORDER_SIZE << SCREEN_SCALE;
    player.speedX = -MAX_SHIP_SPEED;
    keys = 0;
    for (uint8_t i = 0; i < 24; ++i) playerMove();
    assert(!player.turning && player.direction == 1);
    playerMove();
    assert(player.entity.X >= (int16_t)SCENE_BORDER_SIZE);

    reset();
    aliens[0] = (Entity_t){.X = 100, .Y = 80, .active = TRUE};
    for (uint8_t i = 0; i < ALIEN_MOVE_DELAY; ++i) alienMoves();
    assert(alienBullets[0].active);
    aliens[0].overlapped = TRUE;
    aliens[0].animStep = 0;
    for (uint8_t i = 0; i < 21; ++i) alienMoves();
    assert(!aliens[0].active && !alienBullets[0].active);
    for (uint8_t i = 0; i < ENTITY_SPRITES; ++i) assert(!shadow_OAM[ALIEN_SPRITE + i].y);
    aliens[1] = (Entity_t){.X = 800, .Y = 80, .active = TRUE};
    alienMoves();
    for (uint8_t i = 0; i < ENTITY_SPRITES; ++i) assert(!shadow_OAM[ALIEN_SPRITE + ENTITY_SPRITES + i].y);
    puts("OK: vitesse, limites, retournements, explosions et sprites hors écran");
}

static void test_full_loop(void) {
    static const uint8_t inputs[] = {
        J_RIGHT | J_DOWN | J_A, J_RIGHT | J_UP | J_A,
        J_LEFT | J_DOWN | J_A, J_LEFT | J_UP | J_A,
        J_SELECT, J_RIGHT | J_A, J_DOWN | J_A, J_UP | J_A
    };
    reset();
    initAliens();
    for (uint16_t frame = 0; frame < 4096; ++frame) {
        keys = inputs[(frame >> 8) & 7];
        playerMove();
        alienMoves();
        moveBullets();
        frame_view();
        assert(player.entity.X >= (int16_t)SCENE_BORDER_SIZE);
        assert(player.entity.X <= (int16_t)(SCENE_WIDTH - SCENE_BORDER_SIZE));
        assert(player.entity.Y >= (int16_t)SCENE_BORDER_SIZE);
        assert(player.entity.Y <= (int16_t)(SCENE_HEIGHT - SCENE_BORDER_SIZE));
        assert(player.speedX >= -MAX_SHIP_SPEED && player.speedX <= MAX_SHIP_SPEED);
        assert(player.speedY >= -MAX_SHIP_SPEED && player.speedY <= MAX_SHIP_SPEED);
    }
    puts("OK: 4096 frames intégrées avec tirs, diagonales, retournements et contrôle du décor");
}

static void test_camera_reframe(void) {
    reset();
    player.entity.X = 400;
    player.upscaledX = 400 << SCREEN_SCALE;
    scene.camera_x = 400 - START_SCROLL_X;
    player.direction = -1;
    for (uint8_t i = 0; i != 30; ++i) {
        uint16_t previous = scene.camera_x;
        playerMove();
        assert(previous - scene.camera_x <= CAMERA_PAN_STEP);
    }
    assert(scene.camera_x == 400 - (VIEWPORT_WIDTH - START_SCROLL_X));
    reset();
    player.entity.X = 400;
    player.upscaledX = 400 << SCREEN_SCALE;
    scene.camera_x = 400 - START_SCROLL_X;
    keys = J_LEFT;
    playerMove();
    assert(player.turning && scene.camera_x == 400 - START_SCROLL_X - CAMERA_PAN_STEP);
    puts("OK: recadrage progressif selon la direction de vol");
}

static void reset_boss(void) {
    reset();
    initializing = TRUE;
    initBoss();
    initializing = FALSE;
    for (uint8_t i = 0; i < NB_ALIENS; ++i) assert(!aliens[i].active);
    for (uint8_t y = 0; y < bigShip_HEIGHT / 8; ++y)
        for (uint8_t x = 0; x < bigShip_WIDTH / 8; ++x)
            assert(tilemap[y * 32 + 21 + x] == bigShip_map[y * (bigShip_WIDTH / 8) + x]);
}

static void test_boss(void) {
    reset_boss();
    boss.X = 90;
    for (uint8_t i = 0; i < BOSS_MOVE_DELAY; ++i) bossMove();
    assert(bossBullets[0].active);
    int16_t shotX = bossBullets[0].X;
    bossMove();
    moveBullets();
    assert(!boss.moved && bossBullets[0].X == shotX);

    playerBullets[0] = (Bullet_t){.X = shipguns[0].X - 8, .Y = shipguns[0].Y, .active = TRUE};
    moveBullets();
    assert(shipguns[0].overlapped && !playerBullets[0].active);
    for (uint8_t i = 0; i != 21; ++i) bossMove();
    assert(!shipguns[0].active && !bossBullets[0].active);
    for (uint8_t i = 0; i < ENTITY_SPRITES; ++i) assert(!shadow_OAM[SHIPGUN_SPRITE + i].y);
    bossBullets[1] = (Bullet_t){.X = player.entity.X, .Y = player.entity.Y, .active = TRUE};
    moveBullets();
    assert(player.entity.overlapped && !bossBullets[1].active);
    puts("OK: tirs des tourelles, cadence, collisions et destruction");

    reset_boss();
    uint8_t entered = FALSE;
    for (uint16_t frame = 0; frame != 4096; ++frame) {
        keys = ((frame >> 8) & 1) ? J_LEFT | J_UP | J_A : J_RIGHT | J_DOWN | J_A;
        playerMove();
        bossMove();
        moveBullets();
        inVblank = FALSE;
        transfers = 0;
        updateView();
        assert(!transfers); /* Boss background is loaded once, then scrolled. */
        assert(SCX_REG == (uint8_t)(BOSS_ENTRY_X - boss.X));
        assert(SCY_REG == (uint8_t)-boss.Y);
        assert(boss.X >= BOSS_MIN_X && boss.X <= BOSS_ENTRY_X);
        if (boss.X <= BOSS_MAX_X) entered = TRUE;
        if (entered) assert(boss.X <= BOSS_MAX_X);
        assert(boss.Y >= 0 && boss.Y <= BOSS_MAX_Y);
        assert(!scene.camera_x && !scene.camera_y);
        assert(player.entity.X >= (int16_t)SCENE_BORDER_SIZE && player.entity.X <= (int16_t)VIEWPORT_WIDTH);
        assert(player.entity.Y >= (int16_t)SCENE_BORDER_SIZE && player.entity.Y <= (int16_t)VIEWPORT_HEIGHT);
        assert(!player.turning && player.direction == 1);
        for (uint8_t i = 0; i != NB_SHIPGUN; ++i) {
            if (!shipguns[i].active) continue;
            assert(shipguns[i].X == boss.X + (i == 1 ? 32 : 56));
            assert(shipguns[i].Y == boss.Y + 32 + i * 24);
        }
    }
    assert(entered);
    puts("OK: 4096 frames de combat, limites écran, déplacements du boss et alignement des tourelles");
}

int main(void) {
    test_scrolling();
    test_projectiles();
    test_pool_and_flip();
    test_player_and_aliens();
    test_full_loop();
    test_camera_reframe();
    test_boss();
    puts("Tous les tests de logique passent.");
    return 0;
}

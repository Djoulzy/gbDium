/* Exercise the real game code with a simulated tile map and shadow OAM.
 * This checks logic and memory safety; it does not emulate SM83/PPU timing. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <gbdk/metasprites.h>
#include "player.h"
#include "mobs.h"

volatile OAM_item_t shadow_OAM[40];
uint8_t SCX_REG, SCY_REG, DIV_REG;
static uint8_t keys, randomByte;
static uint8_t tilemap[32 * 32];
static uint8_t inVblank, transfers;
static uint16_t transferBytes;

uint8_t joypad(void) { return keys; }
void vsync(void) { inVblank = TRUE; }
void initrand(uint16_t seed) { randomByte = (uint8_t)seed; }
uint8_t test_rand(void) { randomByte += 37; return randomByte; }
void set_sprite_data(uint8_t first, uint8_t count, const uint8_t *data) {
    (void)first; (void)count; (void)data;
}

void set_bkg_submap(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *map, uint8_t map_w) {
    assert(x + w <= Scene1Width && y + h <= Scene1Height);
    for (uint8_t row = 0; row < h; ++row)
        for (uint8_t col = 0; col < w; ++col)
            tilemap[((y + row) & 31) * 32 + ((x + col) & 31)] = map[(y + row) * map_w + x + col];
}
void set_bkg_tiles(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *data) {
    assert(inVblank);
    assert(x < 32 && y < 32 && w <= 32 && h <= 32);
    assert(w == 1 || h == 1);
    ++transfers;
    transferBytes += w * h;
    for (uint8_t row = 0; row < h; ++row)
        for (uint8_t col = 0; col < w; ++col)
            tilemap[((y + row) & 31) * 32 + ((x + col) & 31)] = *data++;
}
uint8_t test_metasprite(const metasprite_t *frame, uint8_t first, uint8_t x, uint8_t y, uint8_t props) {
    uint8_t count = 0;
    while (frame->dy != -128) {
        assert(count < 4 && first < 40);
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
    playerBullets[3].active = FALSE;
    player.direction = -1;
    playerMove();
    assert(playerBullets[3].active && playerBullets[3].speedX == -SHOOT_SPEED);
    assert(shadow_OAM[PLAYER_BULLET_SPRITE + 3].prop == S_FLIPX);
    playerBullets[3].active = FALSE;
    player.shootCooldown = 0;
    player.direction = 1;
    playerMove();
    assert(shadow_OAM[PLAYER_BULLET_SPRITE + 3].prop == 0);
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
    alienMoves();
    assert(alienBullets[0].active);
    aliens[0].overlapped = TRUE;
    aliens[0].animStep = 0;
    for (uint8_t i = 0; i < 21; ++i) alienMoves();
    assert(!aliens[0].active && !alienBullets[0].active);
    for (uint8_t i = 0; i < 4; ++i) assert(!shadow_OAM[ALIEN_SPRITE + i].y);
    aliens[1] = (Entity_t){.X = 800, .Y = 80, .active = TRUE};
    alienMoves();
    for (uint8_t i = 0; i < 4; ++i) assert(!shadow_OAM[ALIEN_SPRITE + 4 + i].y);
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

int main(void) {
    test_scrolling();
    test_projectiles();
    test_pool_and_flip();
    test_player_and_aliens();
    test_full_loop();
    puts("Tous les tests de logique passent.");
    return 0;
}

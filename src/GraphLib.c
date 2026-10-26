#include "GraphLib.h"
#include "player.h"
#include "mobs.h"
#include "../res/scene1.h"

Scene_t scene;
Bullet_t playerBullets[MAX_SHOOT_NUM];
Bullet_t alienBullets[NB_ALIENS];

/* Column gathering happens outside VBlank. Only the bounded VRAM copies
 * and scroll-register writes follow vsync(). No full-map redraw per frame. */
static uint8_t columnBuffer[19];

void setupScene(void) {
    scene.camera_x = 0;
    scene.camera_y = 0;
    scene.map_pos_x = 0;
    scene.map_pos_y = 0;
    /* Include the partially visible row and column before scrolling starts. */
    set_bkg_submap(0, 0, 21, 19, Scene1, Scene1Width);
}

/* An 8x8 shot against a 16x16 metasprite with pivot (-8, 0).
 * World-space collisions are independent of camera movement. */
static inline uint8_t bulletHits(const Bullet_t *bullet, const Entity_t *target) {
    return bullet->X < target->X + 8 &&
           bullet->X + 8 > target->X - 8 &&
           bullet->Y < target->Y + 16 &&
           bullet->Y + 8 > target->Y;
}

void moveBullets(void) {
    Bullet_t *bullet;
    Entity_t *target;
    uint8_t count, targets, sprite;
    int16_t x, y;

    bullet = playerBullets;
    sprite = PLAYER_BULLET_SPRITE;
    for (count = MAX_SHOOT_NUM; count; --count, ++bullet, ++sprite) {
        if (!bullet->active) continue;
        bullet->X += bullet->speedX;
        bullet->Y += bullet->speedY;
        x = bullet->X - (int16_t)scene.camera_x;
        y = bullet->Y - scene.camera_y;
        if (isVisible(x, y)) {
            target = aliens;
            for (targets = NB_ALIENS; targets; --targets, ++target) {
                if (target->active && !target->overlapped && bulletHits(bullet, target)) {
                    target->overlapped = TRUE;
                    target->animStep = 0;
                    bullet->active = FALSE;
                    break;
                }
            }
        } else bullet->active = FALSE;

        if (bullet->active) move_sprite(sprite, (uint8_t)x, (uint8_t)y);
        else hide_sprite(sprite);
    }

    bullet = alienBullets;
    sprite = ALIEN_BULLET_SPRITE;
    for (count = NB_ALIENS; count; --count, ++bullet, ++sprite) {
        if (!bullet->active) continue;
        bullet->X += bullet->speedX;
        bullet->Y += bullet->speedY;
        x = bullet->X - (int16_t)scene.camera_x;
        y = bullet->Y - scene.camera_y;
        if (!isVisible(x, y)) bullet->active = FALSE;
        else if (bulletHits(bullet, &player.entity)) {
            player.entity.overlapped = TRUE;
            bullet->active = FALSE;
        }
        if (bullet->active) move_sprite(sprite, (uint8_t)x, (uint8_t)y);
        else hide_sprite(sprite);
    }
}

void updateView(void) {
    uint8_t tx = (uint8_t)(scene.camera_x >> 3u);
    uint8_t ty = scene.camera_y >> 3u;
    uint8_t column = 0, row = 0;
    uint8_t columnHeight = 0, rowWidth = 0;
    uint8_t remaining;
    const uint8_t *source;
    uint8_t *destination;
    const uint8_t *rowSource = Scene1;

    if (tx != scene.map_pos_x) {
        column = tx < scene.map_pos_x ? tx : tx + 20u;
        if (column < Scene1Width) {
            columnHeight = ty < Scene1Height - 18u ? 19u : 18u;
            source = Scene1 + (uint16_t)ty * Scene1Width + column;
            destination = columnBuffer;
            for (remaining = columnHeight; remaining; --remaining) {
                *destination++ = *source;
                source += Scene1Width;
            }
        }
    }
    if (ty != scene.map_pos_y) {
        row = ty < scene.map_pos_y ? ty : ty + 18u;
        if (row < Scene1Height) {
            rowWidth = tx < Scene1Width - 20u ? 21u : 20u;
            rowSource = Scene1 + (uint16_t)row * Scene1Width + tx;
        }
    }

    /* GBDK transfers shadow OAM in its VBlank handler. */
    vsync();
    if (columnHeight) set_bkg_tiles(column & 31u, ty & 31u, 1, columnHeight, columnBuffer);
    if (rowWidth) set_bkg_tiles(tx & 31u, row & 31u, rowWidth, 1, rowSource);
    SCX_REG = (uint8_t)scene.camera_x;
    SCY_REG = scene.camera_y;
    scene.map_pos_x = tx;
    scene.map_pos_y = ty;
}

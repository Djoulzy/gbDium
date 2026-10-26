#include <gbdk/metasprites.h>
#include "boss.h"
#include "mobs.h"
#include "../res/bigShip.h"
#include "../res/ShipGun.h"
#include "../res/alien_tiles.h"

Boss_t boss;
Entity_t shipguns[NB_SHIPGUN];

static const uint8_t gunOffsetX[NB_SHIPGUN] = {56, 32, 56};
static const uint8_t gunOffsetY[NB_SHIPGUN] = {32, 56, 80};
static const metasprite_t shipgun[] = {
    METASPR_ITEM(0, -8, SHIPGUN_TILE, 0),
    METASPR_ITEM(0, 8, SHIPGUN_TILE + 2u, 0), METASPR_TERM
};

void initBoss(void) {
    Entity_t *gun = shipguns;
    Bullet_t *bullet = bossBullets;
    uint8_t i;
    gameMode = GAME_BOSS;
    scene.camera_x = scene.camera_y = 0;
    SCX_REG = SCY_REG = 0;
    set_bkg_data(0, bigShip_TILE_COUNT, bigShip_tiles);
    fill_bkg_rect(0, 0, 32, 32, 0);
    set_bkg_tiles(21, 0, bigShip_WIDTH >> 3u, bigShip_HEIGHT >> 3u, bigShip_map);
    /* Keep graphics in disjoint tile ranges, including explosion frames. */
    set_sprite_data(ALIEN_TILE, 18, alien_tiles);
    set_sprite_data(SHIPGUN_TILE, 6, ShipGun);
    boss.X = BOSS_ENTRY_X;
    boss.Y = 0;
    boss.speedX = -1;
    boss.speedY = 1;
    boss.timer = BOSS_MOVE_DELAY;
    boss.moved = FALSE;
    for (i = 0; i != NB_SHIPGUN; ++i, ++gun, ++bullet) {
        gun->X = boss.X + gunOffsetX[i];
        gun->Y = boss.Y + gunOffsetY[i];
        gun->active = TRUE;
        gun->overlapped = FALSE;
        gun->animStep = 0;
        bullet->active = FALSE;
        set_sprite_tile(SHIPGUN_BULLET_SPRITE + i, SHIPGUN_BULLET_TILE);
        set_sprite_prop(SHIPGUN_BULLET_SPRITE + i, 0);
        hideEntity(SHIPGUN_SPRITE + i * ENTITY_SPRITES);
        hide_sprite(SHIPGUN_BULLET_SPRITE + i);
    }
    for (i = 0; i != NB_ALIENS; ++i) {
        aliens[i].active = FALSE;
        alienBullets[i].active = FALSE;
        hideEntity(ALIEN_SPRITE + i * ENTITY_SPRITES);
        hide_sprite(ALIEN_BULLET_SPRITE + i);
    }
}

void bossMove(void) {
    Entity_t *gun = shipguns;
    Bullet_t *bullet = bossBullets;
    uint8_t i, sprite = SHIPGUN_SPRITE;
    boss.moved = FALSE;
    if (!--boss.timer) {
        boss.timer = BOSS_MOVE_DELAY;
        boss.moved = TRUE;
        boss.X += boss.speedX;
        boss.Y += boss.speedY;
        if (boss.X <= BOSS_MIN_X) boss.speedX = 1;
        else if (boss.X >= BOSS_MAX_X) boss.speedX = -1;
        if (boss.Y <= 0) boss.speedY = 1;
        else if (boss.Y >= BOSS_MAX_Y) boss.speedY = -1;
    }
    for (i = 0; i != NB_SHIPGUN; ++i, ++gun, ++bullet, sprite += ENTITY_SPRITES) {
        if (!gun->active) continue;
        gun->X = boss.X + gunOffsetX[i];
        gun->Y = boss.Y + gunOffsetY[i];
        if (gun->overlapped) {
            if (gun->animStep == 20u) {
                gun->active = FALSE;
                bullet->active = FALSE;
                hideEntity(sprite);
                hide_sprite(SHIPGUN_BULLET_SPRITE + i);
                continue;
            }
            if (isVisible(gun->X, gun->Y))
                move_metasprite_ex(alienFrames[gun->animStep < 10u ? 2u : 3u], 0, 0, sprite, (uint8_t)gun->X, (uint8_t)gun->Y);
            else hideEntity(sprite);
            ++gun->animStep;
        } else if (isVisible(gun->X, gun->Y)) {
            move_metasprite_ex(shipgun, 0, 0, sprite, (uint8_t)gun->X, (uint8_t)gun->Y);
            if (boss.moved && !bullet->active) {
                bullet->X = gun->X;
                bullet->Y = gun->Y;
                bullet->speedX = -2;
                bullet->speedY = 0;
                bullet->active = TRUE;
            }
        } else hideEntity(sprite);
    }
}

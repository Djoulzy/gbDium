#include <rand.h>
#include <gbdk/metasprites.h>
#include "mobs.h"
#include "player.h"
#include "../res/alien_tiles.h"

Entity_t aliens[NB_ALIENS];
static uint8_t alienTurn;
static int8_t alienSpeedX[NB_ALIENS], alienSpeedY[NB_ALIENS];
static uint8_t alienFractionX[NB_ALIENS], alienFractionY[NB_ALIENS];

static const metasprite_t alien1[] = {
    METASPR_ITEM(0, -8, 54, 0), METASPR_ITEM(0, 8, 56, 0), METASPR_TERM
};
static const metasprite_t alien2[] = {
    METASPR_ITEM(0, -8, 58, 0), METASPR_ITEM(0, 8, 60, 0), METASPR_TERM
};
static const metasprite_t alienBoom1[] = {
    METASPR_ITEM(0, -8, 62, 0), METASPR_ITEM(0, 8, 64, 0), METASPR_TERM
};
static const metasprite_t alienBoom2[] = {
    METASPR_ITEM(0, -8, 66, 0), METASPR_ITEM(0, 8, 68, 0), METASPR_TERM
};
const metasprite_t * const alienFrames[4] = {
    alien1, alien2, alienBoom1, alienBoom2
};

void initAliens(void) {
    Entity_t *alien = aliens;
    Bullet_t *bullet = alienBullets;
    uint8_t i;
    set_sprite_data(ALIEN_TILE, 18, alien_tiles);
    initrand(DIV_REG);
    alienTurn = ALIEN_MOVE_DELAY;
    for (i = 0; i != NB_ALIENS; ++i, ++alien, ++bullet) {
        alien->X = (uint16_t)rand() << 2u;
        alien->Y = rand();
        alien->active = TRUE;
        alien->overlapped = FALSE;
        alien->animStep = 0;
        alienSpeedX[i] = alienSpeedY[i] = 0;
        alienFractionX[i] = alienFractionY[i] = 0;
        bullet->active = FALSE;
        set_sprite_tile(ALIEN_BULLET_SPRITE + i, 70);
        set_sprite_prop(ALIEN_BULLET_SPRITE + i, 0);
        hideEntity(ALIEN_SPRITE + i * ENTITY_SPRITES);
        hide_sprite(ALIEN_BULLET_SPRITE + i);
    }
}

uint8_t alienMoves(void) {
    Entity_t *alien = aliens;
    Bullet_t *bullet = alienBullets;
    uint8_t i, sprite = ALIEN_SPRITE;
    uint8_t frame, remaining = 0;
    int8_t delta;
    int16_t x, y;
    --alienTurn;
    for (i = 0; i != NB_ALIENS; ++i, ++alien, ++bullet, sprite += ENTITY_SPRITES) {
        if (!alien->active) continue;
        if (alien->overlapped) {
            if (alien->animStep == 20u) {
                alien->active = FALSE;
                bullet->active = FALSE;
                hideEntity(sprite);
                hide_sprite(ALIEN_BULLET_SPRITE + i);
                continue;
            }
            frame = alien->animStep < 10u ? 2u : 3u;
            ++alien->animStep;
        } else {
            if (!alienTurn) {
                if (alien->X < player.entity.X && alienSpeedX[i] < ALIEN_MAX_SPEED) alienSpeedX[i] += ALIEN_SPEED;
                else if (alien->X > player.entity.X && alienSpeedX[i] > -ALIEN_MAX_SPEED) alienSpeedX[i] -= ALIEN_SPEED;
                if (alien->Y < player.entity.Y && alienSpeedY[i] < ALIEN_MAX_SPEED) alienSpeedY[i] += ALIEN_SPEED;
                else if (alien->Y > player.entity.Y && alienSpeedY[i] > -ALIEN_MAX_SPEED) alienSpeedY[i] -= ALIEN_SPEED;
                /* Carry the fraction in both directions: signed >> 4 alone
                 * would move left immediately but discard small right speeds. */
                delta = (int8_t)alienFractionX[i] + alienSpeedX[i];
                alien->X += delta >> 4;
                alienFractionX[i] = (uint8_t)delta & 15u;
                delta = (int8_t)alienFractionY[i] + alienSpeedY[i];
                alien->Y += delta >> 4;
                alienFractionY[i] = (uint8_t)delta & 15u;
                if (++alien->animStep == 20u) alien->animStep = 0;
            }
            frame = alien->animStep < 10u ? 0u : 1u;
        }
        x = alien->X - (int16_t)scene.camera_x;
        y = alien->Y - scene.camera_y;
        if (isVisible(x, y)) {
            move_metasprite_ex(alienFrames[frame], 0, 0, sprite, (uint8_t)x, (uint8_t)y);
            if (!alien->overlapped && !alienTurn && !bullet->active) {
                bullet->X = alien->X;
                bullet->Y = alien->Y;
                bullet->speedX = player.entity.X == alien->X ? 0 : (player.entity.X > alien->X ? ALIEN_SHOOT_SPEED : -ALIEN_SHOOT_SPEED);
                bullet->speedY = player.entity.Y == alien->Y ? 0 : (player.entity.Y > alien->Y ? ALIEN_SHOOT_SPEED : -ALIEN_SHOOT_SPEED);
                bullet->active = TRUE;
            }
        } else hideEntity(sprite);
        ++remaining;
    }
    if (!alienTurn) alienTurn = ALIEN_MOVE_DELAY;
    return remaining;
}

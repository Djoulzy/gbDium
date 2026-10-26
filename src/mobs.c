#include <rand.h>
#include <gbdk/metasprites.h>
#include "mobs.h"
#include "player.h"
#include "../res/alien_tiles.h"

Entity_t aliens[NB_ALIENS];
static uint8_t alienTurn;

static const metasprite_t alien1[] = {
    METASPR_ITEM(0, -8, 53, 0), METASPR_ITEM(8, 0, 54, 0),
    METASPR_ITEM(-8, 8, 55, 0), METASPR_ITEM(8, 0, 56, 0), METASPR_TERM
};
static const metasprite_t alien2[] = {
    METASPR_ITEM(0, -8, 57, 0), METASPR_ITEM(8, 0, 58, 0),
    METASPR_ITEM(-8, 8, 59, 0), METASPR_ITEM(8, 0, 60, 0), METASPR_TERM
};
static const metasprite_t alienBoom1[] = {
    METASPR_ITEM(0, -8, 61, 0), METASPR_ITEM(8, 0, 62, 0),
    METASPR_ITEM(-8, 8, 63, 0), METASPR_ITEM(8, 0, 64, 0), METASPR_TERM
};
static const metasprite_t alienBoom2[] = {
    METASPR_ITEM(0, -8, 65, 0), METASPR_ITEM(8, 0, 66, 0),
    METASPR_ITEM(-8, 8, 67, 0), METASPR_ITEM(8, 0, 68, 0), METASPR_TERM
};
static const metasprite_t * const alienFrames[] = {
    alien1, alien2, alienBoom1, alienBoom2
};

void initAliens(void) {
    Entity_t *alien = aliens;
    Bullet_t *bullet = alienBullets;
    uint8_t i;
    set_sprite_data(53, 17, alien_tiles);
    initrand(DIV_REG);
    alienTurn = NB_ALIENS;
    for (i = 0; i != NB_ALIENS; ++i, ++alien, ++bullet) {
        alien->X = (uint16_t)rand() << 2u;
        alien->Y = rand();
        alien->active = TRUE;
        alien->overlapped = FALSE;
        alien->animStep = 0;
        bullet->active = FALSE;
        set_sprite_tile(ALIEN_BULLET_SPRITE + i, 69);
        set_sprite_prop(ALIEN_BULLET_SPRITE + i, 0);
        hideEntity(ALIEN_SPRITE + i * 4u);
        hide_sprite(ALIEN_BULLET_SPRITE + i);
    }
}

void alienMoves(void) {
    Entity_t *alien = aliens;
    Bullet_t *bullet = alienBullets;
    uint8_t remaining, sprite = ALIEN_SPRITE;
    uint8_t bulletSprite = ALIEN_BULLET_SPRITE;
    uint8_t frame;
    int16_t x, y;

    --alienTurn;
    for (remaining = NB_ALIENS; remaining; --remaining, ++alien, ++bullet, sprite += 4u, ++bulletSprite) {
        if (!alien->active) continue;
        if (alien->overlapped) {
            if (alien->animStep == 20u) {
                alien->active = FALSE;
                bullet->active = FALSE;
                hideEntity(sprite);
                hide_sprite(bulletSprite);
                continue;
            }
            frame = alien->animStep < 10u ? 2u : 3u;
            ++alien->animStep;
        } else {
            /* Preserve the original 4 movement frames out of every 5. */
            if (alienTurn) {
                if (alien->X < player.entity.X) alien->X += ALIEN_SPEED;
                else if (alien->X > player.entity.X) alien->X -= ALIEN_SPEED;
                if (alien->Y < player.entity.Y) alien->Y += ALIEN_SPEED;
                else if (alien->Y > player.entity.Y) alien->Y -= ALIEN_SPEED;
                if (++alien->animStep == 20u) alien->animStep = 0;
            }
            frame = alien->animStep < 10u ? 0u : 1u;
        }
        x = alien->X - (int16_t)scene.camera_x;
        y = alien->Y - scene.camera_y;
        if (isVisible(x, y)) {
            move_metasprite_ex(alienFrames[frame], 0, 0, sprite, (uint8_t)x, (uint8_t)y);
            if (!alien->overlapped && alienTurn && !bullet->active) {
                bullet->X = alien->X;
                bullet->Y = alien->Y;
                bullet->speedX = player.entity.X == alien->X ? 0 : (player.entity.X > alien->X ? ALIEN_SHOOT_SPEED : -ALIEN_SHOOT_SPEED);
                bullet->speedY = player.entity.Y == alien->Y ? 0 : (player.entity.Y > alien->Y ? ALIEN_SHOOT_SPEED : -ALIEN_SHOOT_SPEED);
                bullet->active = TRUE;
            }
        } else hideEntity(sprite);
    }
    if (!alienTurn) alienTurn = NB_ALIENS;
}

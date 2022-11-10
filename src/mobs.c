#include <gb/gb.h>
#include <gbdk/emu_debug.h>
#include <gbdk/platform.h>
#include <stdlib.h>
#include <rand.h>
#include "GraphLib.h"
#include "../res/alien_tiles.h"

#define NB_ALIENS       5
#define SHOOT_SPRITE    ALIENS_SPRITE+(NB_ALIENS*4)
#define SCREEN_MULTI    4
#define MAX_SHOOT_NUM   10
#define ALIEN_SPEED     1
#define SHOOT_SPEED     2
#define DELAY_COUNT     2

Entity_t alienTroop[NB_ALIENS];
Bullet_t* aliens_bullet[NB_ALIENS][1];
uint8_t alien_turn = NB_ALIENS;

const metasprite_t alien1[] = {
    METASPR_ITEM(0, -8, 45, 0), METASPR_ITEM(8, 0, 46, 0), METASPR_ITEM(-8, 8, 47, 0), METASPR_ITEM(8, 0, 48, 0),
    METASPR_TERM
};

const metasprite_t alien2[] = {
    METASPR_ITEM(0, -8, 49, 0), METASPR_ITEM(8, 0, 50, 0), METASPR_ITEM(-8, 8, 51, 0), METASPR_ITEM(8, 0, 52, 0),
    METASPR_TERM
};

const metasprite_t* const alienFrames[2] = {
    alien1, alien2
};

void initAliens(void) {
    uint8_t i;

    set_sprite_data(45, 9, alien_tiles);

    initrand(DIV_REG);
    for (i = 0; i<NB_ALIENS; i++) {
        setupEntity(&alienTroop[i], alienFrames, rand() << 2, rand());
        aliens_bullet[i][0] = allocBullet(53);
        assignBulletsToEntity(&alienTroop[i], aliens_bullet[i], 1, 1);

        // dumpEntity(&alienTroop[i]);
    }
}

void alienMoves(Scene_t* scene, Coord_t* playerCoord) {
    uint8_t i;
    int8_t spdx, spdy;

    alien_turn--;
    for (i = 0; i<NB_ALIENS; i++) {
        if (i == alien_turn) {
            if (playerCoord->X > alienTroop[i].coord.X) alienTroop[i].speedX = ALIEN_SPEED; else alienTroop[i].speedX = -ALIEN_SPEED;
            if (playerCoord->Y > alienTroop[i].coord.Y) alienTroop[i].speedY = ALIEN_SPEED; else alienTroop[i].speedY = -ALIEN_SPEED;

            alienTroop[i].coord.X += alienTroop[i].speedX;
            alienTroop[i].coord.Y += alienTroop[i].speedY;
            updateMobPos(scene, &alienTroop[i]);
            
            if (isVisible(&(alienTroop[i].coord))) {
                spdx = spdy = 0;
                if (!alienTroop[i].bullets[0]->active) {
                    if (playerCoord->X > alienTroop[i].coord.X) spdx = SHOOT_SPEED;
                    else if (playerCoord->X < alienTroop[i].coord.X) spdx = -SHOOT_SPEED;
                    if (playerCoord->Y > alienTroop[i].coord.Y) spdy = SHOOT_SPEED;
                    else if (playerCoord->Y < alienTroop[i].coord.Y) spdy = -SHOOT_SPEED;
                    entityShoot(&alienTroop[i], spdx, spdy, 0);
                }

                alienTroop[i].animStep = abs(alienTroop[i].animStep -1);
                move_metasprite(alienFrames[alienTroop[i].animStep], 0, alienTroop[i].spriteNum, alienTroop[i].coord.viewportX, alienTroop[i].coord.viewportY);
            }
        } else {
            updateMobPos(scene, &alienTroop[i]);
            if (isVisible(&(alienTroop[i].coord)))
                move_metasprite(alienFrames[alienTroop[i].animStep], 0, alienTroop[i].spriteNum, alienTroop[i].coord.viewportX, alienTroop[i].coord.viewportY);
        }

        moveEntityBullets(scene, &alienTroop[i]);
    }
    if (!alien_turn) alien_turn = NB_ALIENS;
}
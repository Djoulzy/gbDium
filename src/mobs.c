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
    int8_t spdx, spdy;

    alien_turn--;

    if (playerCoord->X > alienTroop[alien_turn].coord.X) alienTroop[alien_turn].speedX = ALIEN_SPEED; else alienTroop[alien_turn].speedX = -ALIEN_SPEED;
    if (playerCoord->Y > alienTroop[alien_turn].coord.Y) alienTroop[alien_turn].speedY = ALIEN_SPEED; else alienTroop[alien_turn].speedY = -ALIEN_SPEED;

    updateMobPos(scene, &alienTroop[alien_turn]);
    
    if (isVisible(&(alienTroop[alien_turn].coord))) {
        spdx = spdy = 0;
        if (!alienTroop[alien_turn].bullets[0]->active) {
            if (playerCoord->X > alienTroop[alien_turn].coord.X) spdx = SHOOT_SPEED;
            else if (playerCoord->X < alienTroop[alien_turn].coord.X) spdx = -SHOOT_SPEED;
            if (playerCoord->Y > alienTroop[alien_turn].coord.Y) spdy = SHOOT_SPEED;
            else if (playerCoord->Y < alienTroop[alien_turn].coord.Y) spdy = -SHOOT_SPEED;
            entityShoot(&alienTroop[alien_turn], spdx, spdy, 0);
        }

        alienTroop[alien_turn].animStep += 16;
        alienTroop[alien_turn].animStep = abs(alienTroop[alien_turn].animStep -1);
        move_metasprite(alienFrames[alienTroop[alien_turn].animStep >> 7], 0, alienTroop[alien_turn].spriteNum, alienTroop[alien_turn].coord.viewportX, alienTroop[alien_turn].coord.viewportY);
    } else {
        hide_metasprite(alienFrames[0], alienTroop[alien_turn].spriteNum);
    }

    moveEntityBullets(scene, &alienTroop[alien_turn]);

    if (!alien_turn) alien_turn = NB_ALIENS;
}
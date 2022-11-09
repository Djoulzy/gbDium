#include <gb/gb.h>
#include <gbdk/emu_debug.h>
#include <gbdk/platform.h>
#include <stdlib.h>
#include <rand.h>
#include "GraphLib.h"
#include "../res/alien_tiles.h"

#define NB_ALIENS       5
#define ALIENS_SPRITE   10
#define SHOOT_SPRITE    ALIENS_SPRITE+(NB_ALIENS*4)
#define SCREEN_MULTI    4
#define MAX_SHOOT_NUM   10
#define SHOOT_SPEED     6

Entity_t alienTroop[NB_ALIENS];
uint8_t next_shoot = 0;

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
    Bullet_t* alien_shoot;

    set_sprite_data(45, 9, alien_tiles);

    initrand(DIV_REG);
    for (i = 0; i<NB_ALIENS; i++) {
        setupEntity(&alienTroop[i], alienFrames, (rand() << 1) << SCREEN_MULTI, (rand() << 1) << SCREEN_MULTI);
        alien_shoot = allocBullet(53);
        assignBulletsToEntity(&alienTroop[i], &alien_shoot, 1, 1);
    }
}

void alienMoves(Scene_t* scene, Coord_t* playerCoord) {
    uint8_t i;
    int8_t spdx, spdy;

    for (i = 0; i<NB_ALIENS; i++) {
        if (playerCoord->sceneX > alienTroop[i].coord.sceneX) alienTroop[i].speedX = 4; else alienTroop[i].speedX = -4;
        if (playerCoord->sceneY > alienTroop[i].coord.sceneY) alienTroop[i].speedY = 4; else alienTroop[i].speedY = -4;

        updateEntityPos(scene, &alienTroop[i]);
        
        if (isVisible(&alienTroop[i])) {
            if (!alienTroop[i].bullets[0]->active) {
                if (playerCoord->sceneX > alienTroop[i].coord.sceneX) spdx = SHOOT_SPEED;
                else if (playerCoord->sceneX < alienTroop[i].coord.sceneX) spdx = -SHOOT_SPEED;
                if (playerCoord->sceneY > alienTroop[i].coord.sceneY) spdy = SHOOT_SPEED;
                else if (playerCoord->sceneY < alienTroop[i].coord.sceneY) spdy = -SHOOT_SPEED;
                entityShoot(&alienTroop[i], spdx, spdy, 0);
            }

            alienTroop[i].animStep = abs(alienTroop[i].animStep -1);
            move_metasprite(alienFrames[alienTroop[i].animStep >> 7], 0, alienTroop[i].spriteNum, alienTroop[i].coord.visibleX, alienTroop[i].coord.visibleY);
            alienTroop[i].animStep += 16;
        }

        // moveEntityBullets(scene, &alienTroop[i]);

        // if (alienTroop[i]->shoot->active) {
        //     alienTroop[i]->shoot->x += alienTroop[i]->shoot->spdx;
        //     alienTroop[i]->shoot->y += alienTroop[i]->shoot->spdy;
        //     tmpX = (alienTroop[i]->shoot->x - xref) >> SCREEN_MULTI;
        //     tmpY = (alienTroop[i]->shoot->y - yref) >> SCREEN_MULTI;
        //     // EMU_printf("ShootX: %d, ShootY: %d", tmpX, tmpY);
        //     if ((tmpX>=0) && (tmpX<=176) && (tmpY>=0) && (tmpY<=160)) {
        //         move_sprite(alienTroop[i]->shoot->spriteNum, tmpX, tmpY);
        //     } else alienTroop[i]->shoot->active = FALSE;
        // }
    }
}
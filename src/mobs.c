#include <gb/gb.h>
#include <gbdk/emu_debug.h>
#include <gbdk/platform.h>
#include <gbdk/metasprites.h>
#include <stdlib.h>
#include <rand.h>
#include "def.h"
#include "../res/aliens.h"

#define NB_ALIENS       5
#define ALIENS_SPRITE   10
#define SHOOT_SPRITE    ALIENS_SPRITE+(NB_ALIENS*4)
#define SCREEN_MULTI    4
#define MAX_SHOOT_NUM   10
#define SHOOT_SPEED     6

Aliens_t alienTroop[NB_ALIENS];
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

    set_sprite_data(45, 9, aliens);

    initrand(DIV_REG);
    for (i = 0; i<NB_ALIENS; i++) {
        set_sprite_tile(ALIENS_SPRITE+(i<<2), 45);
        alienTroop[i].active = 1;
        alienTroop[i].x = (rand() << 1) << SCREEN_MULTI;
        alienTroop[i].y = (rand() << 1) << SCREEN_MULTI;
        alienTroop[i].animStep = 0;
        alienTroop[i].spdx;
        alienTroop[i].spdy;
        set_sprite_tile(SHOOT_SPRITE+i, 53);
        alienTroop[i].shoot.active = 0;
    }
}

void alienMoves(int16_t xref, int16_t yref, int16_t shipX, int16_t shipY) {
    uint8_t i;
    int16_t tmpX, tmpY;

    for (i = 0; i<NB_ALIENS; i++) {
        if (shipX > alienTroop[i].x) alienTroop[i].spdx = 4; else alienTroop[i].spdx = -4;
        if (shipY > alienTroop[i].y) alienTroop[i].spdy = 4; else alienTroop[i].spdy = -4;

        alienTroop[i].x += alienTroop[i].spdx;
        alienTroop[i].y += alienTroop[i].spdy;

        tmpX = (alienTroop[i].x - xref) >> SCREEN_MULTI;
        tmpY = (alienTroop[i].y - yref) >> SCREEN_MULTI;
        
        if ((tmpX>=0) && (tmpX<=176) && (tmpY>=0) && (tmpY<=160)) {
            if (!alienTroop[i].shoot.active) {
                alienTroop[i].shoot.active = TRUE;
                alienTroop[i].shoot.x = alienTroop[i].x;
                alienTroop[i].shoot.y = alienTroop[i].y;
                alienTroop[i].shoot.spdx = 0;
                if (shipX > alienTroop[i].x) alienTroop[i].shoot.spdx = SHOOT_SPEED;
                else if (shipX < alienTroop[i].x) alienTroop[i].shoot.spdx = -SHOOT_SPEED;
                alienTroop[i].shoot.spdy = 0;
                if (shipY > alienTroop[i].y) alienTroop[i].shoot.spdy = SHOOT_SPEED;
                else if (shipY < alienTroop[i].y) alienTroop[i].shoot.spdy = -SHOOT_SPEED;
            }

            // EMU_printf("MobX: %d, MobY: %d", tmpX, tmpY);
            alienTroop[i].animStep = abs(alienTroop[i].animStep -1);
            move_metasprite(alienFrames[alienTroop[i].animStep >> 7], 0, ALIENS_SPRITE+(i<<2), tmpX, tmpY);
            alienTroop[i].animStep += 16;
            // EMU_printf("Anim: %d", alienTroop[i].animStep >> 7);
        } else hide_metasprite(alienFrames[0], ALIENS_SPRITE+(i<<2));

        if (alienTroop[i].shoot.active) {
            alienTroop[i].shoot.x += alienTroop[i].shoot.spdx;
            alienTroop[i].shoot.y += alienTroop[i].shoot.spdy;
            tmpX = (alienTroop[i].shoot.x - xref) >> SCREEN_MULTI;
            tmpY = (alienTroop[i].shoot.y - yref) >> SCREEN_MULTI;
            // EMU_printf("ShootX: %d, ShootY: %d", tmpX, tmpY);
            if ((tmpX>=0) && (tmpX<=176) && (tmpY>=0) && (tmpY<=160)) {
                move_sprite(SHOOT_SPRITE+i, tmpX, tmpY);
            } else alienTroop[i].shoot.active = FALSE;
        }
    }
}
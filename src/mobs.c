#include <gb/gb.h>
#include <gbdk/emu_debug.h>
#include <gbdk/platform.h>
#include <gbdk/metasprites.h>
#include <stdlib.h>
#include <rand.h>
#include "../res/aliens.h"

#define NB_ALIENS       1
#define ALIENS_SPRITE   10
#define SCREEN_MULTI    4

typedef struct Aliens Aliens_t;
struct Aliens {
    uint8_t active;
    int8_t spdx, spdy;
    int16_t x,y;
    uint8_t animStep;
};

Aliens_t alienTroop[NB_ALIENS];

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

    set_sprite_data(45, 8, aliens);

    initrand(DIV_REG);
    for (i = 0; i<NB_ALIENS; i++) {
        set_sprite_tile(ALIENS_SPRITE+(i<<2), 45);
        alienTroop[i].active = 1;
        alienTroop[i].x = (rand() << 1) << SCREEN_MULTI;
        alienTroop[i].y = (rand() << 1) << SCREEN_MULTI;
        alienTroop[i].animStep = 0;
    }
}

void alienMoves(int16_t xref, int16_t yref) {
    uint8_t i;
    int16_t tmpX, tmpY;

    for (i = 0; i<NB_ALIENS; i++) {
        tmpX = (alienTroop[i].x - xref) >> SCREEN_MULTI;
        tmpY = (alienTroop[i].y - yref) >> SCREEN_MULTI;
        
        if ((tmpX>=0) && (tmpX<=176) && (tmpY>=0) && (tmpY<=160)) {
            // EMU_printf("MobX: %d, MobY: %d", tmpX, tmpY);
            alienTroop[i].animStep = abs(alienTroop[i].animStep -1);
            move_metasprite(alienFrames[alienTroop[i].animStep >> 7], 0, ALIENS_SPRITE+(i<<2), tmpX, tmpY);
            alienTroop[i].animStep += 16;
            // EMU_printf("Anim: %d", alienTroop[i].animStep >> 7);
        } else hide_metasprite(alienFrames[0], ALIENS_SPRITE+(i<<2));
    }
}
#include <gb/gb.h>
#include <stdint.h>
#include <gbdk/platform.h>
#include <gbdk/metasprites.h>
#include "GraphLib.h"
#include "player.h"
#include "mobs.h"

#include "../res/scene_tiles.h"
#include "../res/scene1.h"

Scene_t scene;

void init_gfx() {
    SPRITES_8x8;
    DISPLAY_OFF;

    set_bkg_data(0, 10, scene_tiles);
    setupScene(&scene, Scene1, Scene1Width, Scene1Height);

    initPlayer();
    initAliens();

    SHOW_BKG;
    SHOW_SPRITES;
    DISPLAY_ON;
}

void main(void)
{
	init_gfx();

    SCX_REG = 0; SCY_REG = 0;
    while(1) {
        playerMove();
        // alienMoves(&scene, &(player->entity->coord));
        updateView(&scene);
    }
}

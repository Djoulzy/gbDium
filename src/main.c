#include <gb/gb.h>
#include <gbdk/emu_debug.h>
#include <stdint.h>

#include <gbdk/platform.h>
#include <gbdk/metasprites.h>
#include "GraphLib.h"
#include "player.h"
#include "mobs.h"

#include "../res/scene_tiles.h"
#include "../res/scene1.h"

Scene_t scene;
Entity_t player;

void init_gfx() {
    SPRITES_8x8;
    DISPLAY_OFF;

    // Load Background tiles and then map
    set_bkg_data(0, 10, scene_tiles);


    setupScene(&scene, Scene1, Scene1Width, Scene1Height);

    initPlayer();
    initAliens();

	// Turn the background map on to make it visible
    SHOW_BKG;
    SHOW_SPRITES;
    DISPLAY_ON;
}

void main(void)
{
	init_gfx();

    // EMU_printf("shoots: %u", ship_shoot[0]->spriteNum);
    // EMU_printf("shoots: %u", player.bullets[0]->spriteNum);
    // dumpEntity(&player);

    SCX_REG = 0; SCY_REG = 0;
    // Loop forever
    while(1) {
		// Game main loop processing goes here

        moveEntityBullets(&scene, &player);

        alienMoves(&scene, &(player.coord));

        updateView(&scene);
    }
}

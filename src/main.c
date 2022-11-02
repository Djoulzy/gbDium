#include <gb/gb.h>
#include <stdint.h>
#include <gbdk/platform.h>
#include <gbdk/metasprites.h>
#include "../res/Ship_16x16.h"


void init_gfx() {
    // Load Background tiles and then map
    // set_bkg_data(0, 79u, dungeon_tiles);
    // set_bkg_tiles(0, 0, 32u, 32u, dungeon_mapPLN0);

    SPRITES_8x16;
    set_sprite_data(0, 19, Ship);
    // set_sprite_tile(0, 0);
    // set_sprite_tile(1, 2);
    // move_sprite(0, 70, 75);
    // move_sprite(1, 78, 75);

    const metasprite_t ship_meta0[] = {
        METASPR_ITEM(0, 0, 0, 0), METASPR_ITEM(0, 8, 2, 0), METASPR_ITEM(0, 0, 4, 0), METASPR_ITEM(0, 8, 6, 0),
        METASPR_TERM
    };
    
    const metasprite_t* const ship_meta[1] = {
        ship_meta0
    };

    move_metasprite(ship_meta[0], 0, 0, 70,75);
	// Turn the background map on to make it visible
    // SHOW_BKG;
    SHOW_SPRITES;
}


void main(void)
{
	init_gfx();

    // Loop forever
    while(1) {
		// Game main loop processing goes here

		// Done processing, yield CPU and wait for start of next frame
        wait_vbl_done();
    }
}

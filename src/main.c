#include <gb/gb.h>
#include <stdint.h>
#include <gbdk/platform.h>
#include <gbdk/metasprites.h>
#include "../res/setSprites.h"

#define SHIP_SPRITE     0

joypads_t joypads;

void init_gfx() {
    // Load Background tiles and then map
    // set_bkg_data(0, 79u, dungeon_tiles);
    // set_bkg_tiles(0, 0, 32u, 32u, dungeon_mapPLN0);

    SPRITES_8x16;
    set_sprite_data(0, 20, Ship);

	// Turn the background map on to make it visible
    // SHOW_BKG;
    SHOW_SPRITES;
}


void main(void)
{
    uint16_t ShipX = 70, ShipY = 70;
    uint8_t hiwater = 0;

	init_gfx();

    joypad_init(1, &joypads);
    // Loop forever
    while(1) {
		// Game main loop processing goes here

        joypad_ex(&joypads);
        if (joypads.joy0 & J_UP) {
            ShipY--;
        } else if (joypads.joy0 & J_DOWN) {
            ShipY++;
        }

        if (joypads.joy0 & J_LEFT) {
            ShipX--;
        } else if (joypads.joy0 & J_RIGHT) {
            ShipX++;
        }
    
        hiwater = move_metasprite(ship_meta[0], 0, SHIP_SPRITE, ShipX, ShipY);
        hide_sprites_range(hiwater, 40);  

		// Done processing, yield CPU and wait for start of next frame
        wait_vbl_done();
    }
}

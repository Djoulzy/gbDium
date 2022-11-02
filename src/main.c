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
    uint16_t ShipX, ShipY, inclinaison;
    int16_t SpdX, SpdY, direction;
    uint8_t hiwater = 0;

	init_gfx();

    joypad_init(1, &joypads);

    ShipX = ShipY = 64 << 4;
    SpdX = SpdY = direction = 0;

    // Loop forever
    while(1) {
		// Game main loop processing goes here

        joypad_ex(&joypads);
        if (joypads.joy0 & J_UP) {
            // SpdY -= 2;
            // if (SpdY < -32) SpdY = -32;
        } else if (joypads.joy0 & J_DOWN) {
            // SpdY += 2;
            // if (SpdY > 32) SpdY = 32;
        }

        if (joypads.joy0 & J_LEFT) {
            SpdX -= 2;
            if (SpdX < -32) SpdX = -32;
            direction = -1;
        } else if (joypads.joy0 & J_RIGHT) {
            SpdX += 2;
            if (SpdX > 32) SpdX = 32;
            direction = 1;
        }
        ShipX += SpdX, ShipY += SpdY;

    

        if (direction >= 0)
            hiwater = move_metasprite(ship_meta[0], 0, SHIP_SPRITE, (ShipX >> 4), (ShipY >> 4));
        else
            hiwater = move_metasprite_vflip(ship_meta[0], 0, SHIP_SPRITE, (ShipX >> 4), (ShipY >> 4));
        // hide_sprites_range(hiwater, 40);  

		// Done processing, yield CPU and wait for start of next frame
        wait_vbl_done();
    }
}

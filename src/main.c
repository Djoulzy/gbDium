#include <gb/gb.h>
#include <gbdk/emu_debug.h>
#include <stdint.h>
#include <stdlib.h>
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
    uint16_t ShipX, ShipY;
    int16_t SpdX, SpdY, direction, inclinaison;
    uint8_t hiwater = 0;

	init_gfx();

    joypad_init(1, &joypads);

    ShipX = ShipY = 64 << 4;
    SpdX = SpdY = direction = inclinaison = 0;

    // Loop forever
    while(1) {
		// Game main loop processing goes here

        joypad_ex(&joypads);
        if (joypads.joy0 & J_UP) {
            SpdY -= 2;
            if (SpdY < -16) SpdY = -16;
            inclinaison++;
            if (inclinaison > 4) inclinaison = 4;
        } else if (joypads.joy0 & J_DOWN) {
            SpdY += 2;
            if (SpdY > 16) SpdY = 16;
            inclinaison--;
            if (inclinaison < -4) inclinaison = -4;
        } else inclinaison = 0;

        if (joypads.joy0 & J_LEFT) {
            SpdX -= 2;
            if (SpdX < -16) SpdX = -16;
            direction = -1;
        } else if (joypads.joy0 & J_RIGHT) {
            SpdX += 2;
            if (SpdX > 16) SpdX = 16;
            direction = 1;
        }
        ShipX += SpdX, ShipY += SpdY;

        if (direction >= 0) {
            if (inclinaison >= 0)
                hiwater = move_metasprite(ship_meta[abs(inclinaison)], 0, SHIP_SPRITE, (ShipX >> 4), (ShipY >> 4));
            else {
                EMU_printf("%d", inclinaison);
                hiwater = move_metasprite_hflip(ship_meta[abs(inclinaison)], 0, SHIP_SPRITE, (ShipX >> 4), (ShipY >> 4)+16);
            }
        }
        else {
            if (inclinaison >= 0)
                hiwater = move_metasprite_vflip(ship_meta[abs(inclinaison)], 0, SHIP_SPRITE, (ShipX >> 4), (ShipY >> 4));
            else hiwater = move_metasprite_hvflip(ship_meta[abs(inclinaison)], 0, SHIP_SPRITE, (ShipX >> 4), (ShipY >> 4)+16);
        }
        // hide_sprites_range(hiwater, 40);  

		// Done processing, yield CPU and wait for start of next frame
        wait_vbl_done();
    }
}

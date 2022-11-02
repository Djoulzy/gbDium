#include <gb/gb.h>
#include <gbdk/emu_debug.h>
#include <stdint.h>
#include <stdlib.h>
#include <gbdk/platform.h>
#include <gbdk/metasprites.h>
#include "../res/setSprites.h"

#define SHIP_SPRITE     0
#define MAX_SHIP_SPEED  16
#define SHIP_ACCEL      2

joypads_t joypads;

void init_gfx() {
    // Load Background tiles and then map
    // set_bkg_data(0, 79u, dungeon_tiles);
    // set_bkg_tiles(0, 0, 32u, 32u, dungeon_mapPLN0);

    SPRITES_8x16;
    set_sprite_data(0, 44, Ship);

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

        move_metasprite(ship_retournement[0], 0, 2, 20, 20);
        move_metasprite(ship_retournement[1], 0, 4, 40, 22);
        move_metasprite(ship_retournement[2], 0, 6, 60, 24);
        move_metasprite(ship_retournement[3], 0, 8, 80, 26);
        move_metasprite(ship_retournement[4], 0, 10, 100, 20);
        move_metasprite(ship_retournement[5], 0, 12, 120, 40);

        joypad_ex(&joypads);
        if (joypads.joy0 & J_UP) {
            SpdY -= SHIP_ACCEL;
            if (SpdY < - MAX_SHIP_SPEED) SpdY = - MAX_SHIP_SPEED;
            inclinaison++;
            if (inclinaison > 16) inclinaison = 16;
        } else if (joypads.joy0 & J_DOWN) {
            SpdY += SHIP_ACCEL;
            if (SpdY > MAX_SHIP_SPEED) SpdY = MAX_SHIP_SPEED;
            inclinaison--;
            if (inclinaison < -16) inclinaison = -16;
        } else if (inclinaison != 0) {
            if (inclinaison > 0) inclinaison--;
            else inclinaison++;
        }

        if (joypads.joy0 & J_LEFT) {
            SpdX -= SHIP_ACCEL;
            if (SpdX < - MAX_SHIP_SPEED) SpdX = - MAX_SHIP_SPEED;
            direction = -1;
        } else if (joypads.joy0 & J_RIGHT) {
            SpdX += SHIP_ACCEL;
            if (SpdX > MAX_SHIP_SPEED) SpdX = MAX_SHIP_SPEED;
            direction = 1;
        }
        ShipX += SpdX, ShipY += SpdY;

        if (direction >= 0) {
            if (inclinaison >= 0)
                hiwater = move_metasprite(ship_meta[abs(inclinaison >> 2)], 0, SHIP_SPRITE, (ShipX >> 4), (ShipY >> 4));
            else {
                EMU_printf("%d", inclinaison);
                hiwater = move_metasprite_hflip(ship_meta[abs(inclinaison >> 2)], 0, SHIP_SPRITE, (ShipX >> 4), (ShipY >> 4)+16);
            }
        }
        else {
            if (inclinaison >= 0)
                hiwater = move_metasprite_vflip(ship_meta[abs(inclinaison >> 2)], 0, SHIP_SPRITE, (ShipX >> 4), (ShipY >> 4));
            else hiwater = move_metasprite_hvflip(ship_meta[abs(inclinaison >> 2)], 0, SHIP_SPRITE, (ShipX >> 4), (ShipY >> 4)+16);
        }
        // hide_sprites_range(hiwater, 40);  

		// Done processing, yield CPU and wait for start of next frame
        wait_vbl_done();
    }
}

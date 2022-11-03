#include <gb/gb.h>
#include <gbdk/emu_debug.h>
#include <stdint.h>
#include <stdlib.h>
#include <gbdk/platform.h>
#include <gbdk/metasprites.h>
#include "../res/setSprites.h"

#define SHIP_SPRITE     0
#define SHOOT_SPRITE    4
#define MAX_SHIP_SPEED  16
#define SHIP_ACCEL      2

joypads_t joypads;

typedef struct ShipShoot ShipShoot;
struct ShipShoot {
    uint8_t active;
    uint8_t sprite_num;
    int8_t dir;
    int16_t x,y;
};

void init_gfx() {
    // Screen : 160x144

    // Load Background tiles and then map
    // set_bkg_data(0, 79u, dungeon_tiles);
    // set_bkg_tiles(0, 0, 32u, 32u, dungeon_mapPLN0);

    SPRITES_8x8;
    set_sprite_data(0, 45, Ship);
    set_sprite_tile(SHOOT_SPRITE, 44);
    move_sprite(SHOOT_SPRITE, 10, 10);

	// Turn the background map on to make it visible
    // SHOW_BKG;
    SHOW_SPRITES;
}

void main(void)
{
    uint16_t ShipX, ShipY, retourn_anim;
    int16_t SpdX, SpdY;
    int8_t direction, inclinaison;
    uint8_t hiwater, retournement = 0;
    ShipShoot ship_shoot;

	init_gfx();

    joypad_init(1, &joypads);

    ShipX = ShipY = 64 << 4;
    SpdX = SpdY = inclinaison = retourn_anim = 0;
    direction = 1;

    // Loop forever
    while(1) {
		// Game main loop processing goes here

        joypad_ex(&joypads);

        if (joypads.joy0 & J_A) {
            EMU_printf("A PRESSED");
            ship_shoot.dir = direction;
            ship_shoot.active = 1;
            ship_shoot.sprite_num = SHOOT_SPRITE;
            ship_shoot.x = ShipX;
            ship_shoot.y = ShipY;
        }

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
            if ((direction == 1) && (!retournement)) {
                // EMU_printf("Go LEFT");
                retournement = 1;
                retourn_anim = 0;
            }
        } else if (joypads.joy0 & J_RIGHT) {
            SpdX += SHIP_ACCEL;
            if (SpdX > MAX_SHIP_SPEED) SpdX = MAX_SHIP_SPEED;
            if ((direction == -1) && (!retournement)) {
                // EMU_printf("Go RIGHT");
                retournement = 1;
                retourn_anim = 0;
            }
        }
        ShipX += SpdX, ShipY += SpdY;

        if (retournement) {
            if (direction >= 0)
                hiwater = move_metasprite(ship_retournement[retourn_anim >> 2], 0, SHIP_SPRITE, (ShipX >> 4), (ShipY >> 4));
            else
                hiwater = move_metasprite_vflip(ship_retournement[retourn_anim >> 2], 0, SHIP_SPRITE, (ShipX >> 4), (ShipY >> 4));
            retourn_anim++;
            if (retourn_anim>20) {
                retournement = 0;
                inclinaison = 16;
                direction *= -1;
            }
        } else {
            if (direction >= 0) {
                if (inclinaison >= 0)
                    hiwater = move_metasprite(ship_meta[abs(inclinaison >> 2)], 0, SHIP_SPRITE, (ShipX >> 4), (ShipY >> 4));
                else {
                    hiwater = move_metasprite_hflip(ship_meta[abs(inclinaison >> 2)], 0, SHIP_SPRITE, (ShipX >> 4), (ShipY >> 4)+16);
                }
            }
            else {
                if (inclinaison >= 0)
                    hiwater = move_metasprite_vflip(ship_meta[abs(inclinaison >> 2)], 0, SHIP_SPRITE, (ShipX >> 4), (ShipY >> 4));
                else hiwater = move_metasprite_hvflip(ship_meta[abs(inclinaison >> 2)], 0, SHIP_SPRITE, (ShipX >> 4), (ShipY >> 4)+16);
            }
        }
        // hide_sprites_range(hiwater, 40);

        if (ship_shoot.active) {
            if (ship_shoot.dir >= 0) ship_shoot.x++;
            else ship_shoot.x--;
            if ((ship_shoot.x < 0) || (ship_shoot.x > 160))
                ship_shoot.active = 0;
            else
                move_sprite(ship_shoot.sprite_num, ship_shoot.x, ship_shoot.y);
        }

		// Done processing, yield CPU and wait for start of next frame
        wait_vbl_done();
    }
}

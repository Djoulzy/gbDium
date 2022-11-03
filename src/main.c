#include <gb/gb.h>
#include <gbdk/emu_debug.h>
#include <stdint.h>
#include <stdlib.h>
#include <gbdk/platform.h>
#include <gbdk/metasprites.h>
#include "../res/setSprites.h"
#include "../res/backgrounds.h"
#include "../res/scene1.h"

#define SHIP_SPRITE     0
#define SHOOT_SPRITE    4
#define MAX_SHOOT_NUM   6
#define SHOOT_DELAY     10
#define MAX_SHIP_SPEED  16
#define SHIP_ACCEL      2

joypads_t joypads;

typedef struct ShipShoot ShipShoot_t;
struct ShipShoot {
    uint8_t active;
    int8_t dir;
    int16_t x,y;
};

void init_gfx() {
    // Screen : 160x144
    SPRITES_8x8;

    // Load Background tiles and then map
    set_bkg_data(0, 10, backgrounds);
    set_bkg_tiles(0, 0, 32, 32, Scene1);
    set_sprite_data(0, 45, Ship);

	// Turn the background map on to make it visible
    SHOW_BKG;
    SHOW_SPRITES;
}

void main(void)
{
    uint16_t ShipX, ShipY, retourn_anim;
    int16_t SpdX, SpdY;
    int8_t direction, inclinaison;
    uint8_t i, hiwater, next_shoot = 0, shoot_delay = 0, retournement = 0;
    ShipShoot_t ship_shoot[MAX_SHOOT_NUM];

	init_gfx();

    joypad_init(1, &joypads);

    ShipX = ShipY = 64 << 4;
    SpdX = SpdY = inclinaison = retourn_anim = 0;
    direction = 1;

    for (i = 0; i<MAX_SHOOT_NUM; i++) {
        set_sprite_tile(SHOOT_SPRITE+i, 44);
        ship_shoot[i].active = 0;
    }

    // Loop forever
    while(1) {
		// Game main loop processing goes here

        joypad_ex(&joypads);

        if (joypads.joy0 & J_A) {
            if ((next_shoot < MAX_SHOOT_NUM) && (!shoot_delay) && (!retournement)) {
                EMU_printf("SHOOT !");
                ship_shoot[next_shoot].dir = direction;
                ship_shoot[next_shoot].active = 1;
                ship_shoot[next_shoot].x = ShipX >> 4;
                ship_shoot[next_shoot].y = (ShipY >> 4) + 4;
                if (direction < 0) set_sprite_prop(SHOOT_SPRITE+next_shoot, S_FLIPX);
                shoot_delay = SHOOT_DELAY;
            }
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

        next_shoot = 100;
        for (i = 0; i<MAX_SHOOT_NUM; i++) {
            if (ship_shoot[i].active) {
                if (ship_shoot[i].dir >= 0) ship_shoot[i].x += 4;
                else ship_shoot[i].x -= 4;
                move_sprite(SHOOT_SPRITE+i, ship_shoot[i].x, ship_shoot[i].y);
                if ((ship_shoot[i].x < -9) || (ship_shoot[i].x > 168)) {
                    ship_shoot[i].active = 0;
                    next_shoot = i;
                }
            } else next_shoot = i;
        }

		// Done processing, yield CPU and wait for start of next frame
        if (shoot_delay) shoot_delay--;
        EMU_printf("Delay %d", shoot_delay);
        wait_vbl_done();
    }
}

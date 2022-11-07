#include <gb/gb.h>
#include <gbdk/emu_debug.h>
#include <stdint.h>
#include <stdlib.h>
#include <gbdk/platform.h>
#include <gbdk/metasprites.h>
#include "GraphLib.h"
#include "mobs.h"
#include "../res/setSprites.h"
#include "../res/scene_tiles.h"
#include "../res/scene1.h"

#define SHIP_SPRITE         0
#define SHOOT_SPRITE        4
#define MAX_SHOOT_NUM       6
#define SHOOT_DELAY         10
#define MAX_SHIP_SPEED      32
#define SHIP_ACCEL          2
#define SHOOT_SPEED         4
#define SHIP_RETURN_FRAME   5

#define MIN(A,B) ((A)<(B)?(A):(B))

joypads_t joypads;
Scene_t scene;
Entity_t player;

void init_gfx() {
    SPRITES_8x8;
    DISPLAY_OFF;

    // Load Background tiles and then map
    set_bkg_data(0, 10, scene_tiles);
    set_sprite_data(0, 45, Ship);

    setupScene(&scene, Scene1, Scene1Width, Scene1Height);
    setupEntity(&player, ship_meta, scene.startScrollZoneX, scene.startScrollZoneY);
    setCameraStick(&player);

    // initAliens();
	// Turn the background map on to make it visible
    SHOW_BKG;
    SHOW_SPRITES;
    DISPLAY_ON;
}

void main(void)
{
    int16_t visibleX, visibleY;
    uint16_t retourn_anim;
    int8_t direction, inclinaison;
    uint8_t i, next_shoot = 0, shoot_delay = 0;
    uint8_t retournement = FALSE;
    Shoot_t ship_shoot[MAX_SHOOT_NUM];

	init_gfx();

    joypad_init(1, &joypads);

    inclinaison = retourn_anim = 0;
    direction = 1;

    // for (i = 0; i<MAX_SHOOT_NUM; i++) {
    //     set_sprite_tile(SHOOT_SPRITE+i, 44);
    //     ship_shoot[i].active = 0;
    // }

    SCX_REG = 0; SCY_REG = 0;
    // Loop forever
    while(1) {
		// Game main loop processing goes here

        joypad_ex(&joypads);
        if (joypads.joy0 & J_SELECT) {
            player.speedX = player.speedY = 0;
        }

        if (joypads.joy0 & J_UP) {
            player.speedY -= SHIP_ACCEL;
            if (player.speedY < - MAX_SHIP_SPEED) player.speedY = - MAX_SHIP_SPEED;
            inclinaison++;
            if (inclinaison > 16) inclinaison = 16;
        } else if (joypads.joy0 & J_DOWN) {
            player.speedY += SHIP_ACCEL;
            if (player.speedY > MAX_SHIP_SPEED) player.speedY = MAX_SHIP_SPEED;
            inclinaison--;
            if (inclinaison < -16) inclinaison = -16;
        } else if (inclinaison != 0) {
            if (inclinaison > 0) inclinaison--;
            else inclinaison++;
        }

        if ((joypads.joy0 & J_LEFT) && (!retournement)) {
            if (direction == 1) {
                // EMU_printf("Go LEFT");
                retournement = TRUE;
                retourn_anim = 0;
                player.speedX += SHIP_ACCEL;
            } else {
                player.speedX -= SHIP_ACCEL;
                if (player.speedX < - MAX_SHIP_SPEED) player.speedX = - MAX_SHIP_SPEED;
            }
        } else if ((joypads.joy0 & J_RIGHT)&& (!retournement)) {
            if (direction == -1) {
                // EMU_printf("Go RIGHT");
                retournement = TRUE;
                retourn_anim = 0;
                player.speedX -= SHIP_ACCEL;
            } else {
                player.speedX += SHIP_ACCEL;
                if (player.speedX > MAX_SHIP_SPEED) player.speedX = MAX_SHIP_SPEED;
            }
        }

        updateEntityPos(&scene, &player);
        if (isOutOfScene(&scene, &player) & (OUT_LEFT|OUT_RIGHT)) {
            retournement = TRUE;
            retourn_anim = 0;
        };

        // if (joypads.joy0 & J_A) {
        //     if ((next_shoot < MAX_SHOOT_NUM) && (!shoot_delay) && (!retournement)) {
        //         EMU_printf("SHOOT !");
        //         ship_shoot[next_shoot].spdx = direction * SHOOT_SPEED;
        //         ship_shoot[next_shoot].active = 1;
        //         ship_shoot[next_shoot].x = tmpX;
        //         ship_shoot[next_shoot].y = tmpY + 4;
        //         if (direction < 0) set_sprite_prop(SHOOT_SPRITE+next_shoot, S_FLIPX);
        //         shoot_delay = SHOOT_DELAY;
        //         next_shoot = MAX_SHOOT_NUM;
        //     }
        // }

        if (retournement) {
            if (direction >= 0)
                move_metasprite(ship_meta[SHIP_RETURN_FRAME + (retourn_anim >> 2)], 0, player.spriteNum, player.visibleX, player.visibleY);
            else
                move_metasprite_vflip(ship_meta[SHIP_RETURN_FRAME + (retourn_anim >> 2)], 0, player.spriteNum, player.visibleX, player.visibleY);
            retourn_anim++;
            if (retourn_anim > 20) {
                retournement = FALSE;
                inclinaison = 16;
                direction *= -1;
                player.speedX = SHIP_ACCEL * direction;
            }
        } else {
            if (direction >= 0) {
                if (inclinaison >= 0)
                    move_metasprite(ship_meta[abs(inclinaison >> 2)], 0, player.spriteNum, player.visibleX, player.visibleY);
                else {
                    move_metasprite_hflip(ship_meta[abs(inclinaison >> 2)], 0, player.spriteNum, player.visibleX, player.visibleY+16);
                }
            }
            else {
                if (inclinaison >= 0)
                    move_metasprite_vflip(ship_meta[abs(inclinaison >> 2)], 0, player.spriteNum, player.visibleX, player.visibleY);
                else move_metasprite_hvflip(ship_meta[abs(inclinaison >> 2)], 0, player.spriteNum, player.visibleX, player.visibleY+16);
            }
        }

        // next_shoot = 100;
        // for (i = 0; i<MAX_SHOOT_NUM; i++) {
        //     if (ship_shoot[i].active) {
        //         ship_shoot[i].x += ship_shoot[i].spdx;
        //         move_sprite(SHOOT_SPRITE+i, ship_shoot[i].x, ship_shoot[i].y);
        //         if ((ship_shoot[i].x < -9) || (ship_shoot[i].x > 168)) {
        //             ship_shoot[i].active = 0;
        //             next_shoot = i;
        //         }
        //     } else next_shoot = i;
        // }

		// Done processing, yield CPU and wait for start of next frame
        if (shoot_delay) shoot_delay--;

        // alienMoves(scene.camera_x, scene.camera_y, ShipAbsX, ShipAbsY);

        updateView(&scene);
    }
}

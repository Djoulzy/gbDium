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
Bullet_t* ship_shoot[MAX_SHOOT_NUM];

void init_gfx() {
    SPRITES_8x8;
    DISPLAY_OFF;

    // Load Background tiles and then map
    set_bkg_data(0, 10, scene_tiles);
    set_sprite_data(0, 45, Ship);

    setupScene(&scene, Scene1, Scene1Width, Scene1Height);
    setupEntity(&player, ship_meta, scene.startScrollZoneX, scene.startScrollZoneY);
    setCameraStick(&player);

	// Turn the background map on to make it visible
    SHOW_BKG;
    SHOW_SPRITES;
    DISPLAY_ON;
}

void main(void)
{
    uint8_t props;
    uint16_t retourn_anim;
    int8_t direction, inclinaison;
    uint8_t i, next_shoot = 0, shoot_delay = 0;
    uint8_t retournement = FALSE;
    int8_t out = 0;

	init_gfx();

    joypad_init(1, &joypads);

    inclinaison = retourn_anim = 0;
    direction = 1;

    for (i = 0; i<MAX_SHOOT_NUM; i++) {
        ship_shoot[i] = allocBullet(44);
        // setupShoot(ship_shoot[i], 44);
    }
    assignBulletsToEntity(&player, ship_shoot, MAX_SHOOT_NUM, SHOOT_DELAY);

    // EMU_printf("shoots: %u", ship_shoot[0]->spriteNum);
    // EMU_printf("shoots: %u", player.bullets[0]->spriteNum);
    // dumpEntity(&player);

    initAliens();

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
        out = isOutOfViewPort(&scene, &player.coord);
        if (out) {
            if (out & (OUT_LEFT|OUT_RIGHT)) {
                retournement = TRUE;
                retourn_anim = 0;
                player.speedX = 0;
            } else player.speedY = 0;
        }

        if (joypads.joy0 & J_A) {
            if (!retournement) {
                if (direction < 0) props = S_FLIPX;
                else props = 0;
                entityShoot(&player, direction * SHOOT_SPEED, 0, props);
            }
        }

        if (retournement) {
            if (direction >= 0)
                move_metasprite(ship_meta[SHIP_RETURN_FRAME + (retourn_anim >> 2)], 0, player.spriteNum, player.coord.visibleX, player.coord.visibleY);
            else
                move_metasprite_vflip(ship_meta[SHIP_RETURN_FRAME + (retourn_anim >> 2)], 0, player.spriteNum, player.coord.visibleX, player.coord.visibleY);
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
                    move_metasprite(ship_meta[abs(inclinaison >> 2)], 0, player.spriteNum, player.coord.visibleX, player.coord.visibleY);
                else {
                    move_metasprite_hflip(ship_meta[abs(inclinaison >> 2)], 0, player.spriteNum, player.coord.visibleX, player.coord.visibleY+16);
                }
            }
            else {
                if (inclinaison >= 0)
                    move_metasprite_vflip(ship_meta[abs(inclinaison >> 2)], 0, player.spriteNum, player.coord.visibleX, player.coord.visibleY);
                else move_metasprite_hvflip(ship_meta[abs(inclinaison >> 2)], 0, player.spriteNum, player.coord.visibleX, player.coord.visibleY+16);
            }
        }

        moveEntityBullets(&scene, &player);

        alienMoves(&scene, &player.coord);

        updateView(&scene);
    }
}

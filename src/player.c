#include <gb/gb.h>
#include <stdlib.h>
#include "GraphLib.h"

#include "../res/setSprites.h"

#define MAX_SHOOT_NUM       6
#define SHOOT_DELAY         10
#define MAX_SHIP_SPEED      32
#define SHIP_ACCEL          2
#define SHOOT_SPEED         4
#define SHIP_RETURN_FRAME   5

Entity_t player;
joypads_t joypads;
Bullet_t* ship_shoot[MAX_SHOOT_NUM];
extern Scene_t scene;

uint8_t props;
uint16_t retourn_anim;
int8_t direction, inclinaison;
uint8_t retournement = FALSE;
int8_t out = 0;

void initPlayer() {
    set_sprite_data(0, 45, Ship);

    setupEntity(&player, ship_meta, scene.startScrollZoneX, scene.startScrollZoneY);
    setCameraStick(&player);

    joypad_init(1, &joypads);

    inclinaison = retourn_anim = 0;
    direction = 1;

    for (uint8_t i = 0; i<MAX_SHOOT_NUM; i++) {
        ship_shoot[i] = allocBullet(44);
        // setupShoot(ship_shoot[i], 44);
    }
    assignBulletsToEntity(&player, ship_shoot, MAX_SHOOT_NUM, SHOOT_DELAY);
}

void playerMove() {

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

    if (joypads.joy0 & J_A) {
        if (!retournement) {
            if (direction < 0) props = S_FLIPX;
            else props = 0;
            entityShoot(&player, direction * SHOOT_SPEED, 0, props);
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
}
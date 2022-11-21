#include <gb/gb.h>
#include <stdlib.h>
#include "player.h"
#include "mobs.h"
#include "GraphLib.h"

#include "../res/setSprites.h"

EntityList_t* player;
joypads_t joypads;
BulletList_t* ship_shoot;
extern Scene_t scene;

uint8_t props;
int8_t direction;
uint8_t retournement = FALSE;
int8_t out = 0;

void initPlayer() {
    set_sprite_data(0, 54, Ship);

    player = addEntityToList(player, ship_meta, START_SCROLL_X, START_SCROLL_Y);
    setCameraStick(player->entity);
    player->entity->shootDelay = SHOOT_DELAY;

    prepareBulletList(player->entity, 52, MAX_SHOOT_NUM);
    joypad_init(1, &joypads);

    direction = 1;
    player->entity->animStep = 0;
    player->entity->anim = anim_inclinaison;
}

void playerMove() {

    joypad_ex(&joypads);
    if (joypads.joy0 & J_SELECT) {
        player->entity->speedX = player->entity->speedY = 0;
    }

    if (joypads.joy0 & J_UP) {
        player->entity->anim = anim_inclinaison;
        player->entity->speedY -= SHIP_ACCEL;
        if (player->entity->speedY < - MAX_SHIP_SPEED) player->entity->speedY = - MAX_SHIP_SPEED;
        player->entity->animStep++;
        if (player->entity->animStep > 19) player->entity->animStep = 19;
    } else if (joypads.joy0 & J_DOWN) {
        player->entity->anim = anim_inclinaison;
        player->entity->speedY += SHIP_ACCEL;
        if (player->entity->speedY > MAX_SHIP_SPEED) player->entity->speedY = MAX_SHIP_SPEED;
        player->entity->animStep--;
        if (player->entity->animStep < -19) player->entity->animStep = -19;
    } else if (!retournement && (player->entity->animStep != 0)) {
        if (player->entity->animStep > 0) player->entity->animStep--;
        else player->entity->animStep++;
    }

    if ((joypads.joy0 & J_LEFT) && (!retournement)) {
        if (direction == 1) {
            // EMU_printf("Go LEFT");
            retournement = TRUE;
            player->entity->anim = anim_retournement;
            player->entity->animStep = 0;
            player->entity->speedX += SHIP_ACCEL;
        } else {
            player->entity->speedX -= SHIP_ACCEL;
            if (player->entity->speedX < - MAX_SHIP_SPEED) player->entity->speedX = - MAX_SHIP_SPEED;
        }
    } else if ((joypads.joy0 & J_RIGHT)&& (!retournement)) {
        if (direction == -1) {
            // EMU_printf("Go RIGHT");
            retournement = TRUE;
            player->entity->anim = anim_retournement;
            player->entity->animStep = 0;
            player->entity->speedX -= SHIP_ACCEL;
        } else {
            player->entity->speedX += SHIP_ACCEL;
            if (player->entity->speedX > MAX_SHIP_SPEED) player->entity->speedX = MAX_SHIP_SPEED;
        }
    }

    if (joypads.joy0 & J_A) {
        if (!retournement) {
            if (direction < 0) props = S_FLIPX;
            else props = 0;
            if (shootOk(player->entity)) entityShoot(player->entity, direction * SHOOT_SPEED, 0, props);
        }
    }

    player->entity->coord.upscaledX += player->entity->speedX;
    player->entity->coord.upscaledY += player->entity->speedY;
    updatePlayerPos(&scene, player->entity);
    out = isOutOfScene(&scene, &player->entity->coord);
    if (out) {
        if (out & (OUT_LEFT|OUT_RIGHT)) {
            retournement = TRUE;
            player->entity->anim = anim_retournement;
            player->entity->animStep = 0;
            player->entity->speedX = 0;
        } else player->entity->speedY = 0;
    }

    if (retournement) {
        if (direction >= 0)
            move_metasprite(ship_meta[player->entity->anim[player->entity->animStep]], 0, player->entity->spriteNum, player->entity->coord.viewportX, player->entity->coord.viewportY);
        else
            move_metasprite_vflip(ship_meta[player->entity->anim[player->entity->animStep]], 0, player->entity->spriteNum, player->entity->coord.viewportX, player->entity->coord.viewportY);
        player->entity->animStep++;
        if (player->entity->animStep > 23) {
            retournement = FALSE;
            player->entity->anim = anim_inclinaison;
            player->entity->animStep = 19;
            direction *= -1;
            player->entity->speedX = SHIP_ACCEL * direction;
        }
    } else {
        if (direction >= 0) {
            if (player->entity->animStep >= 0)
                move_metasprite(ship_meta[player->entity->anim[abs(player->entity->animStep)]], 0, player->entity->spriteNum, player->entity->coord.viewportX, player->entity->coord.viewportY);
            else {
                move_metasprite_hflip(ship_meta[player->entity->anim[abs(player->entity->animStep)]], 0, player->entity->spriteNum, player->entity->coord.viewportX, player->entity->coord.viewportY+16);
            }
        }
        else {
            if (player->entity->animStep >= 0)
                move_metasprite_vflip(ship_meta[player->entity->anim[abs(player->entity->animStep)]], 0, player->entity->spriteNum, player->entity->coord.viewportX, player->entity->coord.viewportY);
            else move_metasprite_hvflip(ship_meta[player->entity->anim[abs(player->entity->animStep)]], 0, player->entity->spriteNum, player->entity->coord.viewportX, player->entity->coord.viewportY+16);
        }
    }

    moveEntityBullets(&scene, player->entity, alienList);
}
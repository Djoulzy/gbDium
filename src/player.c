#include <gbdk/metasprites.h>
#include "player.h"
#include "../res/setSprites.h"

Player_t player;

void initPlayer(void) {
    uint8_t i;
    set_sprite_data(0, 53, Ship);
    player.entity.X = START_SCROLL_X;
    player.entity.Y = START_SCROLL_Y;
    player.entity.active = TRUE;
    player.entity.overlapped = FALSE;
    player.entity.animStep = 0;
    player.upscaledX = START_SCROLL_X << SCREEN_SCALE;
    player.upscaledY = START_SCROLL_Y << SCREEN_SCALE;
    player.speedX = player.speedY = 0;
    player.direction = 1;
    player.tilt = 0;
    player.turning = player.turnFrame = player.shootCooldown = 0;
    for (i = 0; i != MAX_SHOOT_NUM; ++i) {
        playerBullets[i].active = FALSE;
        set_sprite_tile(PLAYER_BULLET_SPRITE + i, 52);
        set_sprite_prop(PLAYER_BULLET_SPRITE + i, 0);
        hide_sprite(PLAYER_BULLET_SPRITE + i);
    }
}

void playerMove(void) {
    uint8_t keys = joypad();
    uint8_t frame, props = 0;
    uint8_t i;
    int8_t tilt;
    Bullet_t *bullet;
    int16_t x, y;

    if (player.shootCooldown) --player.shootCooldown;
    if (keys & J_SELECT) player.speedX = player.speedY = 0;

    if (keys & J_UP) {
        if (player.speedY > -MAX_SHIP_SPEED) player.speedY -= SHIP_ACCEL;
        if (!player.turning && player.tilt < 19) ++player.tilt;
    } else if (keys & J_DOWN) {
        if (player.speedY < MAX_SHIP_SPEED) player.speedY += SHIP_ACCEL;
        if (!player.turning && player.tilt > -19) --player.tilt;
    } else if (!player.turning) {
        if (player.tilt > 0) --player.tilt;
        else if (player.tilt < 0) ++player.tilt;
    }

    if (!player.turning) {
        if (keys & J_LEFT) {
            if (player.direction > 0) {
                player.turning = TRUE;
                player.turnFrame = 0;
                if (player.speedX < MAX_SHIP_SPEED) player.speedX += SHIP_ACCEL;
            } else if (player.speedX > -MAX_SHIP_SPEED) player.speedX -= SHIP_ACCEL;
        } else if (keys & J_RIGHT) {
            if (player.direction < 0) {
                player.turning = TRUE;
                player.turnFrame = 0;
                if (player.speedX > -MAX_SHIP_SPEED) player.speedX -= SHIP_ACCEL;
            } else if (player.speedX < MAX_SHIP_SPEED) player.speedX += SHIP_ACCEL;
        }
    }

    player.upscaledX += player.speedX;
    player.upscaledY += player.speedY;
    player.entity.X = player.upscaledX >> SCREEN_SCALE;
    player.entity.Y = player.upscaledY >> SCREEN_SCALE;

    if (player.entity.X < (int16_t)SCENE_BORDER_SIZE || player.entity.X > (int16_t)(SCENE_WIDTH - SCENE_BORDER_SIZE)) {
        player.entity.X = player.entity.X < (int16_t)SCENE_BORDER_SIZE ? SCENE_BORDER_SIZE : SCENE_WIDTH - SCENE_BORDER_SIZE;
        player.upscaledX = player.entity.X << SCREEN_SCALE;
        player.speedX = 0;
        if (!player.turning) {
            player.turning = TRUE;
            player.turnFrame = 0;
        }
    }
    if (player.entity.Y < (int16_t)SCENE_BORDER_SIZE || player.entity.Y > (int16_t)(SCENE_HEIGHT - SCENE_BORDER_SIZE)) {
        player.entity.Y = player.entity.Y < (int16_t)SCENE_BORDER_SIZE ? SCENE_BORDER_SIZE : SCENE_HEIGHT - SCENE_BORDER_SIZE;
        player.upscaledY = player.entity.Y << SCREEN_SCALE;
        player.speedY = 0;
    }

    /* Clamp before projecting sprites: every object uses the same camera. */
    if (player.entity.X <= (int16_t)START_SCROLL_X) scene.camera_x = 0;
    else if (player.entity.X >= (int16_t)(SCENE_WIDTH - START_SCROLL_X)) scene.camera_x = CAMERA_MAX_X;
    else scene.camera_x = player.entity.X - START_SCROLL_X;
    if (player.entity.Y <= (int16_t)START_SCROLL_Y) scene.camera_y = 0;
    else if (player.entity.Y >= (int16_t)(SCENE_HEIGHT - START_SCROLL_Y)) scene.camera_y = CAMERA_MAX_Y;
    else scene.camera_y = player.entity.Y - START_SCROLL_Y;

    if ((keys & J_A) && !player.turning && !player.shootCooldown) {
        bullet = playerBullets;
        for (i = 0; i != MAX_SHOOT_NUM; ++i, ++bullet) {
            if (bullet->active) continue;
            bullet->X = player.entity.X;
            bullet->Y = player.entity.Y;
            bullet->speedX = player.direction > 0 ? SHOOT_SPEED : -SHOOT_SPEED;
            bullet->speedY = 0;
            bullet->active = TRUE;
            set_sprite_prop(PLAYER_BULLET_SPRITE + i, player.direction > 0 ? 0 : S_FLIPX);
            player.shootCooldown = SHOOT_DELAY;
            break;
        }
    }

    x = player.entity.X - (int16_t)scene.camera_x;
    y = player.entity.Y - scene.camera_y;
    if (player.direction < 0) props = S_FLIPX;
    if (player.turning) frame = 5u + (player.turnFrame >> 2u);
    else {
        tilt = player.tilt;
        if (tilt < 0) {
            tilt = -tilt;
            props |= S_FLIPY;
            y += 16;
        }
        frame = (uint8_t)tilt >> 2u;
    }

    if (props == 0) move_metasprite_ex(ship_meta[frame], 0, 0, PLAYER_SPRITE, (uint8_t)x, (uint8_t)y);
    else if (props == S_FLIPX) move_metasprite_flipx(ship_meta[frame], 0, 0, PLAYER_SPRITE, (uint8_t)x, (uint8_t)y);
    else if (props == S_FLIPY) move_metasprite_flipy(ship_meta[frame], 0, 0, PLAYER_SPRITE, (uint8_t)x, (uint8_t)y);
    else move_metasprite_flipxy(ship_meta[frame], 0, 0, PLAYER_SPRITE, (uint8_t)x, (uint8_t)y);

    if (player.turning && ++player.turnFrame == 24u) {
        player.turning = FALSE;
        player.tilt = 19;
        player.direction = -player.direction;
        player.speedX = player.direction > 0 ? SHIP_ACCEL : -SHIP_ACCEL;
    }
}

#ifndef PLAYER_H
#define PLAYER_H

#include "GraphLib.h"

#define SHOOT_DELAY         10u
#define MAX_SHIP_SPEED      32
#define SHIP_ACCEL          2
#define SHOOT_SPEED         4

#if MAX_SHIP_SPEED > (8 << SCREEN_SCALE)
#error Scrolling streams at most one tile per axis per frame.
#endif

typedef struct {
    Entity_t entity;
    int16_t upscaledX, upscaledY;
    int8_t speedX, speedY;
    int8_t direction, tilt;
    uint8_t turning, turnFrame, shootCooldown;
} Player_t;

extern Player_t player;

void initPlayer(void);
void playerMove(void);

#endif

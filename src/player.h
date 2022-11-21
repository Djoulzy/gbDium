#ifndef player_H
#define player_H

#include "GraphLib.h"

#define MAX_SHOOT_NUM       3
#define SHOOT_DELAY         10
#define MAX_SHIP_SPEED      32
#define SHIP_ACCEL          2
#define SHOOT_SPEED         4
#define SHIP_RETURN_FRAME   5

extern EntityList_t* player;

extern void initPlayer(void);
extern void playerMove(void);

#endif
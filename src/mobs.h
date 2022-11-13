#ifndef mobs_H
#define mobs_H

#include "GraphLib.h"

#define NB_ALIENS           5
#define ALIEN_SPEED         1
#define ALIEN_SHOOT_SPEED   2
#define DELAY_COUNT         2

extern const metasprite_t* const alienFrames[];

extern EntityList_t* alienList;

extern void initAliens(void);
extern void alienMoves(Scene_t*, Coord_t*);

#endif
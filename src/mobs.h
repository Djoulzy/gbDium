#ifndef mobs_H
#define mobs_H

#include "GraphLib.h"

extern const metasprite_t* const alienFrames[];

extern void initAliens(void);
extern void alienMoves(Scene_t*, Coord_t*);

#endif
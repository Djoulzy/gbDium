#ifndef MOBS_H
#define MOBS_H

#include "GraphLib.h"
#include <gbdk/metasprites.h>

#define ALIEN_SPEED 2
#define ALIEN_MAX_SPEED 16
#define ALIEN_SHOOT_SPEED 1
#define ALIEN_MOVE_DELAY 4u

extern Entity_t aliens[NB_ALIENS];
extern const metasprite_t * const alienFrames[4];

void initAliens(void);
void alienMoves(void);

#endif

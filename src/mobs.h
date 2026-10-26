#ifndef MOBS_H
#define MOBS_H

#include "GraphLib.h"

#define ALIEN_SPEED         1
#define ALIEN_SHOOT_SPEED   2

extern Entity_t aliens[NB_ALIENS];

void initAliens(void);
void alienMoves(void);

#endif

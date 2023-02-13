#ifndef boss_H
#define boss_H

#include "GraphLib.h"

#define ALIEN_SHOOT_SPEED   1
#define NB_SHIPGUN      3

extern const metasprite_t* const shipgunFrames[];

extern EntityList_t* shipgunList;

extern void initBoss(void);
extern void bossMove(Scene_t*, Coord_t*);

#endif
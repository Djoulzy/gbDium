#ifndef BOSS_H
#define BOSS_H

#include "GraphLib.h"

#define BOSS_ENTRY_X 168
#define BOSS_MIN_X 80
#define BOSS_MAX_X 96
#define BOSS_MAX_Y 44
#define BOSS_MOVE_DELAY 3u

typedef struct {
    int16_t X, Y;
    int8_t speedX, speedY;
    uint8_t timer, moved;
} Boss_t;

extern Boss_t boss;
extern Entity_t shipguns[NB_SHIPGUN];

void initBoss(void);
void bossMove(void);

#endif

#ifndef GRAPHLIB_H
#define GRAPHLIB_H

#include <gb/gb.h>
#include <stdint.h>
#include "settings.h"

/* World coordinates retain the original metasprite pivot: (-8, 0).
 * Keep them signed and 16-bit until AFTER clipping to the viewport. */
typedef struct {
    int16_t X, Y;
    uint8_t active;
    uint8_t overlapped;
    uint8_t animStep;
} Entity_t;

typedef struct {
    int16_t X, Y;
    int8_t speedX, speedY;
    uint8_t active;
} Bullet_t;

typedef struct {
    uint16_t camera_x;
    uint8_t camera_y;
    uint8_t map_pos_x, map_pos_y;
} Scene_t;

extern Scene_t scene;
extern Bullet_t playerBullets[MAX_SHOOT_NUM];
extern Bullet_t alienBullets[NB_ALIENS];

/* SDCC's plain inline avoids duplicate debug symbols across translation units.
 * Native C11 tests need static inline instead. Both expand these hot helpers. */
#ifdef __SDCC
#define GAME_INLINE inline
#else
#define GAME_INLINE static inline
#endif

GAME_INLINE uint8_t isVisible(int16_t x, int16_t y) {
    return (uint16_t)(x - 1) < VIEWPORT_WIDTH + 16u &&
           (uint16_t)(y - 1) < VIEWPORT_HEIGHT + 16u;
}

GAME_INLINE void hideEntity(uint8_t first) {
    volatile OAM_item_t *oam = &shadow_OAM[first];
    oam->y = 0; ++oam;
    oam->y = 0; ++oam;
    oam->y = 0; ++oam;
    oam->y = 0;
}

#undef GAME_INLINE

void setupScene(void);
void moveBullets(void);
void updateView(void);

#endif

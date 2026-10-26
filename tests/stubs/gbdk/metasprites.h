#ifndef TEST_METASPRITES_H
#define TEST_METASPRITES_H
#include <gb/gb.h>

typedef struct metasprite_t { int8_t dy, dx; uint8_t tile, props; } metasprite_t;
#define METASPR_ITEM(dy, dx, tile, props) {dy, dx, tile, props}
#define METASPR_TERM {-128, 0, 0, 0}
uint8_t test_metasprite(const metasprite_t *frame, uint8_t first, uint8_t x, uint8_t y, uint8_t props);
#define move_metasprite_ex(frame, tile, prop, first, x, y) test_metasprite(frame, first, x, y, 0)
#define move_metasprite_flipx(frame, tile, prop, first, x, y) test_metasprite(frame, first, x, y, S_FLIPX)
#define move_metasprite_flipy(frame, tile, prop, first, x, y) test_metasprite(frame, first, x, y, S_FLIPY)
#define move_metasprite_flipxy(frame, tile, prop, first, x, y) test_metasprite(frame, first, x, y, S_FLIPX | S_FLIPY)
#endif

#ifndef TEST_PLATFORM_H
#define TEST_PLATFORM_H
#include <gb/gb.h>

typedef uint16_t palette_color_t;
#define RGB8(r, g, b) (((r) >> 3) | (((g) >> 3) << 5) | (((b) >> 3) << 10))
#define BANKREF(name)
#endif

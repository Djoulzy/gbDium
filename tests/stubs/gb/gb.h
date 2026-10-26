#ifndef TEST_GB_H
#define TEST_GB_H
#include <stdint.h>

#define TRUE 1u
#define FALSE 0u
#define J_RIGHT 0x01u
#define J_LEFT 0x02u
#define J_UP 0x04u
#define J_DOWN 0x08u
#define J_A 0x10u
#define J_SELECT 0x40u
#define S_FLIPX 0x20u
#define S_FLIPY 0x40u
#define BANKREF_EXTERN(name)

typedef struct OAM_item_t { uint8_t y, x, tile, prop; } OAM_item_t;
extern volatile OAM_item_t shadow_OAM[40];
extern uint8_t SCX_REG, SCY_REG, DIV_REG;
uint8_t joypad(void);
void vsync(void);
void set_sprite_data(uint8_t first, uint8_t count, const uint8_t *data);
void set_bkg_submap(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *map, uint8_t map_w);
void set_bkg_tiles(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *data);
static inline void set_sprite_tile(uint8_t i, uint8_t tile) { shadow_OAM[i].tile = tile; }
static inline void set_sprite_prop(uint8_t i, uint8_t prop) { shadow_OAM[i].prop = prop; }
static inline void move_sprite(uint8_t i, uint8_t x, uint8_t y) { shadow_OAM[i].x = x; shadow_OAM[i].y = y; }
static inline void hide_sprite(uint8_t i) { shadow_OAM[i].y = 0; }
#endif

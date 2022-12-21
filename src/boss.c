#include <gb/gb.h>
#include <gbdk/platform.h>
#include <stdlib.h>
#include <rand.h>
#include "GraphLib.h"

#include "../res/bigShip.h"

extern joypads_t joypads;

void initBoss() {
    set_bkg_data(0, bigShip_TILE_COUNT, bigShip_tiles);
    set_bkg_tiles(21, 0, bigShip_WIDTH >> 3, bigShip_HEIGHT >> 3, bigShip_map);
}

void bossMove() {
    
}
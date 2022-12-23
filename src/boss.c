#include <gb/gb.h>
#include <gbdk/platform.h>
#include <stdlib.h>
#include <rand.h>
#include "GraphLib.h"

#include "../res/bigShip.h"

extern joypads_t joypads;

uint8_t bossX, bossY;
int8_t bossSpeedX, bossSpeedY;
int8_t tempo;

void initBoss() {
    set_bkg_data(0, bigShip_TILE_COUNT, bigShip_tiles);
    set_bkg_tiles(21, 0, bigShip_WIDTH >> 3, bigShip_HEIGHT >> 3, bigShip_map);
    bossX = 21 << 3;
    bossY = 0;
    bossSpeedX = -1;
    bossSpeedY = 1;
    tempo = 3;
}

void bossMove() {
    tempo--;

    if (tempo <= 0) { 
        bossX += bossSpeedX;
        bossY += bossSpeedY;
        scroll_bkg(-bossSpeedX, -bossSpeedY);

        if (bossX < 80) bossSpeedX = 1;
        else if (bossX > 96) bossSpeedX = -1;

        if (bossY < 1) bossSpeedY = 1;
        else if (bossY > 44) bossSpeedY = -1;
        tempo = 3;
    }
}
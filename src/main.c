#include "GraphLib.h"
#include "game.h"
#include "../res/title.h"

static void titleScreen(void) {
    uint8_t keys;
    DISPLAY_OFF;
    HIDE_SPRITES;
    HIDE_WIN;
    SCX_REG = 0;
    SCY_REG = 0;
    BGP_REG = DMG_PALETTE(DMG_WHITE, DMG_LITE_GRAY, DMG_DARK_GRAY, DMG_BLACK);
    set_bkg_data(0, title_TILE_COUNT, title_tiles);
    set_bkg_tiles(0, 0, title_WIDTH >> 3u, title_HEIGHT >> 3u, title_map);
    SHOW_BKG;
    DISPLAY_ON;

    /* Keep the CPU idle between input samples while the title is displayed. */
    do {
        vsync();
        keys = joypad();
    } while (!(keys & J_START));
}

void main(void) {
    titleScreen();
    startGame();
    while (1) updateGame();
}

#include "GraphLib.h"
#include "player.h"
#include "mobs.h"
#include "../res/scene_tiles.h"
#include "../res/title.h"

static void titleScreen(void) {
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
    while (!(joypad() & J_START)) vsync();
}

static void init_gfx(void) {
    DISPLAY_OFF;
    SPRITES_8x8;
    set_bkg_data(0, 10, scene_tiles);
    setupScene();
    initPlayer();
    initAliens();
    SHOW_BKG;
    SHOW_SPRITES;
    DISPLAY_ON;
}

void main(void) {
    titleScreen();
    init_gfx();
    while (1) {
        playerMove();
        alienMoves();
        moveBullets();
        updateView();
    }
}

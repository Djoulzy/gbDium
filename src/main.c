#include "GraphLib.h"
#include "player.h"
#include "mobs.h"
#include "boss.h"
#include "../res/scene_tiles.h"
#include "../res/title.h"

static uint8_t titleScreen(void) {
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
    return keys & J_SELECT ? GAME_FREE : GAME_BOSS;
}

static void init_gfx(void) {
    DISPLAY_OFF;
    SPRITES_8x16;
    if (gameMode == GAME_FREE) {
        set_bkg_data(0, 10, scene_tiles);
        setupScene();
    }
    initPlayer();
    if (gameMode == GAME_BOSS) initBoss();
    else initAliens();
    SHOW_BKG;
    SHOW_SPRITES;
    DISPLAY_ON;
}

void main(void) {
    gameMode = titleScreen();
    init_gfx();
    while (1) {
        playerMove();
        if (gameMode == GAME_BOSS) bossMove();
        else alienMoves();
        moveBullets();
        updateView();
    }
}

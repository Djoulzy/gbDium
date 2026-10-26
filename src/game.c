#include "game.h"
#include "GraphLib.h"
#include "player.h"
#include "mobs.h"
#include "boss.h"
#include "../res/scene_tiles.h"

static void startLevel(uint8_t level) {
    uint8_t i;

    /* The tile map and graphics change together while the LCD is off. */
    DISPLAY_OFF;
    HIDE_SPRITES;
    HIDE_WIN;
    SPRITES_8x16;
    for (i = 0; i != 40u; ++i) hide_sprite(i);
    for (i = 0; i != NB_ALIENS; ++i) {
        aliens[i].active = FALSE;
        alienBullets[i].active = FALSE;
    }
    for (i = 0; i != NB_SHIPGUN; ++i) {
        shipguns[i].active = FALSE;
        bossBullets[i].active = FALSE;
    }
    scene.camera_x = 0;
    scene.camera_y = 0;
    scene.map_pos_x = 0;
    scene.map_pos_y = 0;
    SCX_REG = SCY_REG = 0;
    gameMode = level;
    initPlayer();
    if (level == GAME_FREE) {
        set_bkg_data(0, 10, scene_tiles);
        setupScene();
        initAliens();
    } else initBoss();
    /* Clear hardware OAM too, before the first scanline of the new level. */
    refresh_OAM();
    SHOW_BKG;
    SHOW_SPRITES;
    DISPLAY_ON;
}

void startGame(void) {
    startLevel(GAME_FREE);
}

void updateGame(void) {
    uint8_t remaining = NB_ALIENS;

    playerMove();
    if (gameMode == GAME_BOSS) bossMove();
    else remaining = alienMoves();
    moveBullets();
    updateView();

    /* An exploding enemy remains active until its last animation frame. */
    if (gameMode == GAME_FREE && !remaining) startLevel(GAME_BOSS);
}

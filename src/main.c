#include <gb/gb.h>
#include <gb/drawing.h>
#include <stdint.h>
#include <gbdk/platform.h>
#include <gbdk/metasprites.h>
#include "GraphLib.h"
#include "player.h"
#include "mobs.h"

#include "../res/title.h"
#include "../res/boss.h"

#include "../res/scene_tiles.h"
#include "../res/scene1.h"

#define CGB_BKG_PAL_0 0u
#define CGB_ONE_PAL   1u
const palette_color_t cgb_pal_black[] = {RGB_BLACK, RGB_BLACK, RGB_BLACK, RGB_BLACK};

joypads_t joypads;
Scene_t scene;
extern unsigned char cEMPTY[]; 

void titleScreen(void)
{
	disable_interrupts();
	DISPLAY_OFF;
	HIDE_SPRITES;
	HIDE_WIN;
	HIDE_BKG;
	DISPLAY_ON;
	enable_interrupts();

    BGP_REG = DMG_PALETTE(DMG_WHITE, DMG_LITE_GRAY, DMG_DARK_GRAY, DMG_BLACK);
     // Load up the tile data
     set_bkg_data(0, title_TILE_COUNT, boss_tiles);
     // Set screen x,y pos to 0,0 and draw the map 20,18(size of the screen)
     set_bkg_tiles(0, 0, 20, 18, boss_map);
    SHOW_BKG;

    // Loop forever
    while(1) {
        joypad_ex(&joypads);
        if (joypads.joy0 & J_START) {
            // mode(M_TEXT_OUT);
            break;
        }
        // Main processing goes here
        // Done processing, yield CPU and wait for start of next frame
        wait_vbl_done();
    }
}

void init_gfx() {
	SPRITES_8x16;
	HIDE_SPRITES;
	HIDE_BKG;
	HIDE_WIN;
    set_bkg_data(0, 10, scene_tiles);
    setupScene(&scene, Scene1, Scene1Width, Scene1Height);

    initPlayer();
    initAliens();

	SHOW_BKG;
	// SHOW_WIN;
	SHOW_SPRITES;
	DISPLAY_ON;
}

void none() {
}

void main(void)
{
    titleScreen();
    // DISPLAY_OFF;

    // STAT_REG = 0x45;
    // LYC_REG = 0x00;

	// disable_interrupts();
    // add_LCD(none);
    // enable_interrupts();
    // set_interrupts(VBL_IFLAG | LCD_IFLAG);

    // LCDC_REG = LCDCF_BG9800 | LCDCF_WIN9C00;
	init_gfx();

    // gprintf("%x", get_mode());

    SCX_REG = 0; SCY_REG = 0;
    while(1) {
        playerMove();
        // alienMoves(&scene, &(player->entity->coord));
        updateView(&scene);
    }
}

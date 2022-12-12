#include <gb/gb.h>
#include <gb/drawing.h>
#include <stdint.h>
#include <gbdk/platform.h>
#include <gbdk/metasprites.h>
#include "GraphLib.h"
#include "player.h"
#include "mobs.h"

#include "../res/title.h"
#include "../res/scene_tiles.h"
#include "../res/scene1.h"

#define CGB_BKG_PAL_0 0u
#define CGB_ONE_PAL   1u
const palette_color_t cgb_pal_black[] = {RGB_BLACK, RGB_BLACK, RGB_BLACK, RGB_BLACK};

joypads_t joypads;
Scene_t scene;

void titleScreen(void)
{
    // Set the screen to black via the palettes to hide the image draw
    // if (_cpu == CGB_TYPE) {
    //     set_bkg_palette(BKGF_CGB_PAL0, CGB_ONE_PAL, cgb_pal_black);
    // } else {
    //     BGP_REG = DMG_PALETTE(DMG_BLACK, DMG_BLACK, DMG_BLACK, DMG_BLACK);
    // }

    // Display the image
    // This will automatically switch to APA graphics mode
    // and install it's start and mid-frame ISRs.
    draw_image(title_tiles);
    SHOW_BKG;

    // Then load the palettes at the start of a new frame
    // wait_vbl_done();
    // if (_cpu == CGB_TYPE) {
    //     set_bkg_palette(BKGF_CGB_PAL0, CGB_ONE_PAL, title_palettes);
    // } else {
    //     BGP_REG = DMG_PALETTE(DMG_WHITE, DMG_LITE_GRAY, DMG_DARK_GRAY, DMG_BLACK);
    // }


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
    DISPLAY_OFF;
    VBK_REG = 1;
    VBK_REG = 0;
    set_bkg_data(0, 10, scene_tiles);
    setupScene(&scene, Scene1, Scene1Width, Scene1Height);

    initPlayer();
    initAliens();

    SHOW_BKG;
    SHOW_SPRITES;
    DISPLAY_ON;
}

void main(void)
{
    titleScreen();
    // wait_vbl_done();
    // mode(M_TEXT_OUT);
    // By default,
    // the background and window TDT is located at addresses 0x8800-0x97FF,
    // the sprite TDT at addresses 0x8000-0x8FFF,
    // the BTM at addresses 0x9800-0x9BFF,
    // and the WTM at addresses 0x9C00-0x9FFF
// add_LCD(LCDCF_BG8800);
// add_LCD(LCDCF_BG9800);

	init_gfx();
    // gprintf("%x", get_mode());

    SCX_REG = 0; SCY_REG = 0;
    while(1) {
        playerMove();
        // alienMoves(&scene, &(player->entity->coord));
        updateView(&scene);
    }
}

#include <gb/gb.h>
#include <gbdk/emu_debug.h>
#include <stdint.h>
#include <stdlib.h>
#include <gbdk/platform.h>
#include <gbdk/metasprites.h>
#include "mobs.h"
#include "../res/setSprites.h"
#include "../res/backgrounds.h"
#include "../res/scene1.h"

#define SHIP_SPRITE     0
#define SHOOT_SPRITE    4
#define MAX_SHOOT_NUM   6
#define SHOOT_DELAY     10
#define MAX_SHIP_SPEED  32
#define SHIP_ACCEL      2
#define SCREEN_MULTI    4

#define MIN(A,B) ((A)<(B)?(A):(B))

joypads_t joypads;

int16_t camera_max_x, camera_max_y;
// current and old positions of the camera in pixels
int16_t camera_x, camera_y;
uint16_t old_camera_x, old_camera_y;
// current and old position of the map in tiles
uint8_t map_pos_x, map_pos_y, old_map_pos_x, old_map_pos_y;
// redraw flag, indicates that camera position was changed
uint8_t redraw;

uint16_t startScrollZoneX, startScrollZoneY, endScrollZoneX, endScrollZoneY;
uint16_t screenWidth, screenHeight;

typedef struct ShipShoot ShipShoot_t;
struct ShipShoot {
    uint8_t active;
    int8_t dir;
    int16_t x,y;
};

void init_gfx() {
    // Screen : 160x144
    SPRITES_8x8;
    DISPLAY_OFF;

    // Load Background tiles and then map
    set_bkg_data(0, 10, backgrounds);
    // set_bkg_tiles(0, 0, 32, 32, Scene1);
    set_sprite_data(0, 45, Ship);

    camera_max_y = ((Scene1Height - 18) * 8) << SCREEN_MULTI;
    camera_max_x = ((Scene1Width - 20) * 8) << SCREEN_MULTI;

    map_pos_x = map_pos_y = 0; 
    old_map_pos_x = old_map_pos_y = 255;
    set_bkg_submap(map_pos_x, map_pos_y, 20, 18, Scene1, Scene1Width);

    old_camera_x = camera_x = 0; old_camera_y = camera_y = 0;

    screenWidth = (Scene1Width * 8) << SCREEN_MULTI;
    screenHeight = (Scene1Height * 8) << SCREEN_MULTI;
    startScrollZoneX = 80 << SCREEN_MULTI;
    startScrollZoneY = 72 << SCREEN_MULTI;
    endScrollZoneX = ((Scene1Width * 8) - 80) << SCREEN_MULTI;
    endScrollZoneY = ((Scene1Height * 8) - 72) << SCREEN_MULTI;

    initAliens();
	// Turn the background map on to make it visible
    SHOW_BKG;
    SHOW_SPRITES;
    DISPLAY_ON;
}

void set_camera(const uint8_t * scene, uint16_t sceneWidth, uint16_t sceneHeight) {
    uint16_t tmpX, tmpY;

    tmpX = camera_x >> SCREEN_MULTI;
    tmpY = camera_y >> SCREEN_MULTI;

    // update hardware scroll position
    SCY_REG = tmpY; SCX_REG = tmpX; 
    // up or down
    map_pos_y = (uint8_t)(tmpY >> 3u);
    if (map_pos_y != old_map_pos_y) { 
        if (tmpY < old_camera_y) {
            set_bkg_submap(map_pos_x, map_pos_y, MIN(21u, sceneWidth-map_pos_x), 1, scene, sceneWidth);
        } else {
            if ((sceneHeight - 18u) > map_pos_y) set_bkg_submap(map_pos_x, map_pos_y + 18u, MIN(21u, sceneWidth-map_pos_x), 1, scene, sceneWidth);     
        }
        old_map_pos_y = map_pos_y; 
    }
    // left or right
    map_pos_x = (uint8_t)(tmpX >> 3u);
    if (map_pos_x != old_map_pos_x) {
        if (tmpX < old_camera_x) {
            set_bkg_submap(map_pos_x, map_pos_y, 1, MIN(19u, sceneHeight - map_pos_y), scene, sceneWidth);     
        } else {
            if ((sceneWidth - 20u) > map_pos_x) set_bkg_submap(map_pos_x + 20u, map_pos_y, 1, MIN(19u, sceneHeight - map_pos_y), scene, sceneWidth);     
        }
        old_map_pos_x = map_pos_x;
    }
    // set old camera position to current camera position
    old_camera_x = tmpX, old_camera_y = tmpY;
}

void main(void)
{
    int16_t ShipX, ShipY, tmpX, tmpY, ShipAbsX, ShipAbsY;
    int16_t SpdX, SpdY;
    uint16_t retourn_anim;
    int8_t direction, inclinaison;
    uint8_t i, hiwater, next_shoot = 0, shoot_delay = 0;
    uint8_t retournement = FALSE;
    ShipShoot_t ship_shoot[MAX_SHOOT_NUM];

	init_gfx();

    joypad_init(1, &joypads);


    ShipX = ShipAbsX = startScrollZoneX;
    ShipY = ShipAbsY = startScrollZoneY;
    SpdX = SpdY = inclinaison = retourn_anim = 0;
    direction = 1;

    for (i = 0; i<MAX_SHOOT_NUM; i++) {
        set_sprite_tile(SHOOT_SPRITE+i, 44);
        ship_shoot[i].active = 0;
    }

    SCX_REG = 0; SCY_REG = 0;
    // Loop forever
    while(1) {
		// Game main loop processing goes here

        joypad_ex(&joypads);
        if (joypads.joy0 & J_SELECT) {
            SpdX = SpdY = 0;
        }

        if (joypads.joy0 & J_UP) {
            SpdY -= SHIP_ACCEL;
            if (SpdY < - MAX_SHIP_SPEED) SpdY = - MAX_SHIP_SPEED;
            inclinaison++;
            if (inclinaison > 16) inclinaison = 16;
        } else if (joypads.joy0 & J_DOWN) {
            SpdY += SHIP_ACCEL;
            if (SpdY > MAX_SHIP_SPEED) SpdY = MAX_SHIP_SPEED;
            inclinaison--;
            if (inclinaison < -16) inclinaison = -16;
        } else if (inclinaison != 0) {
            if (inclinaison > 0) inclinaison--;
            else inclinaison++;
        }

        if ((joypads.joy0 & J_LEFT) && (!retournement)) {
            if (direction == 1) {
                // EMU_printf("Go LEFT");
                retournement = TRUE;
                retourn_anim = 0;
                SpdX += SHIP_ACCEL;
            } else {
                SpdX -= SHIP_ACCEL;
                if (SpdX < - MAX_SHIP_SPEED) SpdX = - MAX_SHIP_SPEED;
            }
        } else if ((joypads.joy0 & J_RIGHT)&& (!retournement)) {
            if (direction == -1) {
                // EMU_printf("Go RIGHT");
                retournement = TRUE;
                retourn_anim = 0;
                SpdX -= SHIP_ACCEL;
            } else {
                SpdX += SHIP_ACCEL;
                if (SpdX > MAX_SHIP_SPEED) SpdX = MAX_SHIP_SPEED;
            }
        }

        ShipAbsX += SpdX; ShipAbsY += SpdY;

        if ((ShipAbsX >= startScrollZoneX) && (ShipAbsX <= endScrollZoneX)) {
            camera_x += SpdX;
            if (camera_x > camera_max_x) camera_x = camera_max_x;
            if (camera_x < 0) camera_x = 0;
            redraw = TRUE;
        } else ShipX += SpdX;
        
        if ((ShipAbsY >= startScrollZoneY) && (ShipAbsY <= endScrollZoneY)) {
            camera_y += SpdY;
            // EMU_printf("CamX: %d (%d) - CamY: %d (%d) - AbsX: %d - AbsY: %d - ShpX: %d - ShpY: %d", camera_x, camera_max_x, camera_y, camera_max_y, ShipAbsX, ShipAbsY, tmpX, tmpY);
            if (camera_y > camera_max_y) camera_y = camera_max_y;
            if (camera_y < 0) camera_y = 0;
            redraw = TRUE;
        } else ShipY += SpdY;
        
        tmpX = (ShipX >> SCREEN_MULTI) + 8;
        tmpY = (ShipY >> SCREEN_MULTI) + 8;

        if (joypads.joy0 & J_A) {
            if ((next_shoot < MAX_SHOOT_NUM) && (!shoot_delay) && (!retournement)) {
                EMU_printf("SHOOT !");
                ship_shoot[next_shoot].dir = direction;
                ship_shoot[next_shoot].active = 1;
                ship_shoot[next_shoot].x = tmpX;
                ship_shoot[next_shoot].y = tmpY + 4;
                if (direction < 0) set_sprite_prop(SHOOT_SPRITE+next_shoot, S_FLIPX);
                shoot_delay = SHOOT_DELAY;
            }
        }

        if (retournement) {
            if (direction >= 0)
                hiwater = move_metasprite(ship_retournement[retourn_anim >> 2], 0, SHIP_SPRITE, tmpX, tmpY);
            else
                hiwater = move_metasprite_vflip(ship_retournement[retourn_anim >> 2], 0, SHIP_SPRITE, tmpX, tmpY);
            retourn_anim++;
            if (retourn_anim > 20) {
                retournement = FALSE;
                inclinaison = 16;
                direction *= -1;
                SpdX = SHIP_ACCEL * direction;
            }
        } else {
            // if (!redraw) {
                if (tmpX >= 160)  {
                    retournement = TRUE;
                    retourn_anim = 0;
                    SpdX = 0;
                    ShipX = 152 << SCREEN_MULTI;
                    ShipAbsX = screenWidth;
                }
                if (tmpX < 16)  {
                    retournement = TRUE;
                    retourn_anim = 0;
                    SpdX = 0;
                    ShipAbsX = ShipX = 8 << SCREEN_MULTI;
                }
                if (tmpY < 16) {
                    SpdY = 0;
                    ShipAbsY = ShipY = 8 << SCREEN_MULTI;
                }
                if (tmpY >= 144) {
                    SpdY = 0;
                    ShipY = 136 << SCREEN_MULTI;
                    ShipAbsY = screenHeight;
                }
            // }

            if (direction >= 0) {
                if (inclinaison >= 0)
                    hiwater = move_metasprite(ship_meta[abs(inclinaison >> 2)], 0, SHIP_SPRITE, tmpX, tmpY);
                else {
                    hiwater = move_metasprite_hflip(ship_meta[abs(inclinaison >> 2)], 0, SHIP_SPRITE, tmpX, tmpY+16);
                }
            }
            else {
                if (inclinaison >= 0)
                    hiwater = move_metasprite_vflip(ship_meta[abs(inclinaison >> 2)], 0, SHIP_SPRITE, tmpX, tmpY);
                else hiwater = move_metasprite_hvflip(ship_meta[abs(inclinaison >> 2)], 0, SHIP_SPRITE, tmpX, tmpY+16);
            }
        }
        // hide_sprites_range(hiwater, 40);

        next_shoot = 100;
        for (i = 0; i<MAX_SHOOT_NUM; i++) {
            if (ship_shoot[i].active) {
                if (ship_shoot[i].dir >= 0) ship_shoot[i].x += 4;
                else ship_shoot[i].x -= 4;
                move_sprite(SHOOT_SPRITE+i, ship_shoot[i].x, ship_shoot[i].y);
                if ((ship_shoot[i].x < -9) || (ship_shoot[i].x > 168)) {
                    ship_shoot[i].active = 0;
                    next_shoot = i;
                }
            } else next_shoot = i;
        }

		// Done processing, yield CPU and wait for start of next frame
        if (shoot_delay) shoot_delay--;

        alienMoves(camera_x, camera_y);

        if (redraw) {        
            wait_vbl_done();
            set_camera(Scene1, Scene1Width, Scene1Height);
            redraw = FALSE;
        } else wait_vbl_done();
    }
}

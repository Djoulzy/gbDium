#ifndef GraphLib_H
#define GraphLib_H

#include <gbdk/metasprites.h>

#define MIN(A,B) ((A)<(B)?(A):(B))

#define LEFT_BORDER_LIMIT       0
#define RIGHT_BORDER_LIMIT      160
#define UP_BORDER_LIMIT         0
#define DOWN_BORDER_LIMIT       144

#define OUT_LEFT    2
#define OUT_RIGHT   4
#define OUT_UP      8
#define OUT_DOWN    16

typedef struct {
    uint8_t active;
    uint8_t spriteNum;
    int8_t spdx, spdy;
    int16_t dirx, diry;
    int16_t x,y;
} Shoot_t;

typedef struct {
    uint8_t active;
    int8_t spdx, spdy;
    int16_t x,y;
    uint8_t animStep;
    Shoot_t shoot;
} Aliens_t;

typedef struct {
    uint8_t active;
    uint8_t spriteNum;
    uint8_t cameraStick;
    int8_t speedX, speedY;
    int16_t sceneX, sceneY;
    int16_t visibleX, visibleY;
    uint8_t animStep;
    const metasprite_t** frames;
    Shoot_t** shoots;
} Entity_t;

typedef struct {
    const uint8_t* sceneData;                   // Map data
    uint8_t sceneW, sceneH;                     // map size in tiles
    int16_t camera_max_x, camera_max_y;
    int16_t camera_x, camera_y;
    uint16_t old_camera_x, old_camera_y;
    uint8_t map_pos_x, map_pos_y;
    uint8_t old_map_pos_x, old_map_pos_y;
    uint8_t redraw;
    uint16_t startScrollZoneX, startScrollZoneY;
    uint16_t endScrollZoneX, endScrollZoneY;
    uint16_t screenWidth, screenHeight;
} Scene_t;

extern void setupShoot(Shoot_t*, uint8_t);
extern void setupEntity(Entity_t*, const metasprite_t**, int16_t, int16_t);
extern void assignShootToEntity(Entity_t*, Shoot_t**);
extern void setupScene(Scene_t*, const uint8_t*, uint8_t, uint8_t);
extern void setCamera(Scene_t*);
extern void updateView(Scene_t*);
extern void setCameraStick(Entity_t*);
extern void updateEntityPos(Scene_t*, Entity_t*);
extern int8_t isOutOfScene(Scene_t*, Entity_t*);
extern uint8_t isVisible(Entity_t*);


#endif
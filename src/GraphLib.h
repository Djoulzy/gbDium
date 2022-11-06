#ifndef GraphLib_H
#define GraphLib_H

#include <gbdk/metasprites.h>

#define SCREEN_MULTI        4
#define MIN(A,B) ((A)<(B)?(A):(B))

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
    Shoot_t* shoot;
    const metasprite_t** frames;
} Entity_t;

typedef struct {
    const uint8_t* sceneData;
    uint8_t sceneW, sceneH;
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

extern Shoot_t* newShoot(uint8_t);
extern Entity_t* newEntity(const metasprite_t**);
extern Scene_t* newScene(const uint8_t*, uint8_t, uint8_t);
extern void setCamera(Scene_t*);
extern void updateEntityPos(Scene_t*, Entity_t*);
extern uint8_t isVisible(Entity_t*);

#endif
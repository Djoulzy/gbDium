#ifndef GraphLib_H
#define GraphLib_H

#include <gbdk/metasprites.h>
#include "settings.h"

#define MIN(A,B) ((A)<(B)?(A):(B))

#define OUT_LEFT    2
#define OUT_RIGHT   4
#define OUT_UP      8
#define OUT_DOWN    16

typedef struct {
    uint8_t cameraStick;
    int16_t upscaledX, upscaledY;
    int16_t X, Y;
    uint16_t viewportX, viewportY;
} Coord_t;

typedef struct {
    uint8_t active;
    uint8_t spriteNum;
    int8_t speedX, speedY;
    Coord_t coord;
    uint8_t props;
} Bullet_t;

typedef struct {
    uint8_t active;
    uint8_t spriteNum;
    int8_t speedX, speedY;
    Coord_t coord;
    uint8_t animStep;
    const metasprite_t** frames;
    uint8_t nb_shoots;
    uint8_t availableShoot;
    uint8_t shootDelay;
    uint8_t shootDelayCpt;
    Bullet_t** bullets;
} Entity_t;

typedef struct {
    const uint8_t* sceneData;                   // Map data
    uint8_t nb_tiles_w, nb_tiles_h;             // map size in tiles
    uint16_t camera_max_x, camera_max_y;
    uint16_t camera_x, camera_y;
    uint16_t old_camera_x, old_camera_y;
    uint8_t map_pos_x, map_pos_y;
    uint8_t old_map_pos_x, old_map_pos_y;
    uint8_t redraw;
    uint16_t startScrollZoneX, startScrollZoneY;
    uint16_t endScrollZoneX, endScrollZoneY;
    uint16_t sceneWidth, sceneHeight;
} Scene_t;

extern Bullet_t* allocBullet(uint8_t);
extern void     setupEntity(Entity_t*, const metasprite_t**, int16_t, int16_t);
extern void     assignBulletsToEntity(Entity_t*, Bullet_t**, uint8_t, uint8_t);
extern void     setupScene(Scene_t*, const uint8_t*, uint8_t, uint8_t);
extern void     setCamera(Scene_t*);
extern void     updateView(Scene_t*);
extern void     setCameraStick(Entity_t*);
extern void     updatePlayerPos(Scene_t*, Entity_t*);
extern void     updateMobPos(Scene_t*, Entity_t*);
extern void     moveEntityBullets(Scene_t*, Entity_t*);
extern void     entityShoot(Entity_t*, int8_t, int8_t, uint8_t);
extern int8_t   isOutOfScene(Scene_t*, Coord_t*);
extern uint8_t  isVisible(Coord_t*);
extern void     destroyEntity(Entity_t*);
extern void     dumpEntity(Entity_t*);

#endif
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
    int8_t direction;
    uint8_t overlapped;
    uint16_t viewportX, viewportY;
} Coord_t;

typedef struct {
    uint8_t active;
    uint8_t spriteNum;
    int8_t speedX, speedY;
    Coord_t coord;
    uint8_t props;
} Bullet_t;

typedef struct BulletList_t BulletList_t;
struct BulletList_t {
    Bullet_t* entity;
    BulletList_t* suiv;
};

typedef struct {
    uint8_t active;
    uint8_t spriteNum;
    int8_t speedX, speedY;
    Coord_t coord;
    const uint8_t* anim;
    int8_t animStep;
    const metasprite_t** frames;
    uint8_t shootDelay;
    uint8_t shootDelayCpt;
    BulletList_t* bullets;
    BulletList_t* availableBullet;
} Entity_t;

typedef struct EntityList_t EntityList_t;
struct EntityList_t {
    Entity_t* entity;
    EntityList_t* suiv;
    EntityList_t* prec;
};

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

extern EntityList_t*    addEntityToList(EntityList_t*, const metasprite_t**, int16_t, int16_t);
extern void             prepareBulletList(Entity_t*, uint8_t, uint8_t);
extern void             setupScene(Scene_t*, const uint8_t*, uint8_t, uint8_t);
extern void             setCamera(Scene_t*);
extern void             updateView(Scene_t*);
extern void             setCameraStick(Entity_t*);
extern void             updatePlayerPos(Scene_t*, Entity_t*, uint8_t);
extern void             updateMobPos(Scene_t*, Entity_t*);
extern void             moveEntityBullets(Scene_t*, Entity_t*, EntityList_t*);
extern uint8_t          shootOk(Entity_t*);
extern void             entityShoot(Entity_t*, int8_t, int8_t, uint8_t);
extern int8_t           isOutOfScene(Scene_t*, Coord_t*);
extern uint8_t          isVisible(Coord_t*);
extern void             destroyEntity(Entity_t*);
extern void             dumpEntity(Entity_t*);

#endif
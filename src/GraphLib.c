#include <gb/gb.h>
#include "GraphLib.h"

uint8_t spritesCount = 10;

Shoot_t* newShoot(uint8_t tileNum) {
    Shoot_t tmp;

    tmp.spriteNum = spritesCount;
    spritesCount++;
    tmp.active = FALSE;
    set_sprite_tile(tmp.spriteNum, tileNum);

    return &tmp;
}

Entity_t* newEntity(const metasprite_t** frames) {
    Entity_t tmp;

    tmp.spriteNum = spritesCount;
    spritesCount += 4;
    tmp.active = FALSE;
    tmp.sceneX = 0;
    tmp.sceneY = 0;
    tmp.animStep = 0;
    tmp.frames = frames;
    tmp.shoot = NULL;

    return &tmp;
}

Scene_t* newScene(const uint8_t* sceneData, uint8_t sceneW, uint8_t sceneH) {
    Scene_t tmp;

    tmp.sceneW = sceneW;
    tmp.sceneH = sceneH;

    tmp.camera_max_y = ((sceneH - 18) * 8) << SCREEN_MULTI;
    tmp.camera_max_x = ((sceneW - 20) * 8) << SCREEN_MULTI;

    tmp.map_pos_x = tmp.map_pos_y = 0; 
    tmp.old_map_pos_x = tmp.old_map_pos_y = 255;
    set_bkg_submap(tmp.map_pos_x, tmp.map_pos_y, 20, 18, sceneData, sceneW);

    tmp.old_camera_x = tmp.camera_x = 0;
    tmp.old_camera_y = tmp.camera_y = 0;

    tmp.screenWidth = (sceneW * 8) << SCREEN_MULTI;
    tmp.screenHeight = (sceneH * 8) << SCREEN_MULTI;
    tmp.startScrollZoneX = 80 << SCREEN_MULTI;
    tmp.startScrollZoneY = 72 << SCREEN_MULTI;
    tmp.endScrollZoneX = ((sceneW * 8) - 80) << SCREEN_MULTI;
    tmp.endScrollZoneY = ((sceneH * 8) - 72) << SCREEN_MULTI;

    return &tmp;
}

void updateEntityPos(Scene_t* scene, Entity_t* entity) {
    entity->sceneX += entity->speedX;
    entity->sceneY += entity->speedY;

    if (entity->cameraStick) {
        entity->visibleX = (entity->sceneX - scene->camera_x) >> SCREEN_MULTI;
        entity->visibleY = (entity->sceneY - scene->camera_y) >> SCREEN_MULTI;
    } else {
        entity->visibleX = (entity->sceneX) >> SCREEN_MULTI;
        entity->visibleY = (entity->sceneY) >> SCREEN_MULTI;
    }
}

uint8_t isVisible(Entity_t* entity) {
    if ((entity->visibleX>=0) && (entity->visibleX<=176)
        && (entity->visibleY>=0) && (entity->visibleY<=160))
    {
        return TRUE;
    } else {
        hide_metasprite(entity->frames[0], entity->spriteNum);
        return FALSE;
    }
}

void setCamera(Scene_t* scene) {
    uint16_t tmpX, tmpY;

    tmpX = scene->camera_x >> SCREEN_MULTI;
    tmpY = scene->camera_y >> SCREEN_MULTI;

    // update hardware scroll position
    SCY_REG = tmpY; SCX_REG = tmpX; 
    // up or down
    scene->map_pos_y = (uint8_t)(tmpY >> 3u);
    if (scene->map_pos_y != scene->old_map_pos_y) { 
        if (tmpY < scene->old_camera_y) {
            set_bkg_submap(scene->map_pos_x, scene->map_pos_y, MIN(21u, scene->sceneW - scene->map_pos_x), 1, scene->sceneData, scene->sceneW);
        } else {
            if ((scene->sceneH - 18u) > scene->map_pos_y) set_bkg_submap(scene->map_pos_x, scene->map_pos_y + 18u, MIN(21u, scene->sceneW - scene->map_pos_x), 1, scene->sceneData, scene->sceneW);     
        }
        scene->old_map_pos_y = scene->map_pos_y; 
    }
    // left or right
    scene->map_pos_x = (uint8_t)(tmpX >> 3u);
    if (scene->map_pos_x != scene->old_map_pos_x) {
        if (tmpX < scene->old_camera_x) {
            set_bkg_submap(scene->map_pos_x, scene->map_pos_y, 1, MIN(19u, scene->sceneH - scene->map_pos_y), scene->sceneData, scene->sceneW);     
        } else {
            if ((scene->sceneW - 20u) > scene->map_pos_x) set_bkg_submap(scene->map_pos_x + 20u, scene->map_pos_y, 1, MIN(19u, scene->sceneH - scene->map_pos_y), scene->sceneData, scene->sceneW);     
        }
        scene->old_map_pos_x = scene->map_pos_x;
    }
    // set old camera position to current camera position
    scene->old_camera_x = tmpX, scene->old_camera_y = tmpY;
}
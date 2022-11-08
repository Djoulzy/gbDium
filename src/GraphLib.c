#include <gb/gb.h>
#include <stdlib.h>
#include <gbdk/emu_debug.h>
#include "settings.h"
#include "GraphLib.h"

uint8_t spritesCount = 0;

Bullet_t* allocBullet(uint8_t tileNum) {
    Bullet_t* bullet = malloc(sizeof(Bullet_t));
    bullet->spriteNum = spritesCount;
    // EMU_printf("new sprite: %d", shoot->spriteNum);
    spritesCount++;
    bullet->active = FALSE;
    bullet->speedX = bullet->speedY = 0;
    bullet->sceneX = bullet->sceneY = 0;
    set_sprite_tile(bullet->spriteNum, tileNum);
    return bullet;
}

void setupEntity(Entity_t* entity, const metasprite_t** frames, int16_t sceneX, int16_t sceneY) {
    entity->spriteNum = spritesCount;
    spritesCount += 4;
    entity->active = FALSE;
    entity->sceneX = sceneX;
    entity->sceneY = sceneY;
    entity->speedX = entity->speedY = 0;
    entity->animStep = 0;
    entity->frames = frames;
    entity->bullets = NULL;
}

void assignBulletsToEntity(Entity_t* entity, Bullet_t** bullets, uint8_t size) {
    entity->bullets = bullets;
    entity->nb_shoots = size;
}

void setupScene(Scene_t* tmp, const uint8_t* sceneData, uint8_t sceneW, uint8_t sceneH) {
    tmp->sceneData = sceneData;
    tmp->sceneW = sceneW;
    tmp->sceneH = sceneH;

    tmp->camera_max_x = ((sceneW - 20) * 8) << SCREEN_SCALE;
    tmp->camera_max_y = ((sceneH - 18) * 8) << SCREEN_SCALE;

    tmp->map_pos_x = tmp->map_pos_y = 0; 
    tmp->old_map_pos_x = tmp->old_map_pos_y = 255;
    set_bkg_submap(tmp->map_pos_x, tmp->map_pos_y, 20, 18, tmp->sceneData, tmp->sceneW);

    tmp->old_camera_x = tmp->camera_x = 0;
    tmp->old_camera_y = tmp->camera_y = 0;

    tmp->screenWidth = (sceneW * 8) << SCREEN_SCALE;
    tmp->screenHeight = (sceneH * 8) << SCREEN_SCALE;
    tmp->startScrollZoneX = 80 << SCREEN_SCALE;
    tmp->startScrollZoneY = 72 << SCREEN_SCALE;
    tmp->endScrollZoneX = ((sceneW * 8) - 80) << SCREEN_SCALE;
    tmp->endScrollZoneY = ((sceneH * 8) - 72) << SCREEN_SCALE;
}

void setCameraStick(Entity_t* entity) {
    entity->cameraStick = TRUE;
}

int8_t isOutOfScene(Scene_t* scene, Entity_t* entity) {
    int8_t out = 0;
    
    if (entity->visibleX > VIEWPORT_WIDTH - VIEWPORT_BORDER_SIZE)  {
        entity->speedX = 0;
        entity->visibleX = VIEWPORT_WIDTH - VIEWPORT_BORDER_SIZE;
        entity->sceneX = (int16_t)(scene->screenWidth) - (int16_t)(SCREEN_BORDER_SIZE);
        scene->camera_x = scene->camera_max_x;
        out |= OUT_RIGHT;
    }
    else if (entity->visibleX < VIEWPORT_BORDER_SIZE)  {
        entity->speedX = 0;
        entity->visibleX = VIEWPORT_BORDER_SIZE;
        entity->sceneX = SCREEN_BORDER_SIZE;
        scene->camera_x = 0;
        out |= OUT_LEFT;
    }

    if (entity->visibleY > VIEWPORT_HEIGHT - VIEWPORT_BORDER_SIZE) {
        entity->speedY = 0;
        entity->visibleY = VIEWPORT_HEIGHT - VIEWPORT_BORDER_SIZE;
        entity->sceneY = (int16_t)(scene->screenHeight) - (int16_t)(SCREEN_BORDER_SIZE);
        scene->camera_y = scene->camera_max_y;
        out |= OUT_DOWN;
    }
    else if (entity->visibleY < VIEWPORT_BORDER_SIZE) {
        entity->speedY = 0;
        entity->visibleY = VIEWPORT_BORDER_SIZE ;
        entity->sceneY = SCREEN_BORDER_SIZE;
        scene->camera_y = 0;
        out |= OUT_UP;
    }

    return out;
}

void updateEntityPos(Scene_t* scene, Entity_t* entity) {
    entity->sceneX += entity->speedX;
    entity->sceneY += entity->speedY;

    if (entity->cameraStick) {
        if ((entity->sceneX >= scene->startScrollZoneX) && (entity->sceneX <= scene->endScrollZoneX)) {
            scene->camera_x += entity->speedX;
            if (scene->camera_x > scene->camera_max_x) scene->camera_x = scene->camera_max_x;
            if (scene->camera_x < 0) scene->camera_x = 0;
            scene->redraw = TRUE;
        }
        
        if ((entity->sceneY >= scene->startScrollZoneY) && (entity->sceneY <= scene->endScrollZoneY)) {
            scene->camera_y += entity->speedY;
            // EMU_printf("CamX: %d (%d) - CamY: %d (%d) - AbsX: %d - AbsY: %d - ShpX: %d - ShpY: %d", camera_x, camera_max_x, camera_y, camera_max_y, ShipAbsX, ShipAbsY, tmpX, tmpY);
            if (scene->camera_y > scene->camera_max_y) scene->camera_y = scene->camera_max_y;
            if (scene->camera_y < 0) scene->camera_y = 0;
            scene->redraw = TRUE;
        }
    }

    entity->visibleX = ((entity->sceneX - scene->camera_x) >> SCREEN_SCALE);
    entity->visibleY = ((entity->sceneY - scene->camera_y) >> SCREEN_SCALE);
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

    tmpX = scene->camera_x >> SCREEN_SCALE;
    tmpY = scene->camera_y >> SCREEN_SCALE;

    // update hardware scroll position
    SCY_REG = tmpY; SCX_REG = tmpX; 
    // up or down
    scene->map_pos_y = (uint8_t)(tmpY >> 3u);
    if (scene->map_pos_y != scene->old_map_pos_y) { 
        if (tmpY < scene->old_camera_y) {
            set_bkg_submap(scene->map_pos_x, scene->map_pos_y, MIN(21u, scene->sceneW - scene->map_pos_x), 1, scene->sceneData, scene->sceneW);
        } else {
            if ((scene->sceneH - 18u) > scene->map_pos_y)
                set_bkg_submap(scene->map_pos_x, scene->map_pos_y + 18u, MIN(21u, scene->sceneW - scene->map_pos_x), 1, scene->sceneData, scene->sceneW);     
        }
        scene->old_map_pos_y = scene->map_pos_y; 
    }
    // left or right
    scene->map_pos_x = (uint8_t)(tmpX >> 3u);
    if (scene->map_pos_x != scene->old_map_pos_x) {
        if (tmpX < scene->old_camera_x) {
            set_bkg_submap(scene->map_pos_x, scene->map_pos_y, 1, MIN(19u, scene->sceneH - scene->map_pos_y), scene->sceneData, scene->sceneW);     
        } else {
            if ((scene->sceneW - 20u) > scene->map_pos_x)
                set_bkg_submap(scene->map_pos_x + 20u, scene->map_pos_y, 1, MIN(19u, scene->sceneH - scene->map_pos_y), scene->sceneData, scene->sceneW);     
        }
        scene->old_map_pos_x = scene->map_pos_x;
    }
    // set old camera position to current camera position
    scene->old_camera_x = tmpX, scene->old_camera_y = tmpY;
}

void updateView(Scene_t* scene) {
    if (scene->redraw) {        
        wait_vbl_done();
        setCamera(scene);
        scene->redraw = FALSE;
    } else wait_vbl_done();
}

void destroyEntity(Entity_t* entity) {
    // ToDO: free shoots
}
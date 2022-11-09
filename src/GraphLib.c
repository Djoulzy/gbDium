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
    bullet->coord.sceneX = bullet->coord.sceneY = 0;
    bullet->coord.visibleX = bullet->coord.visibleY = 0;
    bullet->coord.cameraStick = FALSE;
    set_sprite_tile(bullet->spriteNum, tileNum);
    return bullet;
}

void setupEntity(Entity_t* entity, const metasprite_t** frames, int16_t sceneX, int16_t sceneY) {
    entity->spriteNum = spritesCount;
    spritesCount += 4;
    entity->active = FALSE;
    entity->coord.sceneX = sceneX;
    entity->coord.sceneY = sceneY;
    entity->speedX = entity->speedY = 0;
    entity->animStep = 0;
    entity->frames = frames;
    entity->bullets = NULL;
}

void assignBulletsToEntity(Entity_t* entity, Bullet_t** bullets, uint8_t size, uint8_t delay) {
    entity->bullets = bullets;
    entity->nb_shoots = size;
    entity->availableShoot = 0;
    entity->shootDelay = delay;
    entity->shootDelayCpt = 0;
}

void entityShoot(Entity_t* entity, int8_t spdx, int8_t spdy, uint8_t props) {
    if ((entity->availableShoot < entity->nb_shoots) && (!entity->shootDelayCpt)) {
        entity->bullets[entity->availableShoot]->speedX = spdx;
        entity->bullets[entity->availableShoot]->speedY = spdy;

        entity->bullets[entity->availableShoot]->active = TRUE;
        if (entity->coord.cameraStick) {
            entity->bullets[entity->availableShoot]->coord.visibleX = entity->coord.visibleX;
            entity->bullets[entity->availableShoot]->coord.visibleY = entity->coord.visibleY;
        } else {
            entity->bullets[entity->availableShoot]->coord.sceneX = entity->coord.sceneX;
            entity->bullets[entity->availableShoot]->coord.sceneY = entity->coord.sceneY;
        }
        if (props) set_sprite_prop(entity->bullets[entity->availableShoot]->spriteNum, props);
        entity->shootDelayCpt = entity->shootDelay;
        entity->availableShoot = entity->nb_shoots;
    }
}

void moveEntityBullets(Scene_t* scene, Entity_t* entity) {
    for (uint8_t i = 0; i < entity->nb_shoots; i++) {
        if (entity->bullets[i]->active) {
            // dumpEntity(entity);
            if (entity->coord.cameraStick) {
                entity->bullets[i]->coord.visibleX += entity->bullets[i]->speedX;
                entity->bullets[i]->coord.visibleY += entity->bullets[i]->speedY;
            } else {
                entity->bullets[i]->coord.sceneX += entity->bullets[i]->speedX;
                entity->bullets[i]->coord.sceneY += entity->bullets[i]->speedY;
                entity->bullets[i]->coord.visibleX = (entity->bullets[i]->coord.sceneX - scene->camera_x) >> SCREEN_SCALE;
                entity->bullets[i]->coord.visibleY = (entity->bullets[i]->coord.sceneY - scene->camera_y) >> SCREEN_SCALE;
            }

            move_sprite(entity->bullets[i]->spriteNum, entity->bullets[i]->coord.visibleX, entity->bullets[i]->coord.visibleY);
            if (isOutOfViewPort(scene, &(entity->bullets[i]->coord))) {
                entity->bullets[i]->active = FALSE;
                entity->availableShoot = i;
            }
        } else entity->availableShoot = i;
    }
    if (entity->shootDelayCpt) entity->shootDelayCpt--;
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
    entity->coord.cameraStick = TRUE;
}

int8_t isOutOfViewPort(Scene_t* scene, Coord_t* coord) {
    int8_t out = 0;
    
    if (coord->visibleX > VIEWPORT_WIDTH - VIEWPORT_BORDER_SIZE)  {
        coord->visibleX = VIEWPORT_WIDTH - VIEWPORT_BORDER_SIZE;
        coord->sceneX = (int16_t)(scene->screenWidth) - (int16_t)(SCREEN_BORDER_SIZE);
        if (coord->cameraStick) scene->camera_x = scene->camera_max_x;
        out |= OUT_RIGHT;
    }
    else if (coord->visibleX < VIEWPORT_BORDER_SIZE)  {
        coord->visibleX = VIEWPORT_BORDER_SIZE;
        coord->sceneX = SCREEN_BORDER_SIZE;
        if (coord->cameraStick) scene->camera_x = 0;
        out |= OUT_LEFT;
    }

    if (coord->visibleY > VIEWPORT_HEIGHT - VIEWPORT_BORDER_SIZE) {
        coord->visibleY = VIEWPORT_HEIGHT - VIEWPORT_BORDER_SIZE;
        coord->sceneY = (int16_t)(scene->screenHeight) - (int16_t)(SCREEN_BORDER_SIZE);
        if (coord->cameraStick) scene->camera_y = scene->camera_max_y;
        out |= OUT_DOWN;
    }
    else if (coord->visibleY < VIEWPORT_BORDER_SIZE) {
        coord->visibleY = VIEWPORT_BORDER_SIZE ;
        coord->sceneY = SCREEN_BORDER_SIZE;
        if (coord->cameraStick) scene->camera_y = 0;
        out |= OUT_UP;
    }

    return out;
}

void updateEntityPos(Scene_t* scene, Entity_t* entity) {
    entity->coord.sceneX += entity->speedX;
    entity->coord.sceneY += entity->speedY;

    if (entity->coord.cameraStick) {
        if ((entity->coord.sceneX >= scene->startScrollZoneX) && (entity->coord.sceneX <= scene->endScrollZoneX)) {
            scene->camera_x += entity->speedX;
            if (scene->camera_x > scene->camera_max_x) scene->camera_x = scene->camera_max_x;
            if (scene->camera_x < 0) scene->camera_x = 0;
            scene->redraw = TRUE;
        }
        
        if ((entity->coord.sceneY >= scene->startScrollZoneY) && (entity->coord.sceneY <= scene->endScrollZoneY)) {
            scene->camera_y += entity->speedY;
            // EMU_printf("CamX: %d (%d) - CamY: %d (%d) - AbsX: %d - AbsY: %d - ShpX: %d - ShpY: %d", camera_x, camera_max_x, camera_y, camera_max_y, ShipAbsX, ShipAbsY, tmpX, tmpY);
            if (scene->camera_y > scene->camera_max_y) scene->camera_y = scene->camera_max_y;
            if (scene->camera_y < 0) scene->camera_y = 0;
            scene->redraw = TRUE;
        }
    }

    entity->coord.visibleX = ((entity->coord.sceneX - scene->camera_x) >> SCREEN_SCALE);
    entity->coord.visibleY = ((entity->coord.sceneY - scene->camera_y) >> SCREEN_SCALE);
}

uint8_t isVisible(Entity_t* entity) {
    if ((entity->coord.visibleX>=0) && (entity->coord.visibleX<=176)
        && (entity->coord.visibleY>=0) && (entity->coord.visibleY<=160))
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
    for (uint8_t i = 0; i < entity->nb_shoots; i++)
        free(entity->bullets[i]);
}

void dumpEntity(Entity_t* entity) {
    Bullet_t *b;

    EMU_printf("=== Enity: %d ===", entity->spriteNum);
    EMU_printf("Active: %d", entity->active);
    EMU_printf("Scene Coord: %d x %d", entity->coord.sceneX, entity->coord.sceneY);
    EMU_printf("ViewPort Coord: %d x %d", entity->coord.visibleX, entity->coord.visibleY);
    EMU_printf("Camera follow: %d", entity->coord.cameraStick);
    EMU_printf("Max Bullets: %d", entity->nb_shoots);
    for(uint8_t i = 0; i<entity->nb_shoots; i++) {
        b = entity->bullets[i];
        EMU_printf("  %d) Bullets %d: Active: %d Coord: (%d x %d)(%d x %d)", i, b->spriteNum, b->active, b->coord.sceneX, b->coord.sceneY, b->coord.visibleX, b->coord.visibleY);
    }
    EMU_printf("-");
}
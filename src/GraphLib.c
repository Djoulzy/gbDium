#include <gb/gb.h>
#include <stdlib.h>
#include <gbdk/emu_debug.h>
#include "GraphLib.h"

uint8_t spritesCount = 0;

Entity_t LAST_ENTITY = { .active = 255 };

Bullet_t* allocBullet(uint8_t tileNum) {
    Bullet_t* bullet = malloc(sizeof(Bullet_t));

    bullet->spriteNum = spritesCount;
    // EMU_printf("new sprite: %d", shoot->spriteNum);
    spritesCount++;
    bullet->active = FALSE;
    bullet->speedX = bullet->speedY = 0;
    bullet->coord.X = bullet->coord.Y = 0;
    bullet->coord.cameraStick = FALSE;
    bullet->coord.overlapped = FALSE;
    set_sprite_tile(bullet->spriteNum, tileNum);
    return bullet;
}

Entity_t*  allocEntity(const metasprite_t** frames, int16_t X, int16_t Y) {
    Entity_t* entity = malloc(sizeof(Entity_t));

    entity->spriteNum = spritesCount;
    spritesCount += 4;
    entity->active = TRUE;
    entity->coord.X = X;
    entity->coord.Y = Y;
    entity->coord.overlapped = FALSE;
    entity->coord.upscaledX = X << SCREEN_SCALE;
    entity->coord.upscaledY = Y << SCREEN_SCALE;
    entity->speedX = entity->speedY = 0;
    entity->animStep = 0;
    entity->frames = frames;
    entity->bullets = NULL;

    return entity;
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

        entity->bullets[entity->availableShoot]->coord.X = entity->coord.X;
        entity->bullets[entity->availableShoot]->coord.Y = entity->coord.Y;

        if (props) set_sprite_prop(entity->bullets[entity->availableShoot]->spriteNum, props);
        entity->shootDelayCpt = entity->shootDelay;
        entity->availableShoot = entity->nb_shoots;
    }
}

// UINT8 collision(UINT8 x1, UINT8 y1, UINT8 w1, UINT8 h1, UINT8 x2, UINT8 y2, UINT8 w2, UINT8 h2) {  //boudingbox
//     return x1 < x2+w2 &&
//          x2 < x1+w1 &&
//          y1 < y2+h2 &&
//          y2 < y1+h1;
//     }

uint8_t overlap(Coord_t* obj1, Coord_t* obj2) {
    return obj1->viewportX < obj2->viewportX+16 &&
         obj2->viewportX < obj1->viewportX+8 &&
         obj1->viewportY < obj2->viewportY+8 &&
         obj2->viewportY < obj1->viewportY+16;
}

void moveEntityBullets(Scene_t* scene, Entity_t* entity, Entity_t** target) {
    uint8_t collision = FALSE;

    for (uint8_t i = 0; i < entity->nb_shoots; i++) {
        if (entity->bullets[i]->active) {
            entity->bullets[i]->coord.X += entity->bullets[i]->speedX;
            entity->bullets[i]->coord.Y += entity->bullets[i]->speedY;
            entity->bullets[i]->coord.viewportX = entity->bullets[i]->coord.X - scene->camera_x;
            entity->bullets[i]->coord.viewportY = entity->bullets[i]->coord.Y - scene->camera_y;

            for (uint8_t j=0; target[j]->active != 255; j++) {
                if (overlap(&(entity->bullets[i]->coord), &(target[j]->coord))) {
                    target[j]->coord.overlapped = TRUE;
                    target[j]->animStep = 0;
                    collision = TRUE;
                    break;
                }
            }

            if (!collision && isVisible(&(entity->bullets[i]->coord))) {
                move_sprite(entity->bullets[i]->spriteNum, entity->bullets[i]->coord.viewportX, entity->bullets[i]->coord.viewportY);
            } else {
                entity->bullets[i]->active = FALSE;
                entity->availableShoot = i;
                hide_sprite(entity->bullets[i]->spriteNum);
            }
        } else entity->availableShoot = i;
    }
    if (entity->shootDelayCpt) entity->shootDelayCpt--;
}

void setupScene(Scene_t* tmp, const uint8_t* sceneData, uint8_t nb_tiles_w, uint8_t nb_tiles_h) {
    tmp->sceneData = sceneData;
    tmp->nb_tiles_w = nb_tiles_w;
    tmp->nb_tiles_h = nb_tiles_h;

    tmp->camera_max_x = (nb_tiles_w - 20) << 3;
    tmp->camera_max_y = (nb_tiles_h - 18) << 3;

    tmp->map_pos_x = tmp->map_pos_y = 0; 
    tmp->old_map_pos_x = tmp->old_map_pos_y = 255;
    set_bkg_submap(tmp->map_pos_x, tmp->map_pos_y, 20, 18, tmp->sceneData, tmp->nb_tiles_w);

    tmp->old_camera_x = tmp->camera_x = 0;
    tmp->old_camera_y = tmp->camera_y = 0;

    tmp->sceneWidth = nb_tiles_w << 3;
    tmp->sceneHeight = nb_tiles_h << 3;
    tmp->startScrollZoneX = START_SCROLL_X;
    tmp->startScrollZoneY = START_SCROLL_Y;
    tmp->endScrollZoneX = tmp->sceneWidth - START_SCROLL_X;
    tmp->endScrollZoneY = tmp->sceneHeight - START_SCROLL_Y;
}

void setCameraStick(Entity_t* entity) {
    entity->coord.cameraStick = TRUE;
}

uint8_t isVisible(Coord_t* coord) {
    if ((coord->viewportX) && (coord->viewportX<=176) && (coord->viewportY) && (coord->viewportY<=160))
        return TRUE;
    return FALSE;
}

int8_t isOutOfScene(Scene_t* scene, Coord_t* coord) {
    int8_t out = 0;
    
    if (coord->X > scene->sceneWidth - SCENE_BORDER_SIZE)  {
        coord->X = scene->sceneWidth - SCENE_BORDER_SIZE;
        coord->upscaledX = coord->X << SCREEN_SCALE;
        if (coord->cameraStick) scene->camera_x = scene->camera_max_x;
        out |= OUT_RIGHT;
    }
    else if (coord->X < SCENE_BORDER_SIZE)  {
        coord->X = SCENE_BORDER_SIZE;
        coord->upscaledX = coord->X << SCREEN_SCALE;
        if (coord->cameraStick) scene->camera_x = 0;
        out |= OUT_LEFT;
    }

    if (coord->Y > scene->sceneHeight - SCENE_BORDER_SIZE) {
        coord->Y = scene->sceneHeight - SCENE_BORDER_SIZE;
        coord->upscaledY = coord->Y << SCREEN_SCALE;
        if (coord->cameraStick) scene->camera_y = scene->camera_max_y;
        out |= OUT_DOWN;
    }
    else if (coord->Y < SCENE_BORDER_SIZE) {
        coord->Y = SCENE_BORDER_SIZE ;
        coord->upscaledY = coord->Y << SCREEN_SCALE;
        if (coord->cameraStick) scene->camera_y = 0;
        out |= OUT_UP;
    }

    return out;
}

void updatePlayerPos(Scene_t* scene, Entity_t* entity) {
    entity->coord.X = entity->coord.upscaledX >> SCREEN_SCALE;
    entity->coord.Y = entity->coord.upscaledY >> SCREEN_SCALE;

    if (entity->coord.cameraStick) {
        if ((entity->coord.X >= scene->startScrollZoneX) && (entity->coord.X <= scene->endScrollZoneX)) {
            scene->camera_x = entity->coord.X - scene->startScrollZoneX;
            scene->redraw = TRUE;
        }
        
        if ((entity->coord.Y >= scene->startScrollZoneY) && (entity->coord.Y <= scene->endScrollZoneY)) {
            scene->camera_y = entity->coord.Y - scene->startScrollZoneY;
            scene->redraw = TRUE;
        }
    }

    entity->coord.viewportX = entity->coord.X - scene->camera_x;
    entity->coord.viewportY = entity->coord.Y - scene->camera_y;
}

void updateMobPos(Scene_t* scene, Entity_t* entity) {
    entity->coord.viewportX = entity->coord.X - scene->camera_x;
    entity->coord.viewportY = entity->coord.Y - scene->camera_y;
}

void setCamera(Scene_t* scene) {
    // update hardware scroll position
    SCY_REG = scene->camera_y; SCX_REG = scene->camera_x; 

    scene->map_pos_x = (uint8_t)(scene->camera_x >> 3u);
    scene->map_pos_y = (uint8_t)(scene->camera_y >> 3u);
    // left or right

    if (scene->map_pos_x != scene->old_map_pos_x) {
        if (scene->camera_x < scene->old_camera_x) {
            set_bkg_submap(scene->map_pos_x, scene->map_pos_y, SCROLL_STEPS, MIN(19u, scene->nb_tiles_h - scene->map_pos_y), scene->sceneData, scene->nb_tiles_w);     
        } else {
            if ((scene->nb_tiles_w - 20u) > scene->map_pos_x)
                set_bkg_submap(scene->map_pos_x + 20u, scene->map_pos_y, SCROLL_STEPS, MIN(19u, scene->nb_tiles_h - scene->map_pos_y), scene->sceneData, scene->nb_tiles_w);     
        }
        scene->old_map_pos_x = scene->map_pos_x;
    }
    // up or down
    if (scene->map_pos_y != scene->old_map_pos_y) { 
        if (scene->camera_y < scene->old_camera_y) {
            set_bkg_submap(scene->map_pos_x, scene->map_pos_y, MIN(21u, scene->nb_tiles_w - scene->map_pos_x), SCROLL_STEPS, scene->sceneData, scene->nb_tiles_w);
        } else {
            if ((scene->nb_tiles_h - 18u) > scene->map_pos_y)
                set_bkg_submap(scene->map_pos_x, scene->map_pos_y + 18u, MIN(21u, scene->nb_tiles_w - scene->map_pos_x), SCROLL_STEPS, scene->sceneData, scene->nb_tiles_w);     
        }
        scene->old_map_pos_y = scene->map_pos_y; 
    }

    // set old camera position to current camera position
    scene->old_camera_x = scene->camera_x, scene->old_camera_y = scene->camera_y;
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
    EMU_printf("Scene Coord: %d x %d", entity->coord.X, entity->coord.Y);
    EMU_printf("ViewPort Coord: %d x %d", entity->coord.viewportX, entity->coord.viewportY);
    EMU_printf("Camera follow: %d", entity->coord.cameraStick);
    EMU_printf("Max Bullets: %d", entity->nb_shoots);
    for(uint8_t i = 0; i<entity->nb_shoots; i++) {
        b = entity->bullets[i];
        EMU_printf("  %d) Bullets %d: Active: %d Coord: (%d x %d)(%d x %d)", i, b->spriteNum, b->active, b->coord.upscaledX, b->coord.upscaledY, b->coord.X, b->coord.Y);
    }
    EMU_printf("-");
}
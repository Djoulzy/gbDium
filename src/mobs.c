#include <gb/gb.h>
#include <gbdk/platform.h>
#include <stdlib.h>
#include <rand.h>
#include "GraphLib.h"
#include "mobs.h"
#include "player.h"
#include "../res/alien_tiles.h"

EntityList_t* alienList = NULL;
BulletList_t* aliens_bullet[NB_ALIENS];
uint8_t alien_turn = NB_ALIENS;

const metasprite_t alien1[]         = { METASPR_ITEM(0, -8, 54, 0), METASPR_ITEM(0, 8, 56, 0), METASPR_TERM };
const metasprite_t alien2[]         = { METASPR_ITEM(0, -8, 58, 0), METASPR_ITEM(0, 8, 60, 0), METASPR_TERM };
const metasprite_t alien_boom1[]    = { METASPR_ITEM(0, -8, 62, 0), METASPR_ITEM(0, 8, 64, 0), METASPR_TERM };
const metasprite_t alien_boom2[]    = { METASPR_ITEM(0, -8, 66, 0), METASPR_ITEM(0, 8, 68, 0), METASPR_TERM };

const metasprite_t* const alienFrames[4] = {
    alien1, alien2, alien_boom1, alien_boom2
};

const uint8_t anim_mobs[20] = {0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1};
const uint8_t anim_boom[20] = {2,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3,3};

void initAliens(void) {
    uint8_t i;

    set_sprite_data(54, 18, alien_tiles);

    initrand(DIV_REG);
    for (i = 0; i<NB_ALIENS; i++) {
        alienList = addEntityToList(alienList, alienFrames, rand() << 2, rand());
        alienList->entity->shootDelay = 1;
        prepareBulletList(alienList->entity, 70, 1);
    }
}

void destroyed(Scene_t* scene, Entity_t* entity) {
    updateMobPos(scene, entity);
    if (isVisible(&entity->coord))
        move_metasprite(alienFrames[anim_boom[entity->animStep]], 0, entity->spriteNum, entity->coord.viewportX, entity->coord.viewportY);
    entity->animStep++;
    if (entity->animStep > 19) {
        entity->active = FALSE;
        hide_metasprite(alienFrames[0], entity->spriteNum);
        entity->bullets->entity->active = FALSE;
        hide_sprite(entity->bullets->entity->spriteNum);
    }
}

void alienShoot(Entity_t* entity, Coord_t* playerCoord) {
    int8_t spdx = 0, spdy = 0;

    if (playerCoord->X > entity->coord.X) spdx = ALIEN_SHOOT_SPEED;
    else if (playerCoord->X < entity->coord.X) spdx = -ALIEN_SHOOT_SPEED;
    if (playerCoord->Y > entity->coord.Y) spdy = ALIEN_SHOOT_SPEED;
    else if (playerCoord->Y < entity->coord.Y) spdy = -ALIEN_SHOOT_SPEED;
    entityShoot(entity, spdx, spdy, 0);
}

void alienMoves(Scene_t* scene, Coord_t* playerCoord) {
    EntityList_t *p = alienList;
    
    alien_turn--;
    while(p != NULL) {
        if (p->entity->active) {
            if (p->entity->coord.overlapped) destroyed(scene, p->entity);
            else if (!alien_turn) {
                if (playerCoord->X > p->entity->coord.X) p->entity->speedX += ALIEN_SPEED; else p->entity->speedX -= ALIEN_SPEED;
                if (p->entity->speedX > 16) p->entity->speedX = 16;
                if (p->entity->speedX < -16) p->entity->speedX = -16;
                if (playerCoord->Y > p->entity->coord.Y) p->entity->speedY += ALIEN_SPEED; else p->entity->speedY -= ALIEN_SPEED;
                if (p->entity->speedY > 16) p->entity->speedY = 16;
                if (p->entity->speedY < -16) p->entity->speedY = -16;
                p->entity->coord.X += p->entity->speedX >> 4;
                p->entity->coord.Y += p->entity->speedY >> 4;
                updateMobPos(scene, p->entity);
                
                if (isVisible(&p->entity->coord)) {
                    
                    if (shootOk(p->entity)) alienShoot(p->entity, playerCoord);

                    p->entity->animStep++;
                    if (p->entity->animStep > 19) p->entity->animStep = 0;
                    move_metasprite(alienFrames[anim_mobs[p->entity->animStep]], 0, p->entity->spriteNum, p->entity->coord.viewportX, p->entity->coord.viewportY);
                }
            } else {
                updateMobPos(scene, p->entity);
                if (isVisible(&(p->entity->coord)))
                    move_metasprite(alienFrames[anim_mobs[p->entity->animStep]], 0, p->entity->spriteNum, p->entity->coord.viewportX, p->entity->coord.viewportY);
            }

            moveEntityBullets(scene, p->entity, player);
        }
        p = p->suiv;
    }
    if (!alien_turn) alien_turn = 4;
}
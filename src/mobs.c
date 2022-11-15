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

const metasprite_t alien1[] = {
    METASPR_ITEM(0, -8, 53, 0), METASPR_ITEM(8, 0, 54, 0), METASPR_ITEM(-8, 8, 55, 0), METASPR_ITEM(8, 0, 56, 0),
    METASPR_TERM
};

const metasprite_t alien2[] = {
    METASPR_ITEM(0, -8, 57, 0), METASPR_ITEM(8, 0, 58, 0), METASPR_ITEM(-8, 8, 59, 0), METASPR_ITEM(8, 0, 60, 0),
    METASPR_TERM
};

const metasprite_t alien_boom1[] = {
    METASPR_ITEM(0, -8, 61, 0), METASPR_ITEM(8, 0, 62, 0), METASPR_ITEM(-8, 8, 63, 0), METASPR_ITEM(8, 0, 64, 0),
    METASPR_TERM
};

const metasprite_t alien_boom2[] = {
    METASPR_ITEM(0, -8, 65, 0), METASPR_ITEM(8, 0, 66, 0), METASPR_ITEM(-8, 8, 67, 0), METASPR_ITEM(8, 0, 68, 0),
    METASPR_TERM
};

const metasprite_t* const alienFrames[4] = {
    alien1, alien2, alien_boom1, alien_boom2
};

void initAliens(void) {
    uint8_t i;

    set_sprite_data(53, 17, alien_tiles);

    initrand(DIV_REG);
    for (i = 0; i<NB_ALIENS; i++) {
        alienList = addEntityToList(alienList, alienFrames, rand() << 2, rand());
        alienList->entity->shootDelay = 1;
        prepareBulletList(alienList->entity, 69, 1);
    }
}

void destroyed(Scene_t* scene, Entity_t* entity) {
    updateMobPos(scene, entity);
    if (isVisible(&entity->coord))
        move_metasprite(alienFrames[entity->animStep+2], 0, entity->spriteNum, entity->coord.viewportX, entity->coord.viewportY);
    entity->animStep++;
    if (entity->animStep > 1) {
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
            else if (alien_turn) {
                if (playerCoord->X > p->entity->coord.X) p->entity->speedX = ALIEN_SPEED; else p->entity->speedX = -ALIEN_SPEED;
                if (playerCoord->Y > p->entity->coord.Y) p->entity->speedY = ALIEN_SPEED; else p->entity->speedY = -ALIEN_SPEED;
                p->entity->coord.X += p->entity->speedX;
                p->entity->coord.Y += p->entity->speedY;
                updateMobPos(scene, p->entity);
                
                if (isVisible(&p->entity->coord)) {
                    
                    if (shootOk(p->entity)) alienShoot(p->entity, playerCoord);

                    p->entity->animStep = abs(p->entity->animStep -1);
                    move_metasprite(alienFrames[p->entity->animStep], 0, p->entity->spriteNum, p->entity->coord.viewportX, p->entity->coord.viewportY);
                }
            } else {
                updateMobPos(scene, p->entity);
                if (isVisible(&(p->entity->coord)))
                    move_metasprite(alienFrames[p->entity->animStep], 0, p->entity->spriteNum, p->entity->coord.viewportX, p->entity->coord.viewportY);
            }

            moveEntityBullets(scene, p->entity, player);
        }
        p = p->suiv;
    }
    if (!alien_turn) alien_turn = NB_ALIENS;
}
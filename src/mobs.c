#include <gb/gb.h>
#include <gbdk/emu_debug.h>
#include <gbdk/platform.h>
#include <stdlib.h>
#include <rand.h>
#include "GraphLib.h"
#include "mobs.h"
#include "player.h"
#include "../res/alien_tiles.h"

Entity_t* alienTroop[NB_ALIENS+1];
EntityList_t* alienList = NULL;

Bullet_t* aliens_bullet[NB_ALIENS][1];
uint8_t alien_turn = NB_ALIENS;
Entity_t* target[2];

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
    EntityList_t *plist, *newElmt;

    set_sprite_data(53, 17, alien_tiles);

    initrand(DIV_REG);
    for (i = 0; i<NB_ALIENS; i++) {
        alienTroop[i] = allocEntity(alienFrames, rand() << 2, rand());
        aliens_bullet[i][0] = allocBullet(69);
        assignBulletsToEntity(alienTroop[i], aliens_bullet[i], 1, 1);

        newElmt = malloc(sizeof(EntityList_t));
        newElmt->entity = alienTroop[i];
        newElmt->suiv = alienList;
        alienList = newElmt;

        // dumpEntity(&alienTroop[i]);
    }
    alienTroop[NB_ALIENS] = &LAST_ENTITY;
    target[0] = player;
    target[1] = &LAST_ENTITY;

    plist = alienList;
    while(plist != NULL) {
        dumpEntity(plist->entity);
        plist = plist->suiv;
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
        entity->bullets[0]->active = FALSE;
        hide_sprite(entity->bullets[0]->spriteNum);
    }
}

void alienMoves(Scene_t* scene, Coord_t* playerCoord) {
    uint8_t i;
    EntityList_t *plist = alienList;
    int8_t spdx, spdy;

    alien_turn--;
    for (i = 0; i<NB_ALIENS; i++) {
        if (alienTroop[i]->active) {
            if (alienTroop[i]->coord.overlapped) destroyed(scene, alienTroop[i]);
            else if (i == alien_turn) {
                if (playerCoord->X > alienTroop[i]->coord.X) alienTroop[i]->speedX = ALIEN_SPEED; else alienTroop[i]->speedX = -ALIEN_SPEED;
                if (playerCoord->Y > alienTroop[i]->coord.Y) alienTroop[i]->speedY = ALIEN_SPEED; else alienTroop[i]->speedY = -ALIEN_SPEED;
                alienTroop[i]->coord.X += alienTroop[i]->speedX;
                alienTroop[i]->coord.Y += alienTroop[i]->speedY;
                updateMobPos(scene, alienTroop[i]);
                
                if (isVisible(&alienTroop[i]->coord)) {
                    spdx = spdy = 0;
                    if (!alienTroop[i]->bullets[0]->active) {
                        if (playerCoord->X > alienTroop[i]->coord.X) spdx = ALIEN_SHOOT_SPEED;
                        else if (playerCoord->X < alienTroop[i]->coord.X) spdx = -ALIEN_SHOOT_SPEED;
                        if (playerCoord->Y > alienTroop[i]->coord.Y) spdy = ALIEN_SHOOT_SPEED;
                        else if (playerCoord->Y < alienTroop[i]->coord.Y) spdy = -ALIEN_SHOOT_SPEED;
                        entityShoot(alienTroop[i], spdx, spdy, 0);
                    }

                    alienTroop[i]->animStep = abs(alienTroop[i]->animStep -1);
                    move_metasprite(alienFrames[alienTroop[i]->animStep], 0, alienTroop[i]->spriteNum, alienTroop[i]->coord.viewportX, alienTroop[i]->coord.viewportY);
                }
            } else {
                updateMobPos(scene, alienTroop[i]);
                if (isVisible(&(alienTroop[i]->coord)))
                    move_metasprite(alienFrames[alienTroop[i]->animStep], 0, alienTroop[i]->spriteNum, alienTroop[i]->coord.viewportX, alienTroop[i]->coord.viewportY);
            }

            moveEntityBullets(scene, alienTroop[i], target);
        }
    }
    if (!alien_turn) alien_turn = NB_ALIENS;
}
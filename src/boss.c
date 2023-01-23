#include <gb/gb.h>
#include <gbdk/platform.h>
#include <stdlib.h>
#include <rand.h>
#include "GraphLib.h"
#include "boss.h"

#include "../res/bigShip.h"
#include "../res/ShipGun.h"

extern joypads_t joypads;
extern Scene_t scene;

EntityList_t* shipgunList = NULL;
BulletList_t* shipgun_bullet[NB_SHIPGUN];

const metasprite_t shipgun[]         = { METASPR_ITEM(0, -8, 54, 0), METASPR_ITEM(0, 8, 56, 0), METASPR_TERM };
const metasprite_t* const shipgunFrames[1] = {
    shipgun
};

uint8_t bossX, bossY;
int8_t bossSpeedX, bossSpeedY;
int8_t tempo;

void initBoss() {
    int8_t i;

    set_bkg_data(0, bigShip_TILE_COUNT, bigShip_tiles);
    set_bkg_tiles(21, 0, bigShip_WIDTH >> 3, bigShip_HEIGHT >> 3, bigShip_map);
    bossX = 21 << 3;
    bossY = 0;
    bossSpeedX = -1;
    bossSpeedY = 1;
    tempo = 3;

    set_sprite_data(54, 4, ShipGun);

    for (i = 0; i<NB_SHIPGUN; i++) {
        shipgunList = addEntityToList(shipgunList, shipgunFrames, 10, 10);
        shipgunList->entity->shootDelay = 1;
        prepareBulletList(shipgunList->entity, 70, 1);
    }
}

void bossMove() {
    EntityList_t *p = shipgunList;
    tempo--;

    if (tempo <= 0) { 
        bossX += bossSpeedX;
        bossY += bossSpeedY;
        scroll_bkg(-bossSpeedX, -bossSpeedY);

        if (bossX < 80) bossSpeedX = 1;
        else if (bossX > 96) bossSpeedX = -1;

        if (bossY < 1) bossSpeedY = 1;
        else if (bossY > 44) bossSpeedY = -1;
        tempo = 3;
    }

    while(p != NULL) {
        if (p->entity->active) {
            updateMobPos(&scene, p->entity);
            if (isVisible(&(p->entity->coord)))
                move_metasprite(shipgunFrames[0], 0, p->entity->spriteNum, p->entity->coord.viewportX, p->entity->coord.viewportY);
        }
        p = p->suiv;
    }
}
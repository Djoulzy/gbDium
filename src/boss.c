#include <gb/gb.h>
#include <gbdk/platform.h>
#include <stdlib.h>
#include <rand.h>
#include "GraphLib.h"
#include "player.h"
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
EntityList_t *p;

void initBoss() {
    int8_t i;

    set_bkg_data(0, bigShip_TILE_COUNT, bigShip_tiles);
    set_bkg_tiles(21, 0, bigShip_WIDTH >> 3, bigShip_HEIGHT >> 3, bigShip_map);
    bossX = 21 << 3;
    bossY = 0;
    bossSpeedX = -1;
    bossSpeedY = 1;
    tempo = 3;

    set_sprite_data(54, 6, ShipGun);

    for (i = 0; i<NB_SHIPGUN; i++) {
        shipgunList = addEntityToList(shipgunList, shipgunFrames, bossX, bossY);
        shipgunList->entity->shootDelay = 1;
        prepareBulletList(shipgunList->entity, 58, 1);
    }

    p = shipgunList;
    p->entity->coord.X += (7 << 3);
    p->entity->coord.Y += (4 << 3);
    p = p->suiv;
    p->entity->coord.X += (4 << 3);
    p->entity->coord.Y += (7 << 3);
    p = p->suiv;
    p->entity->coord.X += (7 << 3);
    p->entity->coord.Y += (10 << 3);
}

void bossShoot(Entity_t* entity, Coord_t* playerCoord) {
    int8_t spdx = 0, spdy = 0;

    // if (playerCoord->X > entity->coord.X) spdx = ALIEN_SHOOT_SPEED;
    // else if (playerCoord->X < entity->coord.X) spdx = -ALIEN_SHOOT_SPEED;
    // if (playerCoord->Y > entity->coord.Y) spdy = ALIEN_SHOOT_SPEED;
    // else if (playerCoord->Y < entity->coord.Y) spdy = -ALIEN_SHOOT_SPEED;
    entityShoot(entity, -2, 0, 0);
}

void bossMove(Scene_t* scene, Coord_t* playerCoord) {
    tempo--;

    if (tempo <= 0) {

        bossX += bossSpeedX;
        bossY += bossSpeedY;
    
            if (bossX < 80) bossSpeedX = 1;
        else if (bossX > 96) bossSpeedX = -1;

        if (bossY < 1) bossSpeedY = 1;
        else if (bossY > 44) bossSpeedY = -1;

        p = shipgunList;
        while(p != NULL) {
            if (p->entity->active) {
                p->entity->coord.X += bossSpeedX;
                p->entity->coord.Y += bossSpeedY;
                if (shootOk(p->entity)) bossShoot(p->entity, playerCoord);
                move_metasprite(shipgunFrames[0], 0, p->entity->spriteNum, p->entity->coord.X, p->entity->coord.Y);
            }
            moveEntityBullets(scene, p->entity, player);
            p = p->suiv;
        }
        tempo = 3;
        wait_vbl_done();
        scroll_bkg(-bossSpeedX, -bossSpeedY);
    } else {
        wait_vbl_done();
    }
}
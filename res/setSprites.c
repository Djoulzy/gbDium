#include <stdint.h>
#include <gbdk/platform.h>
#include <gbdk/metasprites.h>

#include "Ship_16x16.h"

const metasprite_t ship_normal[]            = { METASPR_ITEM(0, -8, 0, 0),  METASPR_ITEM(0, 8, 2, 0),  METASPR_TERM };
const metasprite_t ship_up1[]               = { METASPR_ITEM(0, -8, 4, 0),  METASPR_ITEM(0, 8, 6, 0),  METASPR_TERM };
const metasprite_t ship_up2[]               = { METASPR_ITEM(0, -8, 8, 0),  METASPR_ITEM(0, 8, 10, 0), METASPR_TERM };
const metasprite_t ship_up3[]               = { METASPR_ITEM(0, -8, 12, 0), METASPR_ITEM(0, 8, 14, 0), METASPR_TERM };
const metasprite_t ship_up4[]               = { METASPR_ITEM(0, -8, 16, 0), METASPR_ITEM(0, 8, 18, 0), METASPR_TERM };
const metasprite_t ship_retournement1[]     = { METASPR_ITEM(0, -8, 20, 0), METASPR_ITEM(0, 8, 22, 0), METASPR_TERM };
const metasprite_t ship_retournement2[]     = { METASPR_ITEM(0, -8, 24, 0), METASPR_ITEM(0, 8, 26, 0), METASPR_TERM };
const metasprite_t ship_retournement3[]     = { METASPR_ITEM(0, -8, 28, 0), METASPR_ITEM(0, 8, 30, 0), METASPR_TERM };
const metasprite_t ship_retournement4[]     = { METASPR_ITEM(0, -8, 32, 0), METASPR_ITEM(0, 8, 34, 0), METASPR_TERM };
const metasprite_t ship_retournement5[]     = { METASPR_ITEM(0, -8, 36, 0), METASPR_ITEM(0, 8, 38, 0), METASPR_TERM };
const metasprite_t ship_retournement6[]     = { METASPR_ITEM(0, -8, 40, 0), METASPR_ITEM(0, 8, 42, 0), METASPR_TERM };
const metasprite_t ship_boom1[]             = { METASPR_ITEM(0, -8, 44, 0), METASPR_ITEM(0, 8, 46, 0), METASPR_TERM };
const metasprite_t ship_boom2[]             = { METASPR_ITEM(0, -8, 48, 0), METASPR_ITEM(0, 8, 51, 0), METASPR_TERM };

const metasprite_t* const ship_meta[13] = {
    ship_normal, ship_up1, ship_up2, ship_up3, ship_up4,
    ship_retournement1, ship_retournement2, ship_retournement3, ship_retournement4, ship_retournement5, ship_retournement6,
    ship_boom1, ship_boom2
};

const uint8_t anim_inclinaison[20] = {0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4};
const uint8_t anim_retournement[24] = {5,5,5,5,6,6,6,6,7,7,7,7,8,8,8,8,9,9,9,9,10,10,10,10};
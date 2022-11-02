#include <stdint.h>
#include <gbdk/platform.h>
#include <gbdk/metasprites.h>

#include "Ship_16x16.h"

const metasprite_t ship_normal[] = {
    METASPR_ITEM(0, -8, 0, 0), METASPR_ITEM(0, 8, 2, 0),
    METASPR_TERM
};

const metasprite_t ship_up1[] = {
    METASPR_ITEM(0, -8, 4, 0), METASPR_ITEM(0, 8, 6, 0),
    METASPR_TERM
};

const metasprite_t ship_up2[] = {
    METASPR_ITEM(0, -8, 8, 0), METASPR_ITEM(0, 8, 10, 0),
    METASPR_TERM
};

const metasprite_t ship_up3[] = {
    METASPR_ITEM(0, -8, 12, 0), METASPR_ITEM(0, 8, 14, 0),
    METASPR_TERM
};

const metasprite_t ship_up4[] = {
    METASPR_ITEM(0, -8, 16, 0), METASPR_ITEM(0, 8, 18, 0),
    METASPR_TERM
};

const metasprite_t* const ship_meta[5] = {
    ship_normal, ship_up1, ship_up2, ship_up3, ship_up4
};
#ifndef setSprite_H
#define setSprite_H

#include <stdint.h>
#include <gbdk/platform.h>
#include <gbdk/metasprites.h>

#include "Ship_16x16.h"

BANKREF_EXTERN(sprite)

extern const metasprite_t* const ship_meta[5];
extern const metasprite_t* const ship_retournement[6];

#endif
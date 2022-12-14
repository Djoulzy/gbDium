#ifndef setSprite_H
#define setSprite_H

#include <stdint.h>
#include <gbdk/platform.h>
#include <gbdk/metasprites.h>

#include "Ship_16x16.h"

BANKREF_EXTERN(sprite)

extern const metasprite_t* const ship_meta[];
extern const uint8_t anim_inclinaison[];
extern const uint8_t anim_retournement[];

#endif
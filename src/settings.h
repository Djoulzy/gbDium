#ifndef SETTINGS_H
#define SETTINGS_H

#include "../res/scene1.h"

#define VIEWPORT_WIDTH          160u
#define VIEWPORT_HEIGHT         144u
#define START_SCROLL_X          40u
#define START_SCROLL_Y          72u
#define SCREEN_SCALE            4u
#define SCENE_BORDER_SIZE       16u
#define SCENE_WIDTH             (Scene1Width * 8u)
#define SCENE_HEIGHT            (Scene1Height * 8u)
#define CAMERA_MAX_X            (SCENE_WIDTH - VIEWPORT_WIDTH)
#define CAMERA_MAX_Y            (SCENE_HEIGHT - VIEWPORT_HEIGHT)

#define GAME_FREE               0u
#define GAME_BOSS               1u
#define CAMERA_PAN_STEP         3u
#define MAX_SHOOT_NUM           3u
#define NB_ALIENS               5u
#define NB_SHIPGUN              3u
#define ENTITY_SPRITES          2u
#define PLAYER_SPRITE           0u
#define PLAYER_BULLET_SPRITE     ENTITY_SPRITES
#define ALIEN_SPRITE             (PLAYER_BULLET_SPRITE + MAX_SHOOT_NUM)
#define ALIEN_BULLET_SPRITE      (ALIEN_SPRITE + NB_ALIENS * ENTITY_SPRITES)
#define SHIPGUN_SPRITE           (ALIEN_BULLET_SPRITE + NB_ALIENS)
#define SHIPGUN_BULLET_SPRITE    (SHIPGUN_SPRITE + NB_SHIPGUN * ENTITY_SPRITES)
#define USED_SPRITES             (SHIPGUN_BULLET_SPRITE + NB_SHIPGUN)
#define ALIEN_TILE               54u
#define SHIPGUN_TILE             72u
#define SHIPGUN_BULLET_TILE      76u

#if USED_SPRITES > 40
#error Too many hardware sprites: the Game Boy has 40 OAM entries.
#endif
#if CAMERA_MAX_Y > 255
#error Vertical camera coordinates must fit in uint8_t.
#endif
#if (SCENE_WIDTH << SCREEN_SCALE) > 32767 || (SCENE_HEIGHT << SCREEN_SCALE) > 32767
#error Fixed-point world coordinates must fit in int16_t.
#endif

#endif

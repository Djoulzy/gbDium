#ifndef def_H
#define def_H

typedef struct Shoot Shoot_t;
struct Shoot {
    uint8_t active;
    int8_t spdx, spdy;
    int16_t dirx, diry;
    int16_t x,y;
};

typedef struct Aliens Aliens_t;
struct Aliens {
    uint8_t active;
    int8_t spdx, spdy;
    int16_t x,y;
    uint8_t animStep;
    Shoot_t shoot;
};

#endif
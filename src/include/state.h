#ifndef INFEX_STATE_H
#define INFEX_STATE_H

#include "world.h"

enum INFEX_SCREEN {
    INFEX_SCREEN_NONE,
    INFEX_SCREEN_TITLE,
    INFEX_SCREEN_GAME,
};

struct Geometry {
    unsigned int screen_width_px, screen_height_px;
    unsigned int hex_radius_px, hex_delta_width_px, hex_delta_height_px;
};

struct State {
    enum INFEX_SCREEN screen_curr;
    struct Geometry geometry;
    struct World *world;
};

void state_update(struct State *state);

#endif

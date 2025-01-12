#ifndef INFEX_STATE_H
#define INFEX_STATE_H

#include "infex.h"

struct State {
    enum INFEX_SCREEN screen_curr;
    unsigned int screen_width_px, screen_height_px;
};

void state_update(struct State *state);

#endif

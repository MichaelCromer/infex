#include <raylib.h>

#include "include/state.h"

void state_update(struct State *state)
{
    switch (state->screen_curr) {
        case INFEX_SCREEN_TITLE:
            if (IsKeyPressed(KEY_ENTER)) {
                world_initialise(10, 10);
                state->screen_curr = INFEX_SCREEN_GAME;
            }
            break;
        case INFEX_SCREEN_GAME:
            if (IsKeyPressed(KEY_ENTER)) {
                state->screen_curr = INFEX_SCREEN_TITLE;
            }
            break;
        default:
            break;
    }
}

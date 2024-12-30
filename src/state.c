#include "include/camera.h"
#include "include/state.h"
#include "include/world.h"

void state_update(struct State *state)
{
    switch (state->screen_curr) {
        case INFEX_SCREEN_TITLE:
            if (IsKeyPressed(KEY_ENTER)) {
                world_initialise(10, 10);
                camera_offset((Vector2){state->screen_width_px/2.0f, state->screen_height_px/2.0f});
                state->screen_curr = INFEX_SCREEN_GAME;
            }
            break;
        case INFEX_SCREEN_GAME:
            if (IsKeyPressed(KEY_ENTER)) {
                state->screen_curr = INFEX_SCREEN_TITLE;
            }

            if (IsKeyDown(KEY_W)) {
                camera_pan((Vector2){ 0.0f, -1.0f });
            }

            if (IsKeyDown(KEY_A)) {
                camera_pan((Vector2){ -1.0f, 0.0f });
            }

            if (IsKeyDown(KEY_S)) {
                camera_pan((Vector2){ 0.0f, 1.0f });
            }

            if (IsKeyDown(KEY_D)) {
                camera_pan((Vector2){ 1.0f, 0.0f });
            }

            break;
        default:
            break;
    }
}

#include <raylib.h>

#include "include/draw.h"

int main(void)
{
    struct State state = {
        .screen_curr = INFEX_SCREEN_TITLE,
        .geometry = {
            .screen_width_px = 800,
            .screen_height_px = 450,
            .hex_delta_height_px = 20,
            .hex_delta_width_px = 20,
        },
        .world = NULL,
    };

    InitWindow(state.geometry.screen_width_px, state.geometry.screen_height_px, "Infex");

    while (!WindowShouldClose()) {
        draw_screen(&state);
        state_update(&state);
    }

    CloseWindow();
    return 0;
}

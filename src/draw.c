#include <raylib.h>

#include "include/draw.h"


void draw_terrain_tile(int x, int y, uint8_t h)
{
    DrawRectangle(x-5, y-5, x+5, y+5, (h == 0) ? BLUE : GREEN );
}


void draw_map(struct State *state)
{
    struct World *world = state->world;

    if (!world) {
        return;
    }

    int x = 0, y = 0, i = 0;
    for (size_t r = 0; r < world->rows; r++) {
        for (size_t c = 0; c < world->cols; c++) {
            draw_terrain_tile(x, y, world->height[i]);
            x += state->geometry.hex_delta_width_px;
            i++;
        }
        y += state->geometry.hex_delta_height_px;
    }
}

void draw_screen_title(struct State *state)
{
    unsigned int screen_width = state->geometry.screen_width_px,
                 screen_height = state->geometry.screen_height_px;
    DrawRectangle(0, 0, screen_width, screen_height, GREEN);
    DrawText("Title Screen", 20, 20, 40, DARKGREEN);
}

void draw_screen_game(struct State *state)
{
    unsigned int screen_width = state->geometry.screen_width_px,
                 screen_height = state->geometry.screen_height_px;
    DrawRectangle(0, 0, screen_width, screen_height, PURPLE);
    DrawText("In-Game", 20, 20, 40, MAROON);

    draw_map(state);
}

void draw_screen_none(struct State *state)
{
    unsigned int screen_width = state->geometry.screen_width_px,
                 screen_height = state->geometry.screen_height_px;
    DrawRectangle(0, 0, screen_width, screen_height, LIGHTGRAY);
    DrawText("NULL Screen", 20, 20, 40, GRAY);
}

void draw_screen(struct State *state)
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

    switch (state->screen_curr) {
        case INFEX_SCREEN_TITLE:
            draw_screen_title(state);
            break;
        case INFEX_SCREEN_GAME:
            draw_screen_game(state);
            break;
        default:
            draw_screen_none(state);
            break;
    }

    EndDrawing();
}


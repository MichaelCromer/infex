#include <raylib.h>

#include "include/draw.h"

void draw_map(struct State *state)
{
    struct World *world = state->world;

    if (!world) {
        return;
    }

    for (size_t i = 0; i < world->rows * world->cols; i++) {
        DrawRectangleV(world->centre[i], (Vector2) {
                       world->centre[i].x + 10, world->centre[i].y + 10},
                       ((world->height[i] == 0) ? BLUE : GREEN));
    }
}

void draw_screen_title(struct State *state)
{
    unsigned int screen_width = state->screen_width_px,
        screen_height = state->screen_height_px;
    DrawRectangle(0, 0, screen_width, screen_height, GREEN);
    DrawText("Title Screen", 20, 20, 40, DARKGREEN);
}

void draw_screen_game(struct State *state)
{
    unsigned int screen_width = state->screen_width_px,
        screen_height = state->screen_height_px;
    DrawRectangle(0, 0, screen_width, screen_height, PURPLE);
    DrawText("In-Game", 20, 20, 40, MAROON);

    draw_map(state);
}

void draw_screen_none(struct State *state)
{
    unsigned int screen_width = state->screen_width_px,
        screen_height = state->screen_height_px;
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

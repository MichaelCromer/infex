#include "include/camera.h"
#include "include/draw.h"
#include "include/world.h"

void draw_map(void)
{
    Vector2 *face = world_faces();
    uint8_t *height = world_heights();
    float scale = world_scale();
    for (size_t i = 0; i < world_num_tiles(); i++) {
        DrawPoly(*face, 6, scale, 30.0f, (*height == 0) ? BLUE : GREEN);
        face++;
        height++;
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

    BeginMode2D(*camera_state());

    draw_map();

    EndMode2D();
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

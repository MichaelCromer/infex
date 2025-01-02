#include "include/camera.h"
#include "include/draw.h"
#include "include/world.h"

#if INFEX_DEBUG == 1
#include <stdio.h>
#endif

void draw_map(void)
{
    Vector2 *faces = world_faces();
    Color *colours = world_colours();
    float scale = world_scale();
#if INFEX_DEBUG == 1
    Vector2 *vertices = world_vertices();
    char tmp[128] = { 0 };
#endif
    for (size_t i = 0; i < world_num_faces(); i++) {
        DrawPoly(faces[i], 6, scale, 30.0f, colours[i]);
#if INFEX_DEBUG == 1
        Vector2 pos = faces[i];
        snprintf(tmp, 128, "(%lu, %lu)", world_row(i), world_col(i));
        DrawText(tmp, pos.x-20, pos.y-10, 12, RED);
#endif
    }

#if INFEX_DEBUG == 1
    for (size_t j = 0; j < world_num_vertices(); j++) {
        DrawCircleV(vertices[j], 3, BLACK);
    }
#endif
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

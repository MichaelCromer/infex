#include "include/draw.h"
#include "include/infex.h"

#if INFEX_DEBUG == 1
#include <stdio.h>
#endif

void draw_enemy(void)
{
    float *enemy = enemy_state();
    Vector2 *faces = world_faces();
    float scale = world_scale();
    for (size_t i = 0; i < grid_size(); i++) {
        if (FloatEquals(enemy[i], 0.0f)) {
            continue;
        }
        Color colour = (Color) { 255, 0, 0, (int) (255 * (enemy[i] / MAX_H)) };
        colour = RED;
        DrawPoly(faces[i], 6, scale - 6, 30.0f, colour);
    }
}

void draw_map(void)
{
    Vector2 *faces = world_faces();
    Color *colours = world_colours();
    float scale = world_scale();

    for (size_t i = 0; i < grid_size(); i++) {
        DrawPoly(faces[i], 6, scale, 30.0f, colours[i]);
    }
}

void draw_world(void)
{
    draw_map();
    draw_enemy();
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

    draw_world();

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

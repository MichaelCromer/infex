#include "hdr/camera.h"
#include "hdr/draw.h"
#include "hdr/infex.h"
#include "hdr/interface.h"
#include "hdr/mouse.h"
#include "hdr/world.h"

#if INFEX_DEBUG == 1
#include <stdio.h>
#endif

void draw_enemy(void)
{
    float *enemy = enemy_state();
    Vector2 *faces = grid_centres();
    float scale = grid_scale();
    for (size_t i = 0; i < grid_size(); i++) {
        if (FloatEquals(enemy[i], 0.0f)) {
            continue;
        }
        Color colour = RED;
        DrawPoly(faces[i], 6, scale - 6, 30.0f, colour);
    }
}

void draw_map(void)
{
    Vector2 *faces = grid_centres();
    Color *colours = map_colours();
    float scale = grid_scale();

    for (size_t i = 0; i < grid_size(); i++) {
        DrawPoly(faces[i], 6, scale, 30.0f, colours[i]);
    }
}

void draw_world(void)
{
    draw_map();
    draw_enemy();
}

void draw_mouse(void)
{
    if (mouse_track_face()) {
        Vector2 *faces = grid_centres();
        float scale = grid_scale();
        DrawPoly(faces[mouse_face()], 6, scale/2, 30.0f, YELLOW);
    }
}

void draw_interface(void)
{
    interface_render();
    draw_mouse();
}

void draw_screen_title(void)
{
    draw_interface();
}

void draw_screen_game(void)
{
    DrawRectangle(0, 0, screen_width(), screen_height(), PURPLE);
    DrawText("In-Game", 20, 20, 40, MAROON);

    BeginMode2D(*camera_state());
    draw_world();
    EndMode2D();

    BeginMode2D((Camera2D) { 0 });
    draw_interface();
    EndMode2D();
}

void draw_screen_none(void)
{
    DrawRectangle(0, 0, screen_width(), screen_height(), LIGHTGRAY);
    DrawText("NULL Screen", 20, 20, 40, GRAY);
}

void draw_screen(void)
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

    switch (screen_curr()) {
        case INFEX_SCREEN_TITLE:
            draw_screen_title();
            break;
        case INFEX_SCREEN_GAME:
            draw_screen_game();
            break;
        default:
            draw_screen_none();
            break;
    }

    EndDrawing();
}

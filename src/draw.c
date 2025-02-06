#include "hdr/camera.h"
#include "hdr/draw.h"
#include "hdr/infex.h"
#include "hdr/interface.h"
#include "hdr/mouse.h"
#include "hdr/world.h"


typedef struct InfexDrawAsset {
    Texture2D texture;
    Rectangle bounds;
    Vector2 centre;
} InfexDrawAsset;


void asset_load(InfexDrawAsset *asset, const char *file_name)
{
    asset->texture = LoadTexture(file_name);
    asset->bounds = (Rectangle) { 0, 0, asset->texture.width, asset->texture.height };
    asset->centre = (Vector2) {
        asset->texture.width / 2.0f,
        asset->texture.height / 2.0f
    };
}


void asset_unload(InfexDrawAsset *asset)
{
    UnloadTexture(asset->texture);
    asset->bounds = (Rectangle) { 0 };
    asset->centre = (Vector2) { 0 };
}


InfexDrawAsset hex_tile = { 0 };
InfexDrawAsset building_test = { 0 };

#define NUM_EDGE_ASSETS 13
InfexDrawAsset hex_edge[NUM_EDGE_ASSETS] = { 0 };

void draw_initialise(void)
{
    asset_load(&hex_tile, "res/img/hex_test5.png");
    asset_load(&building_test, "res/img/building1_test.png");

    asset_load(&(hex_edge[0]), "res/img/hex_edge_0000.png");
    asset_load(&(hex_edge[1]), "res/img/hex_edge_0001.png");
    asset_load(&(hex_edge[2]), "res/img/hex_edge_0010.png");
    asset_load(&(hex_edge[3]), "res/img/hex_edge_0011.png");
    asset_load(&(hex_edge[4]), "res/img/hex_edge_0101.png");
    asset_load(&(hex_edge[5]), "res/img/hex_edge_0110.png");
    asset_load(&(hex_edge[6]), "res/img/hex_edge_0111.png");
    asset_load(&(hex_edge[7]), "res/img/hex_edge_1001.png");
    asset_load(&(hex_edge[8]), "res/img/hex_edge_1010.png");
    asset_load(&(hex_edge[9]), "res/img/hex_edge_1011.png");
    asset_load(&(hex_edge[10]), "res/img/hex_edge_1101.png");
    asset_load(&(hex_edge[11]), "res/img/hex_edge_1110.png");
    asset_load(&(hex_edge[12]), "res/img/hex_edge_1111.png");
}


void draw_terminate(void)
{
    asset_unload(&hex_tile);
    asset_unload(&building_test);

    for (size_t i = 0; i < NUM_EDGE_ASSETS; i++) {
        asset_unload(&(hex_edge[i]));
    }
}


void draw_enemy(void)
{
    float *enemy = enemy_state();
    for (size_t i = 0; i < grid_num_faces(); i++) {
        if (FloatEquals(enemy[i], 0.0f)) {
            continue;
        }
        Color colour = RED;
        DrawPoly(grid_face(i), 6, 20, 30.0f, colour);
    }
}


void draw_map_debug(void)
{
    Vector2 *faces = grid_faces();
    Vector2 *vertices = grid_vertices();

    for (size_t i = 0; i < grid_num_faces(); i++) {
        DrawCircleV(faces[i], 2, RED);
    }

    for (size_t i = 0; i < grid_num_vertices(); i++) {
        DrawCircleV(vertices[i], 2, YELLOW);
    }
}


void draw_map(void)
{
    Vector2 *faces = grid_faces();
    Vector2 *vertices = grid_vertices();
    Color *colours = map_colours();
    uint8_t *slopes = map_slopes();

    for (size_t i = 0; i < grid_num_faces(); i++) {
        DrawTexturePro(
            hex_tile.texture,
            hex_tile.bounds,
            (Rectangle) {
                faces[i].x,
                faces[i].y,
                hex_tile.texture.width,
                hex_tile.texture.height
            },
            hex_tile.centre,
            0,
            colours[i]
        );
    }

    for (size_t i = 0; i < grid_num_vertices(); i++) {
        DrawTexturePro(
            hex_edge[slopes[i]].texture,
            hex_edge[slopes[i]].bounds,
            (Rectangle) {
                vertices[i].x,
                vertices[i].y,
                hex_edge[slopes[i]].texture.width,
                hex_edge[slopes[i]].texture.height
            },
            hex_edge[slopes[i]].centre,
            0,
            WHITE
        );
    }

    if (is_debug()) draw_map_debug();
}


void draw_world(void)
{
    draw_map();
    draw_enemy();
}


void draw_mouse(void)
{
    if (is_building_shadow()) {
        Vector2 pos = grid_face(mouse_face());
        DrawTexturePro(
            building_test.texture,
            building_test.bounds,
            (Rectangle) {
                pos.x,
                pos.y,
                building_test.texture.width,
                building_test.texture.height
            },
            building_test.centre,
            0,
            (Color) { 255, 255, 255, 120 }
        );
    }
}


void draw_interface(void)
{
    interface_render();
}


void draw_screen_title(void)
{
    BeginMode2D(*camera_state());
    draw_world();
    draw_interface();
    EndMode2D();
}


void draw_screen_game(void)
{
    BeginMode2D(*camera_state());
    draw_world();
    draw_mouse();
    EndMode2D();

    BeginMode2D(*camera_default());
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
        case INFEX_SCREEN_MAINMENU:
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

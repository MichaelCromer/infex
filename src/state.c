#include "hdr/camera.h"
#include "hdr/interface.h"
#include "hdr/mouse.h"
#include "hdr/state.h"
#include "hdr/world.h"

#define DEFAULT_SCREEN_WIDTH 800
#define DEFAULT_SCREEN_HEIGHT 450

enum INFEX_SCREEN screen;
unsigned int width = DEFAULT_SCREEN_WIDTH;
unsigned int height = DEFAULT_SCREEN_HEIGHT;

enum INFEX_SCREEN screen_curr(void)
{
    return screen;
}

unsigned int screen_width(void)
{
    return width;
}

unsigned int screen_height(void)
{
    return height;
}

void state_initialise(void)
{
    screen = INFEX_SCREEN_MAINMENU;
    interface_initialise();

    world_initialise(32, 32);
    world_generate();
    camera_set_offset(world_centre());
    camera_goto(world_centre());
}

void state_update(void)
{
    float dt = GetFrameTime();

    switch (screen) {
        case INFEX_SCREEN_MAINMENU:
            if (IsKeyPressed(KEY_ENTER)) {
                world_initialise(32, 32);
                world_generate();
                camera_set_offset(world_centre());
                camera_goto(world_centre());
                screen = INFEX_SCREEN_GAME;
            }
            if (enemy_score() > 33) {
                world_generate();
            }
            break;
        case INFEX_SCREEN_GAME:
            camera_update(dt);
            mouse_update(dt);
            if (IsKeyPressed(KEY_ENTER)) {
                screen = INFEX_SCREEN_MAINMENU;
                world_initialise(32, 32);
                world_generate();
                camera_set_offset(world_centre());
                camera_goto(world_centre());
            }
            break;
        default:
            break;
    }

    world_update(dt);
    interface_update(dt);
}

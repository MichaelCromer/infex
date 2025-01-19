#include "hdr/action.h"
#include "hdr/camera.h"
#include "hdr/interface.h"
#include "hdr/mouse.h"
#include "hdr/state.h"
#include "hdr/world.h"

#define DEFAULT_SCREEN_WIDTH 800
#define DEFAULT_SCREEN_HEIGHT 450

bool quit = false;

enum INFEX_SCREEN screen;
int width = 0;
int height = 0;

bool is_quit(void)
{
    return quit;
}

void set_quit(bool b)
{
    quit = b;
}

enum INFEX_SCREEN screen_curr(void)
{
    return screen;
}

int screen_width(void)
{
    return width;
}

int screen_height(void)
{
    return height;
}

void state_initialise(void)
{
    width = GetScreenWidth();
    height = GetScreenHeight();

    InitWindow(width, height, "Infex");

    screen = INFEX_SCREEN_MAINMENU;
    interface_initialise();

    action_mainmenu_background_initialise();
}

void state_update(void)
{
    float dt = GetFrameTime();

    switch (screen) {
        case INFEX_SCREEN_MAINMENU:
            if (IsKeyPressed(KEY_ENTER)) {
                world_initialise(32, 32);
                world_generate();
                camera_centre();
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
                interface_reset();
                screen = INFEX_SCREEN_MAINMENU;
                action_mainmenu_background_initialise();
            }
            break;
        default:
            break;
    }

    world_update(dt);
    interface_update(dt);
}

void state_terminate(void)
{
    CloseWindow();
}

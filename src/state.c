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
    screen = INFEX_SCREEN_TITLE;
    interface_initialise();
}

void state_update(void)
{
    float dt = GetFrameTime();

    switch (screen) {
        case INFEX_SCREEN_TITLE:
            if (IsKeyPressed(KEY_ENTER)) {
                world_initialise(32, 32);
                world_generate();
                camera_set_offset((Vector2) {
                              width / 2.0f,
                              height / 2.0f}
                );
                screen = INFEX_SCREEN_GAME;
            }
            break;
        case INFEX_SCREEN_GAME:
            if (IsKeyPressed(KEY_ENTER)) {
                screen = INFEX_SCREEN_TITLE;
            }
            break;
        default:
            break;
    }

    camera_update(dt);
    mouse_update(dt);
    world_update(dt);
    interface_update(dt);
}

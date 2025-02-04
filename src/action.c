#include "hdr/camera.h"
#include "hdr/infex.h"
#include "hdr/mouse.h"
#include "hdr/state.h"
#include "hdr/world.h"


void action_quit(void)
{
    set_quit(true);
}


void action_mainmenu_background_initialise(void)
{
    world_initialise(64, 64);
    world_generate();
    camera_centre();
}


void action_start_random_game(void)
{
    world_initialise(32, 32);
    world_generate();
    camera_centre();
    screen_set_curr(INFEX_SCREEN_GAME);
}


void action_building_shadow(enum BUILDING_ID b)
{
    if (b == BUILDING_NONE) {
        mouse_set_track(false);
        set_building_shadow(false);
        return;
    }

    mouse_set_track(true);
    set_building_shadow(true);
}

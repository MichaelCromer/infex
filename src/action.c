#include "hdr/camera.h"
#include "hdr/infex.h"
#include "hdr/interface.h"
#include "hdr/mouse.h"
#include "hdr/state.h"
#include "hdr/world.h"


void action_quit(void) { set_quit(true); }


void action_mainmenu_background_initialise(void)
{
    interface_reset();
    world_initialise(64, 64);
    world_generate();
    camera_centre();
}


void action_start_random_game(void)
{
    interface_reset();
    world_initialise(32, 32);
    world_generate();
    camera_centre();
    screen_set_curr(INFEX_SCREEN_GAME);
}


void action_building_shadow(enum BUILDING_ID b)
{
    set_building_shadow(b);
}


void action_building_place(enum BUILDING_ID b)
{
    switch (building_footprint_type(b)) {
        case FOOTPRINT_ONE:
            building_create(b, mouse_face());
            break;
        case FOOTPRINT_THREE:
            building_create(b, mouse_vert());
            break;
        default:
            break;
    }
}

#include "hdr/action.h"
#include "hdr/camera.h"
#include "hdr/infex.h"
#include "hdr/interface.h"
#include "hdr/mouse.h"
#include "hdr/state.h"
#include "hdr/world.h"


static struct
{
    bool quit;
    bool debug;
    enum INFEX_SCREEN screen;
} state = {
    .quit = false,
    .debug = false,
    .screen = INFEX_SCREEN_MAINMENU
};


struct {
    enum BUILDING_ID id;
    enum FOOTPRINT_TYPE footprint;
    bool is_track;
    size_t (*get_index)(void);
} b_shadow = { 0 };


bool state_is_quit(void) { return state.quit; }
void state_set_quit(bool b) { state.quit = b; }
bool state_is_debug(void) { return state.debug; }
void state_set_debug(bool b) { state.debug = b; }

/*
 *  Building shadow handlers
 */
bool is_building_shadow(void) { return b_shadow.is_track; }
void set_building_shadow(bool b) { b_shadow.is_track = b; }
enum BUILDING_ID building_shadow_id(void) { return b_shadow.id; }
enum FOOTPRINT_TYPE building_shadow_footprint(void) { return b_shadow.footprint; }
void set_building_shadow_index_fn(size_t (*fn)(void)) { b_shadow.get_index = fn; }

void set_building_shadow_id(enum BUILDING_ID b)
{
    b_shadow.id = b;
    b_shadow.footprint = building_footprint_type(b);
}

Vector2 building_shadow_pos(void)
{
    if (!b_shadow.get_index) return Vector2Zero();
    switch (b_shadow.footprint) {
        case FOOTPRINT_ONE:
        case FOOTPRINT_SIX:
            return grid_face(b_shadow.get_index());
        case FOOTPRINT_THREE:
            return grid_vert(b_shadow.get_index());
        default:
            return Vector2Zero();
    }
}

void building_shadow_reset(void)
{
    b_shadow.id = BUILDING_NONE;
    b_shadow.is_track = false;
    b_shadow.get_index = NULL;
}


enum INFEX_SCREEN state_screen(void) { return state.screen; }
void state_set_screen(enum INFEX_SCREEN s) { state.screen = s; }


void state_initialise()
{
    //action_mainmenu_background_initialise();
}


void state_update(void)
{
    float dt = GetFrameTime();

    if (IsKeyPressed(KEY_GRAVE)) {
        state_set_debug(!state_is_debug());
    }

    switch (state.screen) {

        case INFEX_SCREEN_MAINMENU:
            if (IsKeyPressed(KEY_ENTER)) {
                action_start_random_game();
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
                state.screen = INFEX_SCREEN_MAINMENU;
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
    return;
}

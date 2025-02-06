#include <math.h>

#include "hdr/action.h"
#include "hdr/camera.h"
#include "hdr/infex.h"
#include "hdr/state.h"
#include "hdr/world.h"

Vector2 mouse = { 0 };
size_t face = 0;
size_t vert = 0;

Vector2 mouse_state(void) { return mouse; }
size_t mouse_face(void) { return face; }
size_t mouse_vert(void) { return vert; }

void mouse_track(void)
{
    float u = (mouse.x / DELTA_COL), v = (mouse.y / DELTA_ROW);
    float R = (grid_num_rows() - 1), C = (grid_num_cols() - 1);

    size_t r_vert = (size_t) round(Clamp(v - 0.5f, 0.0f, R - 1));
    size_t c_vert = (size_t) round(Clamp(2*u - 1.0f, 0.0f, 2*C));

    size_t r_face = (size_t) round(Clamp(v, 0.0f, R));
    size_t c_face = (size_t) round(Clamp(u - ((r_face % 2) ? 0.5f : 0.0f), 0.0f, C));

    face = grid_face_index(r_face, c_face);
    vert = grid_vert_index(r_vert, c_vert);
}

void mouse_update(float dt)
{
    (void)dt;

    mouse = Vector2Add(GetMousePosition(), camera_position());
    mouse_track();

    if (IsMouseButtonPressed(0)) {
        action_building_place(building_shadow());
    } else if (IsMouseButtonPressed(1)) {
        action_building_shadow(BUILDING_NONE);
    }
}

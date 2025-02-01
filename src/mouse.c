#include <math.h>

#include "hdr/camera.h"
#include "hdr/infex.h"
#include "hdr/world.h"

Vector2 mouse = { 0 };
size_t closest_face = 0;
size_t closest_vert = 0;
bool track = false;

Vector2 mouse_state(void) { return mouse; }
size_t mouse_face(void) { return closest_face; }
size_t mouse_vert(void) { return closest_vert; }
bool mouse_is_track(void) { return track; }
void mouse_set_track(bool b) { track = b; }

void mouse_track(void)
{
    float u = (mouse.x / grid_delta_col()), v = (mouse.y / grid_delta_row());
    float R = (grid_num_rows() - 1), C = (grid_num_cols() - 1);

    size_t r_vert = (size_t) round(Clamp(v - 0.5f, 0.0f, R - 1));
    size_t c_vert = (size_t) round(Clamp(2*u - 1.0f, 0.0f, 2*C));

    size_t r_face = (size_t) round(Clamp(v, 0.0f, R));
    size_t c_face = (size_t) round(Clamp(u - ((r_face % 2) ? 0.5f : 0.0f), 0.0f, C));

    closest_face = grid_face_index(r_face, c_face);
    closest_vert = grid_vert_index(r_vert, c_vert);
}

void mouse_update(float dt)
{
    (void)dt;

    mouse = Vector2Add(GetMousePosition(), camera_position());

    if (track) mouse_track();
}

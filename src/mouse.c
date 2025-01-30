#include <math.h>

#include "hdr/camera.h"
#include "hdr/infex.h"
#include "hdr/world.h"

Vector2 mouse = { 0 };
size_t closest_face = 0;
size_t closest_vertex = 0;

bool track_face = true;

Vector2 mouse_state(void) { return mouse; }
size_t mouse_face(void) { return closest_face; }
size_t mouse_vertex(void) { return closest_vertex; }
bool mouse_is_track_face(void) { return track_face; }
void mouse_set_track_face(bool track) { track_face = track; }

void mouse_track_face(void)
{
    size_t row = (size_t) round(
        Clamp( mouse.y / grid_delta_row(), 0.0f, (float) grid_num_rows() - 1)
    );

    bool odd_row = (row % 2);
    float dc = grid_delta_col();
    size_t col = (size_t) round(
        Clamp(
            (mouse.x - ((odd_row) ? (dc / 2.0f) : 0.0f)) / dc,
            0.0f,
            (float) grid_num_cols() - 1
        )
    );

    closest_face = grid_index(row, col);
}

void mouse_update(float dt)
{
    (void)dt;

    mouse = Vector2Add(GetMousePosition(), camera_position());

    if (track_face) mouse_track_face();
}

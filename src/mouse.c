#include "hdr/camera.h"
#include "hdr/infex.h"
#include "hdr/world.h"

#define MOUSE_FACE_THRESHOLD (0.67f)

Vector2 mouse = { 0 };
size_t closest_face = 0;
size_t closest_vertex = 0;

bool track_face = false;

Vector2 mouse_state(void)
{
    return mouse;
}

/* return which tile face centre the mouse is closest to */
size_t mouse_face(void)
{
    return closest_face;
}

size_t mouse_vertex(void)
{
    return closest_vertex;
}

bool mouse_track_face(void)
{
    return track_face;
}

void mouse_set_track_face(bool track)
{
    track_face = track;
}

void mouse_update(float dt)
{
    (void)dt;

    mouse = Vector2Add(GetMousePosition(), camera_position());

    if (track_face) {
        Vector2 *faces = grid_faces();
        for (size_t i = 0; i < grid_size(); i++) {
            if (Vector2Distance(mouse, faces[i]) < MOUSE_FACE_THRESHOLD * grid_scale()) {
                closest_face = i;
                break;
            }
        }
    }
}

#include "include/camera.h"
#include "include/infex.h"
#include "include/world.h"

Vector2 mouse = { 0 };
size_t closest_face = 0;
size_t closest_vertex = 0;

/* return which tile face centre the mouse is closest to */
size_t mouse_face(void)
{
    return closest_face;
}

size_t mouse_vertex(void)
{
    return closest_vertex;
}

void mouse_update(float dt)
{
    (void)dt;

    mouse = Vector2Add(GetMousePosition(), camera_position());

    Vector2 *faces = world_faces();
    for (size_t i = 0; i < world_num_faces(); i++) {
        if (Vector2Distance(mouse, faces[i]) < 0.67f * world_scale()) {
            closest_face = i;
            break;
        }
    }
}

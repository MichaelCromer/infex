#ifndef INFEX_MOUSE_H
#define INFEX_MOUSE_H

#include <stddef.h>

Vector2 mouse_state(void);
bool mouse_is_track_face(void);
size_t mouse_face(void);
size_t mouse_vertex(void);
void mouse_update(float dt);

#endif

#ifndef INFEX_MOUSE_H
#define INFEX_MOUSE_H

#include <stddef.h>

Vector2 mouse_state(void);
bool mouse_is_track(void);
void mouse_set_track(bool b);
size_t mouse_face(void);
size_t mouse_vert(void);
void mouse_update(float dt);

#endif

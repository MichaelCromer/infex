#ifndef INFEX_CAMERA_H
#define INFEX_CAMERA_H

#include "infex.h"

const Camera2D *camera_state();
Vector2 camera_position(void);
void camera_set_offset(Vector2 offset);
void camera_goto(Vector2 target);
void camera_centre(void);
void camera_pan(Vector2 dir);
void camera_update(float dt);

#endif

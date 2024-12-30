#ifndef INFEX_CAMERA_H
#define INFEX_CAMERA_H

#include "infex.h"

Camera2D camera_state();
void camera_offset(Vector2 offset);
void camera_goto(Vector2 target);
void camera_pan(Vector2 dir);

#endif

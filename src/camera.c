#include "include/camera.h"
#include "include/world.h"

#define PAN_SPEED (10.0f)

Camera2D camera = { 
    .target =  { 0.0f, 0.0f },
    .offset =  { 0.0f, 0.0f },
    .rotation = 0.0f,
    .zoom = 1.0f,
};

Camera2D camera_state()
{
    return camera;
}

void camera_offset(Vector2 offset)
{
    camera.offset = offset;
}

void camera_goto(Vector2 target)
{
    camera.target = target;
}

void camera_pan(Vector2 dir)
{
    camera.target = Vector2Add(camera.target, Vector2Scale(dir, PAN_SPEED));
}

#include "include/camera.h"
#include "include/world.h"

#define PAN_FACTOR_INERTIA (128.0f)
#define PAN_FACTOR_DRAG (0.9f)

Camera2D camera = {
    .target = {0.0f, 0.0f},
    .offset = {0.0f, 0.0f},
    .rotation = 0.0f,
    .zoom = 1.0f,
};

Vector2 pan_velocity = { 0.0f, 0.0f };

const Camera2D *camera_state()
{
    return (const Camera2D *)&camera;
}

void camera_offset(Vector2 offset)
{
    camera.offset = offset;
}

void camera_goto(Vector2 target)
{
    camera.target = target;
}

void camera_pan(Vector2 delta)
{
    pan_velocity = Vector2Add(pan_velocity, Vector2Scale(delta, PAN_FACTOR_INERTIA));
}

void camera_update(float dt)
{
    camera.target = Vector2Add(camera.target, Vector2Scale(pan_velocity, dt));
    camera.target = Vector2Clamp(camera.target, Vector2Zero(), world_bounds());
    pan_velocity = Vector2Scale(pan_velocity, PAN_FACTOR_DRAG);
}

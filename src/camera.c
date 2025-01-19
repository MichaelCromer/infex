#include "hdr/infex.h"
#include "hdr/state.h"
#include "hdr/world.h"

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

void camera_set_offset(Vector2 offset)
{
    camera.offset = offset;
}

Vector2 camera_position(void)
{
    return Vector2Subtract(camera.target, camera.offset);
}

void camera_goto(Vector2 target)
{
    camera.target = target;
}

void camera_pan(Vector2 delta)
{
    pan_velocity = Vector2Add(pan_velocity, Vector2Scale(delta, PAN_FACTOR_INERTIA));
}

void camera_centre(void)
{
    Vector2 mid = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
    camera_set_offset(mid);
    camera_goto(mid);
}

void camera_update(float dt)
{
    if (IsKeyDown(KEY_W)) {
        camera_pan((Vector2) { 0.0f, -1.0f});
    }

    if (IsKeyDown(KEY_A)) {
        camera_pan((Vector2) { -1.0f, 0.0f});
    }

    if (IsKeyDown(KEY_S)) {
        camera_pan((Vector2) { 0.0f, 1.0f});
    }

    if (IsKeyDown(KEY_D)) {
        camera_pan((Vector2) { 1.0f, 0.0f});
    }

    camera.target = Vector2Add(camera.target, Vector2Scale(pan_velocity, dt));
    camera.target = Vector2Clamp(
        camera.target,
        world_bounds_lower(),
        world_bounds_upper()
    );
    pan_velocity = Vector2Scale(pan_velocity, PAN_FACTOR_DRAG);
}

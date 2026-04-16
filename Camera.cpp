#include "Camera.h"

Camera::Camera()
{
    pos = Vec2(0, 0);
    zoom = 1.0f;
}

Vec2 Camera::worldToScreen(Vec2 world)
{
    Vec2 r;
    r.x = (world.x - pos.x) * zoom;
    r.y = (world.y - pos.y) * zoom;
    return r;
}

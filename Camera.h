#pragma once
#include "Vec2.h"

class Camera
{
public:
    Vec2 pos;
    float zoom;

    Camera();

    Vec2 worldToScreen(Vec2 world);
};

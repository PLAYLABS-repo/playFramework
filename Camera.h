#pragma once
#include "Vec2.h"

class Camera
{
public:
    Vec2 position = {0.0f, 0.0f};
    float zoom = 1.0f;

    void apply(int screenWidth, int screenHeight);
};

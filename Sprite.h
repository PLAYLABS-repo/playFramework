#pragma once
#include <string>
#include "Vec2.h"

class Image;
class Atlas;
class Camera;

class Sprite
{
public:
    Image* image = nullptr;
    Atlas* atlas = nullptr;

    std::string frameName;

    Vec2 position;
    Vec2 scale = {1.0f, 1.0f};

    float rotation = 0.0f;
    float skewX = 0.0f;
    float skewY = 0.0f;

    float alpha = 1.0f; // ✅ transparency (0 = invisible, 1 = solid)

    void draw(Camera& cam);
};

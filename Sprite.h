#pragma once

#include <string>
#include "Vec2.h"
#include "Image.h"
#include "Atlas.h"
#include "Camera.h"

class Sprite
{
public:
    Vec2 position;
    Vec2 scale;

    float rotation;

    float skewX;
    float skewY;

    float alpha;

    Image* image;
    Atlas* atlas;

    std::string frameName;

    Sprite();
    ~Sprite();

    void draw(Camera& cam);
};

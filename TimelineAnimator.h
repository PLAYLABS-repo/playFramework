#pragma once
#include <vector>
#include <string>

class Image;
class Atlas;
class Camera;

struct AnimElement
{
    std::string frame;

    float x, y;
    float rotation;
    float sx, sy;

    float pivotX, pivotY;
};

struct AnimFrame
{
    std::vector<AnimElement> elements;
    bool isExplicit = false;
};

class TimelineAnimator
{
public:
    bool load(const char* path, const std::string& symbolName);

    void updateAndDraw(float dt, Image* img, Atlas* atlas, Camera& cam);

private:
    std::vector<AnimFrame> frames;

    float time = 0.0f;
    float fps = 24.0f;
};

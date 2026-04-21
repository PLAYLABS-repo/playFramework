#pragma once
#include <vector>
#include <string>
#include <map>

class Image;
class Atlas;
class Camera;

struct AnimElement
{
    std::string frame;    // atlas key e.g. "0005"
    float x, y;          // world position of pivot (DecomposedMatrix Position)
    float rotation;       // degrees (DecomposedMatrix Rotation z)
    float sx, sy;         // scale (DecomposedMatrix Scaling)
    float pivotX, pivotY; // unscaled pixel offset from sprite top-left (transformationPoint)
};

struct AnimFrame
{
    std::vector<AnimElement> elements;
    std::string label;        // named frame label e.g. "main", "emote"
    bool isExplicit = false;
};

class TimelineAnimator
{
public:
    bool load(const char* path, const std::string& symbolName);
    void playAnimation(const std::string& label);
    void updateAndDraw(float dt, Image* img, Atlas* atlas, Camera& cam);

private:
    std::vector<AnimFrame>   frames;
    std::map<std::string, int> labels;  // label name -> frame index
    int   currentFrame = 0;
    float time         = 0.0f;
    float fps          = 24.0f;
};

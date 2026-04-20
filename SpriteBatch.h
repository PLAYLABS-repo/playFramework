#pragma once
#include <vector>
#include <GL/gl.h>

class Image;
class Atlas;
class Camera;

struct SpriteVertex
{
    float x, y;
    float u, v;
    float r, g, b, a;
};

struct SpriteDrawCommand
{
    Image* image;
    float x, y;
    float w, h;
    float rotation;
    float skewX, skewY;
    float alpha;

    float u1, v1, u2, v2;
};

class SpriteBatch
{
public:
    void begin();
    void submit(const SpriteDrawCommand& cmd);
    void end(Camera& cam);

private:
    std::vector<SpriteDrawCommand> commands;
};

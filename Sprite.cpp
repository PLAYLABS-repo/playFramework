#include "Sprite.h"
#include <GL/gl.h>
#include <math.h>

Sprite::Sprite()
{
    position = Vec2(0, 0);
    scale = Vec2(1, 1);

    rotation = 0.0f;

    skewX = 0.0f;
    skewY = 0.0f;

    alpha = 1.0f;

    image = nullptr;
    atlas = nullptr;

    frameName = "0000";
}

Sprite::~Sprite()
{
}

void Sprite::draw(Camera& cam)
{
    if (!image || !atlas) return;

    Frame f = atlas->get(frameName);

    float texW = (float)image->w;
    float texH = (float)image->h;

    float u0 = f.x / texW;
    float v0 = f.y / texH;
    float u1 = (f.x + f.w) / texW;
    float v1 = (f.y + f.h) / texH;

    float w = f.w * scale.x;
    float h = f.h * scale.y;

    float c = cos(rotation);
    float s = sin(rotation);

    // enable alpha
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 1.0f, 1.0f, alpha);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, image->tex);

    glBegin(GL_QUADS);

    auto vtx = [&](float x, float y, float u, float v)
    {
        // skew
        float sx = x + y * skewX;
        float sy = y + x * skewY;

        // rotate
        float rx = sx * c - sy * s;
        float ry = sx * s + sy * c;

        // world
        Vec2 world(position.x + rx, position.y + ry);

        // camera
        Vec2 screen = cam.worldToScreen(world);

        glTexCoord2f(u, v);
        glVertex2f(screen.x, screen.y);
    };

    vtx(0, 0, u0, v0);
    vtx(w, 0, u1, v0);
    vtx(w, h, u1, v1);
    vtx(0, h, u0, v1);

    glEnd();

    // reset color (IMPORTANT)
    glColor4f(1, 1, 1, 1);
}

#include "SpriteBatch.h"
#include "Camera.h"

#include <cmath>

void SpriteBatch::begin()
{
    commands.clear();
}

void SpriteBatch::submit(const SpriteDrawCommand& cmd)
{
    commands.push_back(cmd);
}

static void drawQuad(const SpriteDrawCommand& c)
{
    float hw = c.w * 0.5f;
    float hh = c.h * 0.5f;

    glBindTexture(GL_TEXTURE_2D, c.image->textureID);

    glColor4f(1, 1, 1, c.alpha);

    glPushMatrix();

    glTranslatef(c.x, c.y, 0);
    glRotatef(c.rotation, 0, 0, 1);

    float skew[16] =
    {
        1, c.skewY, 0, 0,
        c.skewX, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    glMultMatrixf(skew);

    glBegin(GL_QUADS);

    glTexCoord2f(c.u1, c.v1); glVertex2f(-hw, -hh);
    glTexCoord2f(c.u2, c.v1); glVertex2f(hw, -hh);
    glTexCoord2f(c.u2, c.v2); glVertex2f(hw, hh);
    glTexCoord2f(c.u1, c.v2); glVertex2f(-hw, hh);

    glEnd();

    glPopMatrix();
}

void SpriteBatch::end(Camera& cam)
{
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (auto& c : commands)
        drawQuad(c);

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}

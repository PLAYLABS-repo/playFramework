#include "Sprite.h"
#include "Image.h"
#include "Atlas.h"
#include "Camera.h"

#include <GL/gl.h>

void Sprite::draw(Camera& cam)
{
    if (!image) return;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, image->textureID);

    // ✅ Alpha
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 1.0f, 1.0f, alpha);

    float x = position.x;
    float y = position.y;

    float w = image->width * scale.x;
    float h = image->height * scale.y;

    float u1 = 0.0f, v1 = 0.0f;
    float u2 = 1.0f, v2 = 1.0f;

    // ✅ ATLAS CUT
    if (atlas && !frameName.empty())
    {
        Frame f = atlas->get(frameName);

        u1 = f.x / (float)image->width;
        v1 = f.y / (float)image->height;
        u2 = (f.x + f.w) / (float)image->width;
        v2 = (f.y + f.h) / (float)image->height;

        w = f.w * scale.x;
        h = f.h * scale.y;
    }

    glPushMatrix();

    // ✅ Correct transform order (NO duplicates)
    glTranslatef(x, y, 0);
    glRotatef(rotation, 0, 0, 1);

    // ✅ Skew
    float skewMatrix[16] = {
        1,        skewY,   0, 0,
        skewX,    1,       0, 0,
        0,        0,       1, 0,
        0,        0,       0, 1
    };
    glMultMatrixf(skewMatrix);

    glBegin(GL_QUADS);

    glTexCoord2f(u1, v1); glVertex2f(-w / 2, -h / 2);
    glTexCoord2f(u2, v1); glVertex2f( w / 2, -h / 2);
    glTexCoord2f(u2, v2); glVertex2f( w / 2,  h / 2);
    glTexCoord2f(u1, v2); glVertex2f(-w / 2,  h / 2);

    glEnd();

    glPopMatrix();

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}

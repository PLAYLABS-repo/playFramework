#include "Sprite.h"
#include "Image.h"
#include "Atlas.h"
#include "Camera.h"

#include <GL/gl.h>
#include <cmath>

// =========================
// MOVE
// =========================
void Sprite::moveTo(float x, float y)
{
    targetPosition = {x, y};
}

void Sprite::scaleTo(float w, float h)
{
    targetScale = {w, h};
}

void Sprite::rotateTo(float degrees)
{
    targetRotation = degrees;
}

// =========================
// UPDATE (LINEAR)
// =========================
void Sprite::update(float dt)
{
    float dx = targetPosition.x - position.x;
    float dy = targetPosition.y - position.y;
    float dist = sqrtf(dx * dx + dy * dy);

    if (dist > 0.001f)
    {
        float step = moveSpeed * dt;

        if (step >= dist)
            position = targetPosition;
        else
        {
            position.x += (dx / dist) * step;
            position.y += (dy / dist) * step;
        }
    }

    float sx = targetScale.x - scale.x;
    float sy = targetScale.y - scale.y;
    float sDist = sqrtf(sx * sx + sy * sy);

    if (sDist > 0.001f)
    {
        float step = scaleSpeed * dt;

        if (step >= sDist)
            scale = targetScale;
        else
        {
            scale.x += (sx / sDist) * step;
            scale.y += (sy / sDist) * step;
        }
    }

    float r = targetRotation - rotation;

    if (fabs(r) > 0.01f)
    {
        float step = rotateSpeed * dt;

        if (step >= fabs(r))
            rotation = targetRotation;
        else
            rotation += (r > 0 ? 1 : -1) * step;
    }
}

// =========================
// AABB
// =========================
AABB Sprite::getAABB() const
{
    if (!image)
        return AABB(position.x, position.y, 0, 0);

    float w = image->width * scale.x;
    float h = image->height * scale.y;

    if (atlas && !frameName.empty())
    {
        Frame f;
        if (atlas->get(frameName, f))
        {
            w = f.w * scale.x;
            h = f.h * scale.y;
        }
    }

    float left = position.x - pivot.x;
    float top  = position.y - pivot.y;

    return AABB(left, top, w, h);
}

// =========================
// DRAW (FIXED ALPHA PIPELINE)
// =========================
void Sprite::draw(Camera& cam)
{
    if (!image) return;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, image->textureID);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // FIXED: alpha always applied cleanly
    float finalAlpha = alpha;
    if (finalAlpha < 0.0f) finalAlpha = 0.0f;
    if (finalAlpha > 1.0f) finalAlpha = 1.0f;

    glColor4f(1.0f, 1.0f, 1.0f, finalAlpha);

    float w = image->width * scale.x;
    float h = image->height * scale.y;

    float u1 = 0, v1 = 0, u2 = 1, v2 = 1;

    if (atlas && !frameName.empty())
    {
        Frame f;
        if (atlas->get(frameName, f))
        {
            u1 = f.x / (float)image->width;
            v1 = f.y / (float)image->height;
            u2 = (f.x + f.w) / (float)image->width;
            v2 = (f.y + f.h) / (float)image->height;

            w = f.w * scale.x;
            h = f.h * scale.y;
        }
    }

    glPushMatrix();

    glTranslatef(position.x, position.y, 0);
    glTranslatef(-pivot.x, -pivot.y, 0);

    glTranslatef(pivot.x, pivot.y, 0);
    glRotatef(rotation, 0, 0, 1);
    glTranslatef(-pivot.x, -pivot.y, 0);

    float skewMatrix[16] =
    {
        1, skewY, 0, 0,
        skewX, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    glMultMatrixf(skewMatrix);

    glBegin(GL_QUADS);

    glTexCoord2f(u1, v1); glVertex2f(0, 0);
    glTexCoord2f(u2, v1); glVertex2f(w, 0);
    glTexCoord2f(u2, v2); glVertex2f(w, h);
    glTexCoord2f(u1, v2); glVertex2f(0, h);

    glEnd();

    glPopMatrix();

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}

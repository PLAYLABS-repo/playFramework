#include "Camera.h"
#include <GL/gl.h>

void Camera::apply(int screenWidth, int screenHeight)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // center-based camera
    float halfW = (float)screenWidth * 0.5f / zoom;
    float halfH = (float)screenHeight * 0.5f / zoom;

    glOrtho(
        position.x - halfW,
        position.x + halfW,
        position.y + halfH,
        position.y - halfH,
        -1.0f,
        1.0f
    );

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

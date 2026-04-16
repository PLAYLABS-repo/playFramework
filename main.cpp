#include "Window.h"
#include "Input.h"
#include "Sprite.h"
#include "Camera.h"
#include "Atlas.h"
#include "Image.h"

#include <GL/gl.h>
#include <windows.h>
#include <stdio.h>

float speed = 0.5f;

int main()
{
    Window window;

    if (!window.create("playFramework", 800, 600))
        return -1;

    // =============================
    // FIX: PROPER 2D RENDER SETUP
    // =============================
    glViewport(0, 0, 800, 600);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 800, 600, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);

    glClearColor(0.33f, 0.33f, 0.33f, 1.0f);

    Camera cam;
    cam.pos = Vec2(0, 0);

    Image texture;
    Atlas atlas;

    if (!texture.load("anim/spritemap.png"))
        return -1;

    if (!atlas.load("anim/spritemap.json"))
        return -1;

    Sprite player;
    player.image = &texture;
    player.atlas = &atlas;
    player.frameName = "0000";

    player.position = Vec2(100, 100);
    player.scale = Vec2(1, 1);
    player.rotation = 0;
    player.rotation = 67.67;

    int frameIndex = 2;
    const int MAX_FRAMES = 1024;

    DWORD lastTime = GetTickCount();
    const DWORD delay = 500;

    while (window.process())
    {
        Input::update(window.getHWND());

        glClear(GL_COLOR_BUFFER_BIT);
        glLoadIdentity();

        // =====================
        // PLAYER MOVEMENT
        // =====================
        if (Input::isKeyDown('A')) player.position.x -= speed;
        if (Input::isKeyDown('D')) player.position.x += speed;
        if (Input::isKeyDown('W')) player.position.y -= speed;
        if (Input::isKeyDown('S')) player.position.y += speed;

        // =====================
        // CAMERA MOVEMENT
        // =====================
        if (Input::isKeyDown(VK_LEFT))  cam.pos.x -= speed;
        if (Input::isKeyDown(VK_RIGHT)) cam.pos.x += speed;
        if (Input::isKeyDown(VK_UP))    cam.pos.y -= speed;
        if (Input::isKeyDown(VK_DOWN))  cam.pos.y += speed;

        // =====================
        // FRAME STEP (100ms)
        // =====================
        DWORD currentTime = GetTickCount();

        if (Input::isKeyDown('X') && (currentTime - lastTime >= delay))
        {
            lastTime = currentTime;

            frameIndex++;

            char buffer[16];
            sprintf(buffer, "%04d", frameIndex);

            player.frameName = buffer;
        }

        if (Input::isKeyDown('Z'))
        {
            frameIndex = 0;
            player.frameName = "0000";
        }

        // =====================
        // SKEW TEST
        // =====================
        player.skewX = (Input::isKeyDown('Q')) ? 0.3f : 0.0f;
        player.skewY = (Input::isKeyDown('E')) ? 0.3f : 0.0f;

        // =====================
        // DRAW
        // =====================
        player.draw(cam);

        SwapBuffers(window.getHDC());
    }

    return 0;
}

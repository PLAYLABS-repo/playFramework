#include "Window.h"
#include "Input.h"
#include "Camera.h"
#include "Image.h"
#include "Atlas.h"
#include "TimelineAnimator.h"
#include "Timer.h"
#include "Sound.h"
#include <iostream>
#include <GL/gl.h>
#include <windows.h>
using namespace std;

int main()
{
    Window window;
    window.create("playFramework", 1280, 720);

    Image texture;
    Atlas atlas;
    texture.load("tests/anim/spritemap.png");
    atlas.load("tests/anim/spritemap.json");

    Anim player("tests/anim/Animation.json");
    player.position = Vec2(50, -90);
    player.size     = Vec2(1.f, 1.0f);
    player.rotation = 0.0f;
    player.anim("PLAYER", "RUN");

    Camera cam;
    cam.position = {0, 0};
    cam.rotation = 0.0f;
    cam.zoom     = 1.232f;

    Sound music;
    if (music.load("test.wav"))
        music.play(true);

    Timer timer;

    while (window.process())
    {
        Input::update(window.getHWND());
        float dt = timer.delta();

        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Camera pan
        float camSpeed = 300.0f * dt;
        if (Input::isKeyDown(VK_LEFT))  cam.position.x -= camSpeed;
        if (Input::isKeyDown(VK_RIGHT)) cam.position.x += camSpeed;
        if (Input::isKeyDown(VK_UP))    cam.position.y -= camSpeed;
        if (Input::isKeyDown(VK_DOWN))  cam.position.y += camSpeed;

        // Player movement
        float speed = 300.0f * dt;
        if (Input::isKeyDown('A')) player.position.x -= speed;
        if (Input::isKeyDown('D')) player.position.x += speed;
        if (Input::isKeyDown('W')) player.position.y -= speed;
        if (Input::isKeyDown('S')) player.position.y += speed;

        // Zoom
        if (Input::isKeyDown('Z')) cam.zoom *= (1.0f + dt * 2.0f);
        if (Input::isKeyDown('X')) cam.zoom /= (1.0f + dt * 2.0f);

        // Switch animations at runtime
        if (Input::isKeyPressed('1')) player.anim("PLAYER", "RUN");
        if (Input::isKeyPressed('2')) player.anim("PLAYER", "IDLE");
        if (Input::isKeyPressed('3')) player.anim("PLAYER", "JUMP");
        if (Input::isKeyPressed('4')) player.anim("PLAYER", "SHOOT");
        if (Input::isKeyPressed('5')) player.anim("PLAYER", "RUNSHOOT");
        if (Input::isKeyPressed('6')) player.anim("PLAYER", "SLIDE");
        if (Input::isKeyPressed('7')) player.anim("PLAYER", "DIE");

        cam.apply(1280, 720);

        player.update(dt);
        player.draw(&texture, &atlas, cam);

        SwapBuffers(window.getHDC());
    }

    return 0;
}

#include "Window.h"
#include "Input.h"
#include "Camera.h"
#include "Image.h"
#include "Atlas.h"
#include "TimelineAnimator.h"
#include "Timer.h"
#include "Sound.h"

#include <GL/gl.h>
#include <windows.h>

int main()
{
    Window window;
    if (!window.create("playFramework", 1280, 720))
        return -1;

    Image texture;
    Atlas atlas;

    if (!texture.load("anim/spritemap.png") ||
        !atlas.load("anim/spritemap.json"))
    {
        MessageBoxA(NULL, "Failed to load texture/atlas", "Error", MB_OK);
        return -1;
    }

    TimelineAnimator timeline;
    if (!timeline.load("anim/Animation.json"))
    {
        MessageBoxA(NULL, "Failed to load animation", "Error", MB_OK);
        return -1;
    }

    // Play a named animation: "ENTITY"_ANIM_"ANIMTYPE"
    // e.g. PLAYER_ANIM_RUN, PLAYER_ANIM_IDLE, PLAYER_ANIM_JUMP, etc.
    timeline.play("ANIMATION", "ALL");
    Camera cam;
    cam.position = {0, 0 };  // centre of screen
    cam.rotation = 0.0f;
    cam.zoom     = 1.0f;

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

        // Zoom
        if (Input::isKeyDown('Z')) cam.zoom *= (1.0f + dt * 2.0f);
        if (Input::isKeyDown('X')) cam.zoom /= (1.0f + dt * 2.0f);

        // Switch animations at runtime: 1-6 keys
        if (Input::isKeyPressed('1')) timeline.play("PLAYER", "RUN");
        if (Input::isKeyPressed('2')) timeline.play("PLAYER", "IDLE");
        if (Input::isKeyPressed('3')) timeline.play("PLAYER", "JUMP");
        if (Input::isKeyPressed('4')) timeline.play("PLAYER", "SHOOT");
        if (Input::isKeyPressed('5')) timeline.play("PLAYER", "RUNSHOOT");
        if (Input::isKeyPressed('6')) timeline.play("PLAYER", "SLIDE");
        if (Input::isKeyPressed('7')) timeline.play("PLAYER", "DIE");

        cam.apply(1280, 720);

        timeline.update(dt);
        timeline.draw(&texture, &atlas, cam);

        SwapBuffers(window.getHDC());
    }

    return 0;
}

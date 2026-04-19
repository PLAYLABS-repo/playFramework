#include "Window.h"
#include "Input.h"
#include "Camera.h"
#include "Image.h"
#include "Atlas.h"
#include "TimelineAnimator.h"
#include "Timer.h"

#include <GL/gl.h>
#include <windows.h>

int main()
{
    // =========================
    // WINDOW
    // =========================
    Window window;
    if (!window.create("playFramework", 1280, 720))
        return -1;

    // =========================
    // GRAPHICS
    // =========================
    Image texture;
    Atlas atlas;

    if (!texture.load("anim/spritemap.png") ||
        !atlas.load("anim/spritemap.json"))
    {
        MessageBox(NULL, "Failed to load texture/atlas", "Error", MB_ICONERROR);
        return -1;
    }

    // =========================
    // TIMELINE
    // =========================
    TimelineAnimator timeline;

    if (!timeline.load("anim/Animation.json", "PLAYER_ANIM_RUN"))
    {
        MessageBox(NULL, "Failed to load animation", "Error", MB_ICONERROR);
        return -1;
    }

    // =========================
    // CAMERA
    // =========================
    Camera cam;
    cam.position = Vec2(0, 0);

    // =========================
    // TIMER
    // =========================
    Timer timer;

    // =========================
    // LOOP
    // =========================
    while (window.process())
    {
        Input::update(window.getHWND());

        float dt = timer.delta();

        glClear(GL_COLOR_BUFFER_BIT);

        // =========================
        // CAMERA CONTROLS (OPTIONAL)
        // =========================
        float camSpeed = 300.0f * dt;

        if (Input::isKeyDown(VK_LEFT))  cam.position.x -= camSpeed;
        if (Input::isKeyDown(VK_RIGHT)) cam.position.x += camSpeed;
        if (Input::isKeyDown(VK_UP))    cam.position.y -= camSpeed;
        if (Input::isKeyDown(VK_DOWN))  cam.position.y += camSpeed;

        // =========================
        // APPLY CAMERA
        // =========================
        cam.apply(1280, 720);

        // =========================
        // 🔥 RUN + DRAW ANIMATION
        // =========================
        timeline.updateAndDraw(dt, &texture, &atlas, cam);

        // =========================
        // PRESENT
        // =========================
        SwapBuffers(window.getHDC());
    }

    return 0;
}

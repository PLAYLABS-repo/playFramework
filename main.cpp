#include "Window.h"
#include "Input.h"
#include "Sprite.h"
#include "Camera.h"
#include "Atlas.h"
#include "Image.h"
#include "Sound.h"
#include "Text.h"
#include "Font.h"

#include <GL/gl.h>
#include <windows.h>

int main()
{
    Window window;
    window.create("playFramework", 800, 600);

    Image texture;
    Atlas atlas;

    texture.load("anim/spritemap.png");
    atlas.load("anim/spritemap.json");


    // ---------- FONT (STB) ----------


    // ---------- SPRITE ----------
    Sprite player;
    player.image = &texture;
    player.atlas = &atlas;
    player.frameName = "0000";
    player.position = Vec2(100, 100);
    player.scale = Vec2(1.0f, 1.0f);
    player.rotation = 0.0f;
    player.alpha = 0.5f;
    player.skewX = 1;

    Camera cam;
    cam.position = Vec2(0, 0);

    // ---------- SOUND ----------
    Sound bgm;
    Sound jump;
    Sound hit;

    bgm.load("test.wav");
    jump.load("test.wav");
    hit.load("shit.wav");

    bgm.play(true);

    while (window.process())
    {
        Input::update(window.getHWND());

        glClear(GL_COLOR_BUFFER_BIT);
        glLoadIdentity();

        float speed = 0.3f;

        if (Input::isKeyDown('A')) player.position.x -= speed;
        if (Input::isKeyDown('D')) player.position.x += speed;
        if (Input::isKeyDown('W')) player.position.y -= speed;
        if (Input::isKeyDown('S')) player.position.y += speed;
        if (Input::isKeyDown('T')) player.alpha += speed;



        // ---------- SOUND TEST ----------
        if (Input::isKeyDown(VK_SPACE))
            jump.play();

        if (Input::isKeyDown('H'))
            hit.play();

        // ---------- RENDER ----------
        player.draw(cam);
;
        SwapBuffers(window.getHDC());
    }

    return 0;
}

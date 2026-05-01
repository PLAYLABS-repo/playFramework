#include "PlaylabsGL.h"
#include <GL/gl.h>

// =============================================================
// HELPERS
// =============================================================

static void drawRect(float x, float y, float w, float h,
                     float r, float g, float b, float a = 1.0f)
{
    glDisable(GL_TEXTURE_2D);
    glColor4f(r, g, b, a);
    glBegin(GL_QUADS);
    glVertex2f(x,     y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x,     y + h);
    glEnd();
    glColor4f(1, 1, 1, 1);
}

// =============================================================
// CAMERA  (top-left origin, pixel-perfect)
// =============================================================
static void applyCamera2D(Camera& cam, int sw, int sh)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float visW = sw / cam.zoom;
    float visH = sh / cam.zoom;

    glOrtho(
        cam.position.x,
        cam.position.x + visW,
        cam.position.y + visH,
        cam.position.y,
        -1.0f, 1.0f
    );

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// =============================================================
// PLAYER
// =============================================================
struct Player
{
    float x     = 200.0f;
    float y     = 436.0f;  // starts at maxY - h
    float w     = 64.0f;
    float h     = 64.0f;
    float speed = 250.0f;

    // Vertical free walk — clamped between these world Y values
    float minY = 0.0f;   // highest the player can walk (smaller Y = higher on screen)
    float maxY = 500.0f;   // lowest the player can walk

    // Jump — arc goes up and comes back to the SAME Y it left from
    float baseY     = 436.0f;  // Y the player jumped from; restored on landing
    float velocityY = 0.0f;
    float gravity   = 900.0f;
    float jumpForce = -520.0f;
    bool  jumping   = false;

    Sprite*           sprite = nullptr;
    TimelineAnimator* anim   = nullptr;

    enum class State { IDLE, RUN, JUMP } state = State::IDLE;
    float facingX = 1.0f;
};

// =============================================================
// ENTRY POINT
// =============================================================
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    Window win;
    if (!win.create("Playlabs GL", 1280, 720))
        return -1;

    Camera cam;
    cam.position = {0.0f, 0.0f};
    cam.zoom     = 1.6f;

    Timer timer;

    // ---------------------------------------------------------
    // Assets
    // ---------------------------------------------------------
    Image* sheet = Playlabs_LoadImage("tests/anim/spritemap.png");
    Atlas* atlas = Playlabs_LoadAtlas("tests/anim/spritemap.json");

    Sound* bgm = Playlabs_CreateSound();
    bgm->load("test.wav");
    bgm->play(true);

    Sound* sfxJump = Playlabs_CreateSound();
    sfxJump->load("assets/jump.mp3");

    // ---------------------------------------------------------
    // Player
    // ---------------------------------------------------------
    Player player;

    player.sprite              = Playlabs_CreateSprite();
    player.sprite->image       = sheet;
    player.sprite->atlas       = atlas;
    player.sprite->frameName   = "0000";
    player.sprite->position      = {player.x, player.y};
    player.sprite->targetPosition = {player.x, player.y};
    player.sprite->scale          = {1.0f, 1.0f};
    player.sprite->targetScale    = {1.0f, 1.0f};

    player.anim = Playlabs_CreateAnimator();
    if (player.anim->load("tests/anim/Animation.json"))
        player.anim->play("PLAYER", "IDLE");

    // ---------------------------------------------------------
    // Background
    // ---------------------------------------------------------
    Sprite* background         = Playlabs_CreateSprite();
    background->frameName      = "bg_layer_00";
    background->position       = {0.0f, 0.0f};
    background->targetPosition = {0.0f, 0.0f};

    // ---------------------------------------------------------
    // GAME LOOP
    // ---------------------------------------------------------
    while (win.process())
    {
        float dt = timer.delta();
        if (dt > 0.05f) dt = 0.05f;

        // -----------------------------------------------------
        // INPUT
        // -----------------------------------------------------
        Playlabs_PollInput(&win);

        bool  moving = false;
        float moveX  = 0.0f;
        float moveY  = 0.0f;

        if (Playlabs_KeyDown(VK_RIGHT) || Playlabs_KeyDown('D')) { moveX =  1.0f; moving = true; }
        if (Playlabs_KeyDown(VK_LEFT)  || Playlabs_KeyDown('A')) { moveX = -1.0f; moving = true; }

        // Vertical walk — only allowed when not jumping
        if (!player.jumping)
        {
            if (Playlabs_KeyDown(VK_UP)   || Playlabs_KeyDown('W')) { moveY = -1.0f; moving = true; }
            if (Playlabs_KeyDown(VK_DOWN) || Playlabs_KeyDown('S')) { moveY =  1.0f; moving = true; }
        }

        // Jump — only when not already jumping
        if (Playlabs_KeyPressed(VK_SPACE) && !player.jumping)
        {
            player.baseY     = player.y;          // remember where we jumped from
            player.velocityY = player.jumpForce;
            player.jumping   = true;
            player.state     = Player::State::JUMP;
            player.anim->play("PLAYER", "JUMP");
            sfxJump->play(false);
        }

        // -----------------------------------------------------
        // MOVEMENT
        // -----------------------------------------------------
        player.x += moveX * player.speed * dt;

        if (!player.jumping)
        {
            // Free vertical walk, clamped to [minY, maxY - h]
            player.y += moveY * player.speed * dt;
            player.y  = (player.y < player.minY)            ? player.minY
                      : (player.y > player.maxY - player.h) ? player.maxY - player.h
                      : player.y;

            // Keep baseY in sync so a jump always leaves from current position
            player.baseY = player.y;
        }
        else
        {
            // Jump arc — gravity pulls back down
            player.velocityY += player.gravity * dt;
            player.y         += player.velocityY * dt;

            // Landed back at (or past) the Y we jumped from
            if (player.y >= player.baseY)
            {
                player.y         = player.baseY;   // snap exactly back
                player.velocityY = 0.0f;
                player.jumping   = false;

                // Transition back to run or idle
                player.state = moving ? Player::State::RUN : Player::State::IDLE;
                player.anim->play("PLAYER", moving ? "RUN" : "IDLE");
            }
        }

        // Facing direction
        if (moveX != 0.0f) player.facingX = moveX > 0 ? -1.0f : 1.0f;

        // Sync sprite
        player.sprite->position      = {player.x, player.y};
        player.sprite->targetPosition = {player.x, player.y};
        player.sprite->scale          = {player.facingX, 1.0f};
        player.sprite->targetScale    = {player.facingX, 1.0f};

        // -----------------------------------------------------
        // ANIMATION STATE  (only when grounded)
        // -----------------------------------------------------
        if (!player.jumping)
        {
            if (moving && player.state != Player::State::RUN)
            {
                player.state = Player::State::RUN;
                player.anim->play("PLAYER", "RUN");
            }
            else if (!moving && player.state != Player::State::IDLE)
            {
                player.state = Player::State::IDLE;
                player.anim->play("PLAYER", "IDLE");
            }
        }

        Playlabs_SetAnimatorParent(player.anim,
            player.x, player.y,
            0.0f,
            player.facingX, 1.0f
        );

        // -----------------------------------------------------
        // CAMERA FOLLOW
        // -----------------------------------------------------
        float targetCamX = player.x - win.getWidth()  * 0.5f + 340.0f;
        float targetCamY = player.y - win.getHeight() * 0.5f + 280.0f;
        cam.position.x  += (targetCamX - cam.position.x) * 5.0f * dt;
        cam.position.y  += (targetCamY - cam.position.y) * 5.0f * dt;

        // -----------------------------------------------------
        // COLLISION — hitbox shrinks while in the air
        // Narrower and half height, anchored to feet
        // -----------------------------------------------------
        float playerHitboxSizeX = 100.0f;
        float playerHitboxSizeY = 200.0f;
        float hitW = player.jumping ? playerHitboxSizeX * 0.6f : playerHitboxSizeX;
        float hitH = player.jumping ? playerHitboxSizeY * 0.5f : playerHitboxSizeY;
        float hitX = player.x + (player.w - 200) * 0.5f;
        float hitY = player.y + (player.h);

        float wX = 100.0f, wY = 100.0f, wW = 100.0f, wH = 100.0f;
        if (Playlabs_AABBIntersects(hitX, hitY, hitW, hitH, wX, wY, wW, wH))
        {
            player.x -= moveX * player.speed * dt;
            player.sprite->position      = {player.x, player.y};
            player.sprite->targetPosition = {player.x, player.y};
        }

        // -----------------------------------------------------
        // RENDER
        // -----------------------------------------------------
        Playlabs_Clear(0.12f, 0.12f, 0.18f, 1.0f);
        applyCamera2D(cam, win.getWidth(), win.getHeight());

        drawRect(0, player.maxY, 2000, 64, 0.25f, 0.20f, 0.15f);  // floor at maxY

        if (!sheet)
            drawRect(player.x, player.y, player.w, player.h, 0.2f, 0.6f, 1.0f);

        background->update(dt);
        background->draw(cam);

        player.sprite->update(dt);
        player.sprite->draw(cam);

        Playlabs_TickAnimator(player.anim, dt, sheet, atlas, &cam);
        drawRect(wX, wY, wW, wH, 1.0f, 0.0f, 0.0f, 0.5f);
        drawRect(hitX, hitY, hitW, hitH, 0.0f, 1.0f, 0.0f, 0.5f);

        Playlabs_Present(&win);
    }

    // ---------------------------------------------------------
    // CLEANUP
    // ---------------------------------------------------------
    Playlabs_DestroyAnimator(player.anim);
    Playlabs_DestroySprite(player.sprite);
    Playlabs_DestroySprite(background);
    Playlabs_DestroySound(sfxJump);
    Playlabs_DestroySound(bgm);
    Playlabs_FreeAtlas(atlas);
    Playlabs_FreeImage(sheet);

    return 0;
}

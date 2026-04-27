#pragma once
#include <string>
#include <vector>
#include <map>
#include "Vec2.h"
#include <GL/gl.h>

class Image;
class Atlas;
class Camera;

// =========================
// STRUCTS
// =========================
struct TA_Element
{
    std::string spriteName;   // ATLAS_SPRITE_instance or bitmap name
    std::string symbolName;   // SYMBOL_Instance name

    Vec2  position  = {0, 0};
    Vec2  bitmapOff = {0, 0}; // bitmap's own local offset inside symbol
    Vec2  scale     = {1, 1};
    float rotation  = 0.0f;   // radians (from DecomposedMatrix.Rotation.z)
    Vec2  pivot     = {0, 0}; // transformationPoint (raw pixels)

    // Graphic symbol playback
    bool  isGraphic  = false;
    int   firstFrame = 0;
    bool  looping    = true;
};

struct TA_Frame
{
    int index    = 0;
    int duration = 1;
    std::vector<TA_Element> elements;
};

struct TA_Layer
{
    std::vector<TA_Frame> frames;
};

struct TA_Timeline
{
    std::vector<TA_Layer> layers;
    int totalFrames = 0;
};

// =========================
// MAIN CLASS
// =========================
class TimelineAnimator
{
public:
    bool load(const char* path);

    // Play a named symbol e.g. play("PLAYER", "RUN") -> plays PLAYER_ANIM_RUN
    void play(const std::string& entity, const std::string& animType);

    void update(float dt);
    void draw(Image* img, Atlas* atlas, Camera& cam);

    int  currentFrame = 0;
    int  totalFrames  = 0;

private:
    std::map<std::string, TA_Timeline> symbols;
    TA_Timeline* activeTimeline = nullptr;

    float frameTimer = 0.0f;
    float fps        = 24.0f;

    void drawTimeline(
        TA_Timeline& timeline,
        Image*       img,
        Atlas*       atlas,
        Vec2         parentPos,
        float        parentRot,  // radians
        Vec2         parentScale,
        int          frame
    );

    void drawSprite(
        const std::string& name,
        Image* img, Atlas* atlas,
        Vec2 pos, float rotRad, Vec2 scale, Vec2 pivot,
        Vec2 bitmapOff
    );
};

// =========================
// ANIM — friendly wrapper around TimelineAnimator
//
// Usage:
//   Anim player("assets/animation.json");
//   player.position = Vec2(400, 300);
//   player.size     = Vec2(1.0f, 1.0f);   // scale; (1,1) = natural atlas size
//   player.rotation = 0.0f;               // radians, applied at the root
//   player.anim("PLAYER", "RUN");         // plays PLAYER_ANIM_RUN
//
//   // each frame:
//   player.update(dt);
//   player.draw(img, atlas, cam);
// =========================
class Anim
{
public:
    Vec2  position = {0.0f, 0.0f};
    Vec2  size     = {1.0f, 1.0f};   // root-level scale multiplier
    float rotation = 0.0f;           // root-level rotation in radians

    // Load the JSON animation file.
    explicit Anim(const char* jsonPath)
    {
        loaded = animator.load(jsonPath);
    }

    // Select which animation clip to play.
    //   anim("PLAYER", "RUN")  →  plays PLAYER_ANIM_RUN
    void anim(const std::string& entity, const std::string& animType)
    {
        animator.play(entity, animType);
    }

    void update(float dt) { animator.update(dt); }

    // Draws the current frame offset by position/size/rotation.
    // The animator's own draw() always starts the root timeline at (0,0) with
    // scale (1,1) and rot 0. We push a GL matrix before calling it so that
    // every element naturally inherits our world transform without touching
    // TimelineAnimator internals at all.
    void draw(Image* img, Atlas* atlas, Camera& cam)
    {
        glPushMatrix();

        // Apply root world transform so all child elements inherit it.
        glTranslatef(position.x, position.y, 0.0f);
        glRotatef(rotation * (180.0f / 3.14159265f), 0.0f, 0.0f, 1.0f);
        glScalef(size.x, size.y, 1.0f);

        animator.draw(img, atlas, cam);

        glPopMatrix();
    }

    bool isLoaded()   const { return loaded; }
    int  frame()      const { return animator.currentFrame; }
    int  frameCount() const { return animator.totalFrames;  }

private:
    TimelineAnimator animator;
    bool loaded = false;
};

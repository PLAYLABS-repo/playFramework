#include "TimelineAnimator.h"
#include "json.hpp"
#include <fstream>
#include <map>
#include <algorithm>

#include "Sprite.h"
#include "Image.h"
#include "Atlas.h"
#include "Camera.h"

#include <GL/gl.h>

using json = nlohmann::json;

// ==========================
// LOAD
// ==========================
bool TimelineAnimator::load(const char* path, const std::string& symbolName)
{
    std::ifstream file(path);
    if (!file.is_open()) return false;

    json j;
    file >> j;

    // Support both export formats from Adobe Animate:
    // - "ANIMATION" (single symbol / stage export) <-- your file uses this
    // - "SYMBOL_DICTIONARY" (multi-symbol export)
    json targetTimeline;
    bool found = false;

    if (j.contains("ANIMATION") && j["ANIMATION"]["SYMBOL_name"] == symbolName)
    {
        targetTimeline = j["ANIMATION"]["TIMELINE"];
        found = true;
    }
    else if (j.contains("SYMBOL_DICTIONARY"))
    {
        for (auto& sym : j["SYMBOL_DICTIONARY"]["Symbols"])
        {
            if (sym["SYMBOL_name"] == symbolName)
            {
                targetTimeline = sym["TIMELINE"];
                found = true;
                break;
            }
        }
    }

    if (!found) return false;

    auto& layers = targetTimeline["LAYERS"];
    std::vector<std::map<int, AnimFrame>> layerTimelines;
    int globalMax = 0;

    for (auto& layer : layers)
    {
        std::map<int, AnimFrame> timeline;

        for (auto& f : layer["Frames"])
        {
            int index    = f["index"];
            int duration = f["duration"];

            AnimFrame frame;
            frame.isExplicit = true;

            // Parse named frame label (e.g. "main", "emote")
            if (f.contains("name"))
                frame.label = f["name"];

            if (f.contains("elements") && !f["elements"].empty())
            {
                for (auto& el : f["elements"])
                {
                    if (!el.contains("SYMBOL_Instance")) continue;
                    auto& inst = el["SYMBOL_Instance"];
                    if (!inst.contains("bitmap")) continue;

                    AnimElement e;
                    e.frame = inst["bitmap"]["name"];

                    auto& d    = inst["DecomposedMatrix"];
                    e.x        = d["Position"]["x"];
                    e.y        = d["Position"]["y"];
                    e.rotation = d["Rotation"]["z"];
                    e.sx       = d["Scaling"]["x"];
                    e.sy       = d["Scaling"]["y"];

                    // transformationPoint: pixel offset from sprite top-left (unscaled)
                    e.pivotX = inst["transformationPoint"]["x"];
                    e.pivotY = inst["transformationPoint"]["y"];

                    frame.elements.push_back(e);
                }
            }

            for (int i = 0; i < duration; i++)
            {
                timeline[index + i] = frame;
                globalMax = std::max(globalMax, index + i);
            }
        }

        // Fill gaps by holding previous keyframe
        for (int i = 1; i <= globalMax; i++)
        {
            if (!timeline.count(i) && timeline.count(i - 1))
            {
                timeline[i]            = timeline[i - 1];
                timeline[i].isExplicit = false;
                timeline[i].label      = "";
            }
        }

        layerTimelines.push_back(timeline);
    }

    frames.resize(globalMax + 1);

    for (int i = 0; i <= globalMax; i++)
    {
        AnimFrame finalFrame;
        for (auto& lt : layerTimelines)
        {
            if (!lt.count(i)) continue;
            const AnimFrame& f = lt.at(i);

            // Carry label from any layer that has one at this index
            if (!f.label.empty())
                finalFrame.label = f.label;

            if (f.isExplicit)
            {
                finalFrame.elements.insert(
                    finalFrame.elements.end(),
                    f.elements.begin(),
                    f.elements.end()
                );
            }
        }
        frames[i] = finalFrame;

        // Register label -> frame index
        if (!frames[i].label.empty())
            labels[frames[i].label] = i;
    }

    return true;
}

// ==========================
// PLAY ANIMATION BY LABEL
// ==========================
void TimelineAnimator::playAnimation(const std::string& label)
{
    if (label.empty())
    {
        currentFrame = 0;
        return;
    }
    auto it = labels.find(label);
    if (it != labels.end())
        currentFrame = it->second;
}

// ==========================
// DRAW
// ==========================
void TimelineAnimator::updateAndDraw(float dt, Image* img, Atlas* atlas, Camera& cam)
{
    if (frames.empty()) return;

    time += dt;
    currentFrame = (int)(time * fps) % (int)frames.size();

    auto& frame = frames[currentFrame];

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, img->textureID);

    for (auto& e : frame.elements)
    {
        glPushMatrix();

        // --------------------------------------------------
        // Get atlas frame UVs + unscaled pixel size
        // --------------------------------------------------
        float u1 = 0, v1 = 0, u2 = 1, v2 = 1;
        float w = 64, h = 64;

        if (atlas)
        {
            Frame f;
            if (atlas->get(e.frame, f))
            {
                float iw = (float)img->width;
                float ih = (float)img->height;
                u1 = f.x / iw;
                v1 = f.y / ih;
                u2 = (f.x + f.w) / iw;
                v2 = (f.y + f.h) / ih;
                w  = f.w;
                h  = f.h;
            }
        }

        // --------------------------------------------------
        // In Adobe Animate's DecomposedMatrix, Position (x,y)
        // is the WORLD position of the element's pivot point.
        // transformationPoint is the pivot offset in LOCAL
        // (unscaled) pixel space from the sprite's top-left.
        //
        // Since Position already encodes where the pivot lands
        // in world space, we just:
        //   1. Move to Position
        //   2. Rotate around origin (pivot is already here)
        //   3. Draw quad offset so the pivot lands correctly
        //      on the quad: shift by -(pivotX*sx, pivotY*sy)
        //      so top-left is at (-pivotX*sx, -pivotY*sy)
        // --------------------------------------------------
        glTranslatef(e.x, e.y, 0);
        glRotatef(e.rotation, 0, 0, 1);

        // Offset quad so pivot point sits at the origin
        float px = e.pivotX * e.sx;
        float py = e.pivotY * e.sy;
        glTranslatef(-px, -py, 0);

        // Draw quad from (0,0) to (w*sx, h*sy)
        float sw = w * e.sx;
        float sh = h * e.sy;

        glBegin(GL_QUADS);
            glTexCoord2f(u1, v1); glVertex2f(0,  0);
            glTexCoord2f(u2, v1); glVertex2f(sw, 0);
            glTexCoord2f(u2, v2); glVertex2f(sw, sh);
            glTexCoord2f(u1, v2); glVertex2f(0,  sh);
        glEnd();

        glPopMatrix();
    }

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}

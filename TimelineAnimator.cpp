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
// LOAD (UNCHANGED CORE LOGIC)
// ==========================
bool TimelineAnimator::load(const char* path, const std::string& symbolName)
{
    std::ifstream file(path);
    if (!file.is_open()) return false;

    json j;
    file >> j;

    auto symbols = j["SYMBOL_DICTIONARY"]["Symbols"];

    for (auto& sym : symbols)
    {
        if (sym["SYMBOL_name"] != symbolName) continue;

        auto layers = sym["TIMELINE"]["LAYERS"];

        std::vector<std::map<int, AnimFrame>> layerTimelines;

        int globalMax = 0;

        for (auto& layer : layers)
        {
            std::map<int, AnimFrame> timeline;

            for (auto& f : layer["Frames"])
            {
                int index = f["index"];
                int duration = f["duration"];

                AnimFrame frame;
                frame.isExplicit = true;

                if (f.contains("elements") && !f["elements"].empty())
                {
                    for (auto& el : f["elements"])
                    {
                        if (!el.contains("SYMBOL_Instance")) continue;

                        auto& inst = el["SYMBOL_Instance"];
                        if (!inst.contains("bitmap")) continue;

                        AnimElement e;

                        e.frame = inst["bitmap"]["name"];

                        auto& d = inst["DecomposedMatrix"];

                        e.x = d["Position"]["x"];
                        e.y = d["Position"]["y"];

                        e.rotation = d["Rotation"]["z"];

                        e.sx = d["Scaling"]["x"];
                        e.sy = d["Scaling"]["y"];

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

            for (int i = 1; i <= globalMax; i++)
            {
                if (!timeline.count(i) && timeline.count(i - 1))
                {
                    timeline[i] = timeline[i - 1];
                    timeline[i].isExplicit = false;
                }
            }

            layerTimelines.push_back(timeline);
        }

        frames.resize(globalMax + 1);

        for (int i = 0; i <= globalMax; i++)
        {
            AnimFrame finalFrame;

            for (auto& layer : layerTimelines)
            {
                if (!layer.count(i)) continue;

                const AnimFrame& f = layer.at(i);

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
        }

        return true;
    }

    return false;
}

// ==========================
// DRAW WITH CORRECT PIVOT ROTATION
// ==========================
void TimelineAnimator::updateAndDraw(float dt, Image* img, Atlas* atlas, Camera& cam)
{
    if (frames.empty()) return;

    time += dt;
    int frameIndex = (int)(time * fps) % frames.size();

    auto& frame = frames[frameIndex];

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (auto& e : frame.elements)
    {
        glPushMatrix();

        glBindTexture(GL_TEXTURE_2D, img->textureID);

        // =========================
        // ATLAS FRAME
        // =========================
        float u1 = 0, v1 = 0, u2 = 1, v2 = 1;
        float w = 64, h = 64;

        if (atlas)
        {
            Frame f;
            if (atlas->get(e.frame, f))
            {
                u1 = f.x / (float)img->width;
                v1 = f.y / (float)img->height;
                u2 = (f.x + f.w) / (float)img->width;
                v2 = (f.y + f.h) / (float)img->height;

                w = f.w;
                h = f.h;
            }
        }

        // =========================
        // STEP 1: MOVE TO WORLD POS
        // =========================
        glTranslatef(e.x, e.y, 0);

        // =========================
        // STEP 2: MOVE TO PIVOT CENTER FIRST
        // =========================
        glTranslatef(e.pivotX, e.pivotY, 0);

        // =========================
        // STEP 3: ROTATE (THIS MUST BE VISIBLE NOW)
        // =========================
        glRotatef(e.rotation, 0, 0, 1);

        // =========================
        // STEP 4: DRAW FROM CENTER (IMPORTANT)
        // =========================
        float hw = (w * 0.5f) * e.sx;
        float hh = (h * 0.5f) * e.sy;

        glBegin(GL_QUADS);

        glTexCoord2f(u1, v1); glVertex2f(-hw, -hh);
        glTexCoord2f(u2, v1); glVertex2f( hw, -hh);
        glTexCoord2f(u2, v2); glVertex2f( hw,  hh);
        glTexCoord2f(u1, v2); glVertex2f(-hw,  hh);

        glEnd();

        glPopMatrix();
    }

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}

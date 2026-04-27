#include "TimelineAnimator.h"
#include "Image.h"
#include "Atlas.h"
#include "Camera.h"

#include <GL/gl.h>
#include <fstream>
#include <cmath>
#include "json.hpp"

using json = nlohmann::json;

// =========================
// HELPERS
// =========================
static float safeFloat(const json& j, const char* key, float fallback = 0.0f)
{
    if (!j.contains(key) || j[key].is_null()) return fallback;
    return j[key].get<float>();
}

static int safeInt(const json& j, const char* key, int fallback = 0)
{
    if (!j.contains(key) || j[key].is_null()) return fallback;
    return j[key].get<int>();
}

static std::string safeString(const json& j, const char* key, const std::string& fallback = "")
{
    if (!j.contains(key) || j[key].is_null()) return fallback;
    return j[key].get<std::string>();
}

// =========================
// ELEMENT PARSE (FIXED)
// =========================
static TA_Element parseElement(const json& e)
{
    TA_Element el;

    // -------------------------
    // ATLAS SPRITE
    // -------------------------
    if (e.contains("ATLAS_SPRITE_instance") && !e["ATLAS_SPRITE_instance"].is_null())
    {
        auto& sp = e["ATLAS_SPRITE_instance"];

        el.spriteName = safeString(sp, "name");

        if (sp.contains("Position"))
        {
            el.position.x = safeFloat(sp["Position"], "x");
            el.position.y = safeFloat(sp["Position"], "y");
        }
    }

    // -------------------------
    // SYMBOL
    // -------------------------
    if (e.contains("SYMBOL_Instance") && !e["SYMBOL_Instance"].is_null())
    {
        auto& inst = e["SYMBOL_Instance"];

        // transform
        if (inst.contains("DecomposedMatrix"))
        {
            auto& dm = inst["DecomposedMatrix"];

            if (dm.contains("Position"))
            {
                el.position.x = safeFloat(dm["Position"], "x");
                el.position.y = safeFloat(dm["Position"], "y");
            }

            if (dm.contains("Scaling"))
            {
                el.scale.x = safeFloat(dm["Scaling"], "x", 1.0f);
                el.scale.y = safeFloat(dm["Scaling"], "y", 1.0f);
            }

            if (dm.contains("Rotation"))
            {
                el.rotation = safeFloat(dm["Rotation"], "z");
            }
        }

        // ❌ IGNORE Animate pivot (UNRELIABLE)
        el.pivot = {0, 0};

        // bitmap inside symbol
        if (inst.contains("bitmap") && !inst["bitmap"].is_null())
        {
            auto& bm = inst["bitmap"];

            el.spriteName = safeString(bm, "name");

            if (bm.contains("Position"))
            {
                el.bitmapOff.x = safeFloat(bm["Position"], "x");
                el.bitmapOff.y = safeFloat(bm["Position"], "y");
            }
        }
        else
        {
            el.symbolName = safeString(inst, "SYMBOL_name");

            std::string sType = safeString(inst, "symbolType");
            if (sType == "graphic")
            {
                el.isGraphic = true;
                el.firstFrame = safeInt(inst, "firstFrame", 0);
            }
        }
    }

    return el;
}

// =========================
// LAYERS
// =========================
static int parseLayers(const json& timelineNode, TA_Timeline& out)
{
    int maxFrame = 0;
    if (!timelineNode.contains("LAYERS")) return 1;

    for (auto& layer : timelineNode["LAYERS"])
    {
        TA_Layer l;

        for (auto& frame : layer["Frames"])
        {
            TA_Frame f;

            f.index = safeInt(frame, "index");
            f.duration = safeInt(frame, "duration", 1);

            maxFrame = std::max(maxFrame, f.index + f.duration);

            for (auto& e : frame["elements"])
                f.elements.push_back(parseElement(e));

            l.frames.push_back(f);
        }

        out.layers.push_back(l);
    }

    return std::max(maxFrame, 1);
}

// =========================
// LOAD
// =========================
bool TimelineAnimator::load(const char* path)
{
    std::ifstream file(path);
    if (!file.is_open()) return false;

    json j;
    try { file >> j; }
    catch (...) { return false; }

    if (!j.contains("SYMBOL_DICTIONARY")) return false;

    for (auto& sym : j["SYMBOL_DICTIONARY"]["Symbols"])
    {
        std::string name = safeString(sym, "SYMBOL_name");
        if (name.empty()) continue;

        TA_Timeline t;

        if (sym.contains("TIMELINE"))
            t.totalFrames = parseLayers(sym["TIMELINE"], t);
        else
            t.totalFrames = 1;

        symbols[name] = std::move(t);
    }

    return true;
}

// =========================
// PLAY
// =========================
void TimelineAnimator::play(const std::string& entity, const std::string& animType)
{
    std::string key = entity + "_ANIM_" + animType;

    if (!symbols.count(key)) return;

    activeTimeline = &symbols[key];
    totalFrames = activeTimeline->totalFrames;

    currentFrame = 0;
    frameTimer = 0.0f;
}

// =========================
// UPDATE
// =========================
void TimelineAnimator::update(float dt)
{
    if (!activeTimeline) return;

    frameTimer += dt;

    if (frameTimer >= 1.0f / fps)
    {
        frameTimer = 0.0f;
        currentFrame++;

        if (currentFrame >= totalFrames)
            currentFrame = 0;
    }
}

// =========================
// DRAW ENTRY
// =========================
void TimelineAnimator::draw(Image* img, Atlas* atlas, Camera& cam)
{
    if (!activeTimeline) return;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    drawTimeline(*activeTimeline, img, atlas,
        Vec2{0, 0}, 0.0f, Vec2{1, 1}, currentFrame);

    glDisable(GL_BLEND);
}

// =========================
// SPRITE DRAW (FIXED 8 ARG VERSION)
// =========================
void TimelineAnimator::drawSprite(
    const std::string& name,
    Image* img,
    Atlas* atlas,
    Vec2 pos,
    float rotRad,
    Vec2 scale,
    Vec2 pivot,
    Vec2 bitmapOff
)
{
    Frame fr;
    if (!atlas->get(name, fr)) return;

    float rotDeg = rotRad * 57.2957795f;

    float w = fr.w * scale.x;
    float h = fr.h * scale.y;

    // =========================
    // FIXED PIVOT (CENTER OF SPRITE)
    // =========================
    float px = (fr.w * 0.5f) * scale.x;
    float py = (fr.h * 0.5f) * scale.y;

    float bx = bitmapOff.x * scale.x;
    float by = bitmapOff.y * scale.y;

    float u1 = fr.x / (float)img->width;
    float v1 = fr.y / (float)img->height;
    float u2 = (fr.x + fr.w) / (float)img->width;
    float v2 = (fr.y + fr.h) / (float)img->height;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, img->textureID);

    glPushMatrix();

    glTranslatef(pos.x, pos.y, 0);

    glTranslatef(px, py, 0);
    glRotatef(rotDeg, 0, 0, 1);
    glTranslatef(-px, -py, 0);

    glTranslatef(bx, by, 0);

    glBegin(GL_QUADS);
    glTexCoord2f(u1, v1); glVertex2f(0, 0);
    glTexCoord2f(u2, v1); glVertex2f(w, 0);
    glTexCoord2f(u2, v2); glVertex2f(w, h);
    glTexCoord2f(u1, v2); glVertex2f(0, h);
    glEnd();

    glPopMatrix();
}

// =========================
// DRAW TIMELINE
// =========================
void TimelineAnimator::drawTimeline(
    TA_Timeline& timeline,
    Image* img,
    Atlas* atlas,
    Vec2 parentPos,
    float parentRot,
    Vec2 parentScale,
    int frame
)
{
    for (int li = (int)timeline.layers.size() - 1; li >= 0; li--)
    {
        auto& layer = timeline.layers[li];

        for (auto& f : layer.frames)
        {
            if (frame < f.index || frame >= f.index + f.duration)
                continue;

            for (auto& e : f.elements)
            {
                float cosR = cosf(parentRot);
                float sinR = sinf(parentRot);

                float lx = e.position.x * parentScale.x;
                float ly = e.position.y * parentScale.y;

                Vec2 pos = {
                    parentPos.x + cosR * lx - sinR * ly,
                    parentPos.y + sinR * lx + cosR * ly
                };

                float rot = parentRot + e.rotation;

                Vec2 scale = {
                    parentScale.x * e.scale.x,
                    parentScale.y * e.scale.y
                };

                if (!e.spriteName.empty())
                {
                    drawSprite(e.spriteName, img, atlas,
                        pos, rot, scale, e.pivot, e.bitmapOff);
                }

                if (!e.symbolName.empty() && symbols.count(e.symbolName))
                {
                    auto& sym = symbols[e.symbolName];

                    int symFrame = e.isGraphic
                        ? (e.firstFrame % std::max(sym.totalFrames, 1))
                        : (frame % std::max(sym.totalFrames, 1));

                    drawTimeline(sym, img, atlas, pos, rot, scale, symFrame);
                }
            }
        }
    }
}

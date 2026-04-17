#include "Animation.h"

#include <fstream>
#include <iostream>
#include "json.hpp"

using json = nlohmann::json;

bool Animation::load(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
        return false;

    json j;
    file >> j;

    frames.clear();

    for (auto& f : j["frames"])
    {
        AnimFrame frame;
        frame.index = f["index"];
        frame.duration = f["duration"];

        for (auto& e : f["elements"])
        {
            SymbolInstance inst;

            auto& sym = e["SYMBOL_Instance"];

            inst.symbolName = sym["SYMBOL_name"];

            if (sym.contains("DecomposedMatrix"))
            {
                inst.x = sym["DecomposedMatrix"]["Position"]["x"];
                inst.y = sym["DecomposedMatrix"]["Position"]["y"];

                inst.scaleX = sym["DecomposedMatrix"]["Scaling"]["x"];
                inst.scaleY = sym["DecomposedMatrix"]["Scaling"]["y"];
            }

            frame.elements.push_back(inst);
        }

        frames.push_back(frame);
    }

    return true;
}

void Animation::reset()
{
    current = 0;
    timer = 0;
}

void Animation::update(float dt)
{
    if (frames.empty()) return;

    timer += dt * 1000.0f;

    if (timer >= frames[current].duration)
    {
        timer = 0;
        current++;

        if (current >= frames.size())
            current = 0;
    }
}

const AnimFrame& Animation::getFrame() const
{
    return frames[current];
}

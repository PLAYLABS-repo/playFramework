#include "Atlas.h"
#include "json.hpp"
#include <fstream>

using json = nlohmann::json;

bool Atlas::load(const char* path)
{
    std::ifstream file(path);
    if (!file.is_open())
        return false;

    json j;
    file >> j;

    auto sprites = j["ATLAS"]["SPRITES"];

    for (auto& s : sprites)
    {
        auto sp = s["SPRITE"];

        Frame f;
        f.x = sp["x"];
        f.y = sp["y"];
        f.w = sp["w"];
        f.h = sp["h"];

        std::string name = sp["name"];
        frames[name] = f;
    }

    return true;
}

bool Atlas::get(const std::string& name, Frame& out) const
{
    auto it = frames.find(name);
    if (it == frames.end())
        return false;

    out = it->second;
    return true;
}

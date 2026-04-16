#include "Atlas.h"
#include "json.hpp"
#include <fstream>

using json = nlohmann::json;

bool Atlas::load(const char* path)
{
    std::ifstream file(path);
    if (!file.is_open()) return false;

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

        frames[sp["name"]] = f;
    }

    return true;
}

Frame Atlas::get(const std::string& name)
{
    return frames[name];
}

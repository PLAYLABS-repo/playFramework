#pragma once
#include <map>
#include <string>

struct Frame
{
    float x, y, w, h;
};

class Atlas
{
public:
    std::map<std::string, Frame> frames;

    bool load(const char* path);
    Frame get(const std::string& name);
};

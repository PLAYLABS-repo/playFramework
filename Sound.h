#pragma once
#include <windows.h>
#include <string>

class Sound
{
public:
    Sound();

    bool load(const char* path);
    void play(bool loop = false);
    void stop();

private:
    std::string file;
};

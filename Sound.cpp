#include "Sound.h"
#include <mmsystem.h>
#include <stdio.h>

#pragma comment(lib, "winmm.lib")

int Sound::counter = 0;

Sound::Sound()
{
    alias = "snd_" + std::to_string(counter++);
}

Sound::~Sound()
{
    stop();
}

bool Sound::load(const char* path)
{
    if (!path) return false;

    file = path;
    return true;
}

void Sound::play(bool loop)
{
    std::string closeCmd = "close " + alias;
    mciSendStringA(closeCmd.c_str(), NULL, 0, NULL);

    std::string openCmd =
        "open \"" + file + "\" type waveaudio alias " + alias;

    mciSendStringA(openCmd.c_str(), NULL, 0, NULL);

    std::string playCmd =
        "play " + alias + (loop ? " repeat" : "");

    mciSendStringA(playCmd.c_str(), NULL, 0, NULL);
}
void Sound::stop()
{
    if (alias.empty()) return;

    std::string cmd = "stop " + alias;
    mciSendStringA(cmd.c_str(), NULL, 0, NULL);

    cmd = "close " + alias;
    mciSendStringA(cmd.c_str(), NULL, 0, NULL);
}

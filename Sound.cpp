#include "Sound.h"
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

Sound::Sound()
{
}

bool Sound::load(const char* path)
{
    file = path;
    return true;
}

void Sound::play(bool loop)
{
    if (file.empty()) return;

    PlaySoundA(
        file.c_str(),
        NULL,
        SND_FILENAME | SND_ASYNC | (loop ? SND_LOOP : 0)
    );
}

void Sound::stop()
{
    PlaySoundA(NULL, 0, 0);
}

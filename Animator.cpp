#include "Animation.h"

void Animation::update(float deltaTime)
{
    if (frames.empty()) return;

    timer += deltaTime;

    float frameTime = 1.0f / speed;

    while (timer >= frameTime)
    {
        timer -= frameTime;
        currentFrame++;

        if (currentFrame >= (int)frames.size())
        {
            if (loop)
                currentFrame = 0;
            else
                currentFrame = frames.size() - 1;
        }
    }
}

std::string Animation::getFrame() const
{
    if (frames.empty()) return "";
    return frames[currentFrame];
}

void Animation::reset()
{
    timer = 0.0f;
    currentFrame = 0;
}

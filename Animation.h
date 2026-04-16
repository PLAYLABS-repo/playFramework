#pragma once
#include <vector>
#include <string>

class Animation
{
public:
    std::vector<std::string> frames;

    float speed = 10.0f; // frames per second
    bool loop = true;

    // runtime
    float timer = 0.0f;
    int currentFrame = 0;

    void update(float deltaTime);
    std::string getFrame() const;

    void reset();
};

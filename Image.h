#pragma once
#include <windows.h>
#include <GL/gl.h>

class Image
{
public:
    GLuint tex = 0;
    int w = 0, h = 0;

    bool load(const char* path);
};

#pragma once
#include <windows.h>

class Window
{
public:
    Window();
    ~Window();

    bool create(const char* title, int width, int height);
    bool process();

    HWND getHWND() const;
    HDC getHDC() const;

private:
    HWND hwnd;
    HDC hdc;
    HGLRC hrc;
    bool running;
};

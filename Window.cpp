#include "Window.h"
#include <GL/gl.h>

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

Window::Window()
{
    hwnd = nullptr;
    hdc = nullptr;
    hrc = nullptr;
    running = false;
}

Window::~Window()
{
    if (hrc)
    {
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(hrc);
    }

    if (hdc && hwnd)
        ReleaseDC(hwnd, hdc);

    if (hwnd)
        DestroyWindow(hwnd);
}

bool Window::create(const char* title, int width, int height)
{
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = "playFrameworkWindow";

    RegisterClass(&wc);

    hwnd = CreateWindowEx(
        0,
        wc.lpszClassName,
        title,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        width, height,
        nullptr,
        nullptr,
        wc.hInstance,
        nullptr
    );

    if (!hwnd)
        return false;

    hdc = GetDC(hwnd);

    // basic pixel format (simple legacy OpenGL)
    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int pf = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, pf, &pfd);

    hrc = wglCreateContext(hdc);
    wglMakeCurrent(hdc, hrc);

    running = true;

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    return true;
}

bool Window::process()
{
    MSG msg;

    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            running = false;
            return false;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return running;
}

HWND Window::getHWND() const
{
    return hwnd;
}

HDC Window::getHDC() const
{
    return hdc;
}

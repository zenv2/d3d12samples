#pragma once

#include <windows.h>
#include <string>

namespace render
{
    class Window
    {
    public:
        Window(const int w, const int h, const std::string title);
        ~Window();

        void Show() { ShowWindow(m_hwnd, SW_SHOW); }

        void MainLoop();

        static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

        friend class Swapchain;

    protected:
        const HWND GetWindowHandle() const { return m_hwnd; }
        const int GetWidth() const { return m_width; }
        const int GetHeight() const { return m_height; }

    private:
        HWND m_hwnd;

        int m_width;
        int m_height;
        std::string m_title;
    };
}
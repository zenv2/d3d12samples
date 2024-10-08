#pragma once

#include <windows.h>
#include <string>


namespace render
{
    class Window
    {
    public:
        Window(void* pSample, const uint32_t w, const uint32_t h, const std::string title);
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

        uint32_t m_width;
        uint32_t m_height;
        std::string m_title;
    };
}
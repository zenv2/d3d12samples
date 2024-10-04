#include <window.h>
#include "utils.h"

namespace render
{
    Window::Window(Sample* pSample, const int w, const int h, const std::string title) :
        m_width(w),
        m_height(h),
        m_title(title),
        m_hwnd(NULL)
    {
        WNDCLASSEX windowClass = { 0 };
        windowClass.cbSize = sizeof(WNDCLASSEX);
        windowClass.style = CS_HREDRAW | CS_VREDRAW;
        windowClass.lpfnWndProc = WindowProc;
        windowClass.hInstance = GetModuleHandle(NULL);
        windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        windowClass.lpszClassName = "d3d12samples";
        RegisterClassEx(&windowClass);

        RECT windowRect = { 0, 0, static_cast<LONG>(m_width), static_cast<LONG>(m_height) };
        AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

        m_hwnd = CreateWindowExA(
            0,
            windowClass.lpszClassName,
            m_title.c_str(),
            WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            windowRect.right - windowRect.left,
            windowRect.bottom - windowRect.top,
            nullptr,
            nullptr,
            GetModuleHandle(NULL),
            pSample );
        
        if(m_hwnd == NULL)
        {
            std::runtime_error("Failed to create Window");
        }
    }

    Window::~Window()
    {
        if(m_hwnd)
        {
            DestroyWindow(m_hwnd);
            m_hwnd = NULL;
        }
    }

    void Window::MainLoop()
    {
            MSG msg = {};
        while (msg.message != WM_QUIT)
        {
            // Process any messages in the queue.
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }

    LRESULT CALLBACK Window::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {

        switch(message)
        {
        case WM_CREATE:
            {
                LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
                SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
            }
            return 0;
        case WM_PAINT:
            {
                Sample* pSample = reinterpret_cast<Sample*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
                if(pSample)
                {
                    pSample->OnRender();
                }
            }
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }

        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}
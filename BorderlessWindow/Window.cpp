#include "Window.h"
#include <stdexcept>
#include <dwmapi.h>
#include <windowsx.h>

Window::Window() : hwnd(0),
                   hinstance(GetModuleHandle(NULL)),
                   borderless(false),
                   borderless_movable(true),
                   borderless_resizeable(true),
                   aero_shadow(false),
                   closed(false)
{
    WNDCLASSEX wcx = { 0 };
    wcx.cbSize = sizeof(WNDCLASSEX);
    wcx.style = CS_HREDRAW | CS_VREDRAW;
    wcx.hInstance = hinstance;
    wcx.lpfnWndProc = WndProc;
    wcx.lpszClassName = L"WindowClass";
    wcx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcx.hCursor = LoadCursor(hinstance, IDC_ARROW);
    RegisterClassEx(&wcx);
    if (FAILED(RegisterClassEx(&wcx))) throw std::runtime_error("couldn't register window class");

    hwnd = CreateWindow(L"WindowClass", L"BorderlessWindow", static_cast<DWORD>(Style::windowed), 0, 0, 1280, 720, 0, 0, hinstance, nullptr);

    if (!hwnd) throw std::runtime_error("couldn't create window because of reasons");

    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
}

Window::~Window()
{
    hide();
    DestroyWindow(hwnd);
}

LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    Window *window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (!window) return DefWindowProc(hwnd, msg, wparam, lparam);

    switch (msg)
    {
        case WM_NCCALCSIZE:
            {
                //this kills the window frame and title bar we added with
                //WS_THICKFRAME and WS_CAPTION
                if (window->borderless) return 0;
                break;
            }

        case WM_CLOSE:
            {
                window->closed = true;
                return 0;
            }

        case WM_NCHITTEST:
            {
                if (window->borderless)
                {
                    if (window->borderless_resizeable)
                    {
                        const LONG border_width = 8; //in pixels
                        RECT winrect;
                        GetWindowRect(hwnd, &winrect);
                        long x = GET_X_LPARAM(lparam);
                        long y = GET_Y_LPARAM(lparam);

                        //bottom left corner
                        if (x >= winrect.left && x < winrect.left + border_width &&
                            y < winrect.bottom && y >= winrect.bottom - border_width)
                        {
                            return HTBOTTOMLEFT;
                        }
                        //bottom right corner
                        if (x < winrect.right && x >= winrect.right - border_width &&
                            y < winrect.bottom && y >= winrect.bottom - border_width)
                        {
                            return HTBOTTOMRIGHT;
                        }
                        //top left corner
                        if (x >= winrect.left && x < winrect.left + border_width &&
                            y >= winrect.top && y < winrect.top + border_width)
                        {
                            return HTTOPLEFT;
                        }
                        //top right corner
                        if (x < winrect.right && x >= winrect.right - border_width &&
                            y >= winrect.top && y < winrect.top + border_width)
                        {
                            return HTTOPRIGHT;
                        }
                        //left border
                        if (x >= winrect.left && x < winrect.left + border_width)
                        {
                            return HTLEFT;
                        }
                        //right border
                        if (x < winrect.right && x >= winrect.right - border_width)
                        {
                            return HTRIGHT;
                        }
                        //bottom border
                        if (y < winrect.bottom && y >= winrect.bottom - border_width)
                        {
                            return HTBOTTOM;
                        }
                        //top border
                        if (y >= winrect.top && y < winrect.top + border_width)
                        {
                            return HTTOP;
                        }
                    }

                    if (window->borderless_movable) return HTCAPTION;
                }
                break;
			}

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            {
                if (wparam == VK_F9) window->borderless_movable = !window->borderless_movable;
                if (wparam == VK_F10) window->borderless_resizeable = !window->borderless_resizeable;
                if (wparam == VK_F11)
                {
                    window->toggle_shadow();
                    window->toggle_borderless();
                }
                if (wparam == VK_F12) window->toggle_shadow();
                return 0;
            }
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

//link dwmapi.lib
#pragma comment(lib, "dwmapi.lib")

void Window::toggle_borderless()
{
    Style newStyle = (borderless) ? Style::windowed : Style::aero_borderless;
    SetWindowLongPtr(hwnd, GWL_STYLE,
                     static_cast<LONG>(newStyle));

    borderless = !borderless;
    if (newStyle == Style::aero_borderless)
    {
        toggle_shadow();
    }
    //redraw frame
    SetWindowPos(hwnd, 0, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
    show();
}

void Window::toggle_shadow()
{
    if (borderless)
    {
        aero_shadow = !aero_shadow;
        const MARGINS shadow_on = { 1, 1, 1, 1 };
        const MARGINS shadow_off = { 0, 0, 0, 0 };
        DwmExtendFrameIntoClientArea(hwnd, (aero_shadow) ? (&shadow_on) : (&shadow_off));
    }
}

void Window::show()
{
    ShowWindow(hwnd, SW_SHOW);
}

void Window::hide()
{
    ShowWindow(hwnd, SW_HIDE);
}



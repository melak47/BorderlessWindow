#include <windows.h>

class Window
{
    // we cannot just use WS_POPUP style
    // WS_THICKFRAME: without this the window cannot be resized and so aero snap, de-maximizing and minimizing won't work
    // WS_SYSMENU: enables the context menu with the move, close, maximize, minize... commands (shift + right-click on the task bar item)
    // HOWEVER, this means that the buttons actually exist inside your client area and are clickable. 
    // WS_MAXIMIZEBOX, WS_MINIMIZEBOX: enables maximize, minimize
    // WS_CAPTION: enables aero minimize animation/transition
    enum class Style : DWORD
    {
        windowed = (WS_OVERLAPPEDWINDOW | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME),
        aero_borderless = ( WS_POPUP | WS_CAPTION | WS_THICKFRAME)
    };

public:
    HWND hwnd;
    HINSTANCE hinstance;
    bool borderless;
    bool aero_shadow;
    bool closed;

    Window();
    ~Window();
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    void toggle_borderless();
    void toggle_shadow();
    void show();
    void hide();
};
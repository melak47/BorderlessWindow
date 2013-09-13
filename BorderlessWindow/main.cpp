#include "Window.h"
#include <stdexcept>
#include <thread>

int main()
{
    try
    {
        Window window;
        window.show();

        MSG msg = { 0 };
        while (!window.closed)
        {
            while (PeekMessage(&msg, window.hwnd, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            std::this_thread::yield();
            //force window to repaint, since we're not actually drawing anything ourselves
            InvalidateRect(window.hwnd, nullptr, true);
        }
    }
    catch (std::exception& e)
    {
        MessageBoxA(0, (std::string("I guess my example code sucks:\n\n") + e.what()).c_str(), "=(", 0);
    }
}
#include <stdexcept>
#include <typeinfo>
#include <string>

#include "Window.hpp"

int main() try {
    BorderlessWindow window;

    MSG msg {};
    while (!window.is_closed()) {
        while (PeekMessage(&msg, window.handle(), 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        //force window to repaint, since we're not actually drawing anything ourselves
        InvalidateRect(window.handle(), nullptr, true);
    }
} catch (const std::exception& e) {
	using namespace std::literals;

	std::string message = "Unhandled exception("s + typeid(e).name() + ") : " + e.what();
    MessageBoxA(0, message.c_str(), "=(", 0);
}

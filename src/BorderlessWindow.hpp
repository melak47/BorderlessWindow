#pragma once

#include <memory>
#include <string>

#include <Windows.h>

struct hwnd_deleter {
    using pointer = HWND;
    auto operator()(HWND handle) const -> void {
        ::DestroyWindow(handle);
    }
};

using unique_handle = std::unique_ptr<HWND, hwnd_deleter>;

class BorderlessWindow {
public:
    BorderlessWindow();
    auto set_borderless(bool enabled) -> void;
    auto set_borderless_shadow(bool enabled) -> void;

private:
    static auto CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept -> LRESULT;
    auto hit_test(POINT cursor) const -> LRESULT;

    bool borderless        = true; // is the window currently borderless
    bool borderless_resize = true; // should the window allow resizing by dragging the borders while borderless
    bool borderless_drag   = true; // should the window allow moving my dragging the client area
    bool borderless_shadow = true; // should the window display a native aero shadow while borderless

    unique_handle handle;
};

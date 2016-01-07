#pragma once

#include <memory>
#include <string>

#include <Windows.h>

struct HWND_deleter {
	using pointer = HWND;
	void operator()(HWND handle) const;
};

using unique_hwnd = std::unique_ptr<HWND__, HWND_deleter>;

class BorderlessWindow
{
public:
    BorderlessWindow();
	bool is_closed() const { return closed; }
	HWND handle() const { return hwnd.get(); }

	void set_borderless(bool enabled);
	void set_borderless_shadow(bool enabled);

private:
	unique_hwnd hwnd;

	bool borderless        = true;  // is the window currently borderless
	bool closed            = false; // has the window been closed
	bool borderless_resize = true;	// should the window allow resizing by dragging the borders while borderless
	bool borderless_move   = true;	// should the window allow moving my dragging the client area
	bool borderless_shadow = true;  // should the window display a native aero window shadow in borderless mode

	static const std::wstring& window_class();
	static HMODULE module_handle();
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	void show() const;
	void set_shadow(bool enabled) const;
};

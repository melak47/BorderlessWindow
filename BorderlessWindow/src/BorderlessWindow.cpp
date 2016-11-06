#include "BorderLessWindow.hpp"

#include <cassert>
#include <stdexcept>

#include <Windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")


// we cannot just use WS_POPUP style
// WS_THICKFRAME: without this the window cannot be resized and so aero snap, de-maximizing and minimizing won't work
// WS_SYSMENU: enables the context menu with the move, close, maximize, minize... commands (shift + right-click on the task bar item)
// HOWEVER, this also enables the menu with the maximize buttons in the title bar, which will exist inside your client area and are clickable.
// WS_CAPTION: enables aero minimize animation/transition
// WS_MAXIMIZEBOX, WS_MINIMIZEBOX: enable minimize/maximize
enum class Style : DWORD {
	windowed         = WS_OVERLAPPEDWINDOW | WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
	aero_borderless  = WS_POPUP            | WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX,
	basic_borderless = WS_POPUP            | WS_THICKFRAME              | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX
};


HMODULE BorderlessWindow::module_handle() {
	static HMODULE hinstance = ::GetModuleHandle(nullptr);
	return hinstance;
}

const wchar_t* BorderlessWindow::window_class() {
	static const wchar_t* window_class_name = [] {
		WNDCLASSEX wcx{};
		wcx.cbSize = sizeof(wcx);
		wcx.style = CS_HREDRAW | CS_VREDRAW;
		wcx.hInstance = module_handle();
		wcx.lpfnWndProc = &BorderlessWindow::WndProc;
		wcx.lpszClassName = L"BorderlessWindowClass";
		wcx.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
		wcx.hCursor = LoadCursor(module_handle(), IDC_ARROW);
		const HRESULT result = ::RegisterClassEx(&wcx);
		if (FAILED(result))
			throw std::runtime_error("failed to register window class");
		return wcx.lpszClassName;
	}();
	return window_class_name;
}


BorderlessWindow::BorderlessWindow()
: hwnd{ ::CreateWindow( window_class(), L"Borderless Window",
	static_cast<DWORD>(Style::aero_borderless), 0, 0,
	1280, 720, nullptr, nullptr, module_handle(), nullptr) } {
	if (!hwnd)
		throw std::runtime_error("failed to create window");

	static_assert(sizeof(LONG_PTR) == sizeof(this), "sizeof(LONG_PTR) != sizeof(this)");

	// store window instance pointer in window user data so WndProc can access it
	::SetWindowLongPtr(hwnd.get(), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

	set_borderless(borderless);
	set_borderless_shadow(borderless_shadow);
	show();
}

bool composition_enabled() {
	BOOL composition_enabled = FALSE;
	bool success = ::DwmIsCompositionEnabled(&composition_enabled) == S_OK;
	return composition_enabled && success;
}

struct HitRegion {
	RECT    bounds;
	LRESULT region;

	bool contains(const POINT& point) const {
		return point.x >= bounds.left && point.x < bounds.right &&
		       point.y >= bounds.top  && point.y < bounds.bottom;
	}
};

LRESULT BorderlessWindow::hit_test(POINT point) const {

	if (borderless && (borderless_resize || borderless_move)) {
		// identify borders and corners to allow resizing the window.
		// Note: On Windows 10, windows behave differently and
		// allow resizing outside the visible window frame.
		// This implementation does not replicate that behavior.
		const auto border_x = ::GetSystemMetrics(SM_CYFRAME) + ::GetSystemMetrics(SM_CXPADDEDBORDER);
		const auto border_y = ::GetSystemMetrics(SM_CYFRAME) + ::GetSystemMetrics(SM_CXPADDEDBORDER);
		RECT winrect;
		::GetWindowRect(handle(), &winrect);

		const auto drag = borderless_move ? HTCAPTION : HTCLIENT;

		const HitRegion hitregions[] {
			{ { winrect.left            , winrect.bottom - border_y
			  , winrect.left  + border_x, winrect.bottom }
			, borderless_resize ? HTBOTTOMLEFT : drag },

			{ { winrect.right - border_x, winrect.bottom - border_y
			  , winrect.right           , winrect.bottom }
			, borderless_resize ? HTBOTTOMRIGHT : drag },

			{ { winrect.left              , winrect.top
			  , winrect.left  + border_x  , winrect.top  + border_y }
			, borderless_resize ? HTTOPLEFT : drag },

			{ { winrect.right - border_x , winrect.top
			  , winrect.right            , winrect.top   + border_y }
			, borderless_resize ? HTTOPRIGHT : drag },

			{ { winrect.left             , winrect.top
			  , winrect.left + border_x  , winrect.bottom }
			, borderless_resize ? HTLEFT : drag},

			{ { winrect.right - border_x , winrect.top
			  , winrect.right            , winrect.bottom }
			, borderless_resize ? HTRIGHT : drag},

			{ { winrect.left             , winrect.top
			  , winrect.right            , winrect.top + border_y }
			, borderless_resize ? HTTOP : drag },

			{ { winrect.left             , winrect.bottom - border_y
			  , winrect.right            , winrect.bottom }
			, borderless_resize ? HTBOTTOM : drag },

			{ winrect
			, drag }
		};

		for (auto&& hr : hitregions) {
			if (hr.contains(point)) {
				return hr.region;
			}
		}
	}

	return HTNOWHERE;
}


LRESULT CALLBACK BorderlessWindow::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (auto window_ptr = reinterpret_cast<BorderlessWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA))) {
		auto& window = *window_ptr;

		switch (msg) {
			case WM_NCCALCSIZE: {
				// this kills the window frame and title bar we added with
				// WS_THICKFRAME or WS_CAPTION
				if (window.borderless) return 0;
				break;
			}
			case WM_NCHITTEST: {
				// When we have no border or title bar, we need to perform our
				// own hit testing to allow resizing and moving.
				if (window.borderless) {
					const POINT cursor {
						GET_X_LPARAM(lparam),
						GET_Y_LPARAM(lparam)
					};

					return window.hit_test(cursor);
				}
				break;
			}
			case WM_NCACTIVATE: {
				if (!composition_enabled()) {
					// Prevents window frame reappearing on windwo activation
					// in "basic" theme, where no aero shadow is present.
					return 1;
				}
				break;
			}


			case WM_CLOSE: {
				window.closed = true;
				return 0;
			}

			case WM_KEYDOWN:
			case WM_SYSKEYDOWN: {
				switch (wparam) {
					case VK_F8:  { window.borderless_move = !window.borderless_move;        return 0; }
					case VK_F9:  { window.borderless_resize = !window.borderless_resize;    return 0; }
					case VK_F10: { window.set_borderless(!window.borderless);               return 0; }
					case VK_F11: { window.set_borderless_shadow(!window.borderless_shadow); return 0; }
				}
				break;
			}
		}
	}

	return ::DefWindowProc(hwnd, msg, wparam, lparam);
}


Style select_borderless_style() {
	return composition_enabled() ? Style::aero_borderless : Style::basic_borderless;
}

void BorderlessWindow::set_borderless(bool enabled) {
	Style new_style = (enabled) ? select_borderless_style() : Style::windowed;
	Style old_style = static_cast<Style>(::GetWindowLongPtr(hwnd.get(), GWL_STYLE));

	if (new_style != old_style) {
		borderless = enabled;

		::SetWindowLongPtr(hwnd.get(), GWL_STYLE, static_cast<LONG>(new_style));

		// when switching between borderless and windowed, restore appropriate shadow state
		set_shadow(borderless_shadow && (new_style != Style::windowed));

		// redraw frame
		::SetWindowPos(hwnd.get(), nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
		show();
	}
}

void BorderlessWindow::set_shadow(bool enabled) const {
	if (composition_enabled()) {
		static const MARGINS shadow_state[2] {{0,0,0,0},{1,1,1,1}};
		::DwmExtendFrameIntoClientArea(hwnd.get(), &shadow_state[enabled]);
	}
}

void BorderlessWindow::set_borderless_shadow(bool enabled) {
	if (borderless) {
		borderless_shadow = enabled;
		set_shadow(enabled);
	}
}

void BorderlessWindow::show() const {
	::ShowWindow(hwnd.get(), SW_SHOW);
}

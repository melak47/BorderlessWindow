BorderlessWindow
================

This sample application demonstrates the necessary WinAPI calls and window 
messages to handle to produce a resizable, borderless window (similar to Steam, GitHub for Windows).



![sample](https://cloud.githubusercontent.com/assets/3340026/20039089/69b3b2f6-a43e-11e6-8a44-21599a34ceba.png)

The borderless window can be resized, moved, and also supports all the Aero features a regular window has:

- snapping to desktop halves/quadrants
- shake to minimize all other windows
- animations when minimizing, maximizing, restoring, snapping
- native soft shadow around the window

The sample should work on Windows 7, 8/8.1 and 10.

What this example does *not* do:

- Draw anything to the client area. You will need to fill the entire window with an opaque color, or the window frame may be visible inside your client area in borderless mode. 
  In my use case I simply fill the D3D backbuffer covering the window's client area.
- Calculate proper client/window size in windowed mode. You will need to use [AdjustWindowRect](https://msdn.microsoft.com/en-us/library/windows/desktop/ms632665(v=vs.85).aspx) and friends to calculate the correct window size for a desired client area size.

Keybinds:

- F8  enables/disables dragging in the borderless window to move it 
- F9  enables/disables resizing the borderless window
- F10 toggles between borderless and windowed mode
- F11 toggles the aero shadow when in borderless mode

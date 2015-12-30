BorderlessWindow
================

This is a win32 application that seeks to demonstrate how to achieve a borderless 
window look, similar to Steam, GitHub for Windows and other applications:

![sample](https://cloud.githubusercontent.com/assets/3340026/12052255/47bc05b2-af0d-11e5-9cb7-e9d9a3691a79.png)

This example application supports resizing, dragging, and all the Aero features
- Aero Snap 
- Aero Shake
- Animations when minimizing, maximizing, restoring, snapping
- native soft shadow around the window

and should work on Windows 7, 8/8.1 and 10.

What this example does *not* do:
- Draw anything to the client area. You will need to fill the entire window with an opaque color, or the window frame may be visible inside your client area in borderless mode. 
  In my use case I simply fill the D3D backbuffer covering the window's client area.
- Calculate proper client/window size in windowed mode. You will need to use [AdjustWindowRect](https://msdn.microsoft.com/en-us/library/windows/desktop/ms632665(v=vs.85).aspx) and friends to calculate the correct window size for a desired client area size.

Keybinds: 
- F10 toggles between borderless and windowed mode
- F11 toggles the aero shadow when in borderless mode

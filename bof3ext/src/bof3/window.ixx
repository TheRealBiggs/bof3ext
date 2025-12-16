module;

#include <cstdint>

#include <windows.h>

export module bof3.window;

import bof3ext.helpers;


export Accessor<0x6BE1D0, Rect> g_DisplayRect;
export Accessor<0x65DA44, BOOL> g_IsFullscreen;
export Accessor<0x65DA48, BOOL> g_UseHardwareRendering;
export Accessor<0x6BE1C8, bool> g_IsVideoPlaying;

export Func<0x5A5130, void, uint32_t /* x */, uint32_t /* y */> SetDisplayRect;
export Func<0x5A5160, int, HWND /* hWnd */, BOOL* /* a2 */, BOOL* /* a3 */, int* /* a4 */> sub_5A5160;
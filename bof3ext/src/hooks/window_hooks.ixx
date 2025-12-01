module;

#include <cstdint>

#include <windows.h>

#include <mciapi.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

export module bof3ext.hooks:window;

import bof3ext.helpers;
import bof3.window;

import std;


constexpr int RENDER_WIDTH = 320 * 4;
constexpr int RENDER_HEIGHT = 240 * 4;

const char* WINDOW_TITLE = "Breath of Fire 3";


Func<0x5A5160, int, HWND /* hWnd */, BOOL* /* a2 */, BOOL* /* a3 */, int* /* a4 */> sub_5A5160;


Func<0x5A5130, void, uint32_t /* x */, uint32_t /* y */> SetDisplayRect;
FuncHook<decltype(SetDisplayRect)> SetDisplayRectHook = [](auto x, auto y) {
	g_DisplayRect->left = x;
	g_DisplayRect->top = y;
	g_DisplayRect->right = x + RENDER_WIDTH;
	g_DisplayRect->bottom = y + RENDER_HEIGHT;
};


Func<0x59E360, void, const char* /* filename */, HWND /* hWnd */, BOOL /* a3 */> PlayMovieFile;
FuncHook<decltype(PlayMovieFile)> PlayMovieFileHook = [](auto filename, auto hWnd, auto a3) {
	sub_5A5160(hWnd, &bool_65DA44, &bool_65DA48, nullptr);

	auto cmd = std::format("open avivideo!{} alias vfw", filename);

	if (mciSendStringA(cmd.c_str(), nullptr, 0, 0) != S_OK)
		return;

	LONG prevWndProc = SetWindowLongA(hWnd, GWL_WNDPROC, 0x59E570 /* WndProc_VideoPlayback */);

	cmd = std::format("window vfw handle {}", (unsigned)hWnd);

	mciSendStringA(cmd.c_str(), nullptr, 0, 0);

	RECT clientRect;
	GetClientRect(hWnd, (LPRECT)&clientRect);

	int x = 0, y = 0;
	int width = 640, height = 480;

	float scaleX = clientRect.right / 640.0f;
	float scaleY = clientRect.bottom / 480.0f;

	if (scaleX == scaleY) {
		width = clientRect.right;
		height = clientRect.bottom;
	} else if (scaleX < scaleY) {
		int diff = (int)((height * scaleY) - (height * scaleX));

		y = diff / 2;
		width = clientRect.right;
		height = (int)(height * scaleX);
	} else {
		int diff = (int)((width * scaleX) - (width * scaleY));

		x = diff / 2;
		width = (int)(width * scaleY);
		height = clientRect.bottom;
	}

	cmd = std::format("put vfw destination at {} {} {} {}", x, y, width, height);

	mciSendStringA(cmd.c_str(), nullptr, 0, 0);
	mciSendStringA("play vfw window from 0 notify", nullptr, 0, hWnd);

	*g_IsVideoPlaying = true;

	MSG msg;

	do {
		if (!GetMessageA(&msg, 0, 0, 0))
			break;

		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	} while (*g_IsVideoPlaying);

	mciSendStringA("stop vfw wait", nullptr, 0, 0);
	mciSendStringA("close vfw wait", nullptr, 0, 0);

	SetWindowLongA(hWnd, GWL_WNDPROC, prevWndProc);
};


export void EnableWindowHooks() {
	EnableHook(SetDisplayRect, SetDisplayRectHook);
	EnableHook(PlayMovieFile, PlayMovieFileHook);

	WriteProtectedMemory(0x4FCCD2, WINDOW_TITLE);

	// Set window/render width/height
	WriteProtectedMemory(0x4FCB44, (uint16_t)RENDER_WIDTH);
	WriteProtectedMemory(0x4FCB4C, (uint16_t)RENDER_HEIGHT);

	*(uint32_t*)0x66B710 = RENDER_WIDTH;
	*(uint32_t*)0x66B714 = RENDER_HEIGHT;

	// Skip code that resets render width and height
	uint8_t nop[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };

	WriteProtectedMemory(0x5A548A, nop);
	WriteProtectedMemory(0x5A54B0, nop);
}
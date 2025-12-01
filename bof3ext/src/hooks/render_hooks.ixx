module;

#include <cstdint>
#include <cstdio>
#include <d3d.h>
#include <intrin.h>

export module bof3ext.hooks:render;

import bof3ext.glyphManager;
import bof3ext.helpers;
import bof3ext.textManager;

import bof3.render;
import bof3.text;

import std;


Func<0x516D50, void, int16_t /* x */, int16_t /* y */, int16_t /* w */, int16_t /* h */, int16_t /* a5 */, int8_t /* a6 */, uint16_t /* a7 */> sub_516D50;


FuncHook<decltype(DrawString)> DrawStringHook = [](auto x, auto y, auto paletteIdx, auto len, auto text) {
	LogDebug("DrawString: %s (%p)\n", text, _ReturnAddress());

	// Adjust text position for centered text to adjust for reduced character width
	if (*dword_905B84 != 0 && *(uint8_t*)(*dword_905B84 + 2) == 4) {
		auto centerPos = *(unsigned short*)(*dword_905B84 + 4);
		auto len = GetStringLength(text);
		x += 6 * len;
		x -= (int16_t)std::floor(((float)GlyphManager::Get().GetGlyphAdvance() / 7.0) * len);
		x -= 2;
	}

	// Translate strings passed directly as pointers into offset 0 data (dialogue)
	if ((uint32_t)text > 0x803580 && (uint32_t)text <= 0x803580 + 0xFFFF) {
		// Find offset into index table
		int i;

		for (i = 0; i < *(uint16_t*)0x803580 / 2; ++i) {
			if (0x803580 + ((uint16_t*)0x803580)[i] == (uint32_t)text)
				break;
		}

		auto areaId = *(uint8_t*)0x904EFC;

		auto& txtMgr = TextManager::Get();

		if (!txtMgr.DialogueFileIsLoaded(areaId))
			txtMgr.LoadDialogue(areaId);

		if (txtMgr.HasDialogue(areaId, i)) {
			auto& translatedText = txtMgr.GetDialogue(areaId, i);

			return DrawString(x, y, paletteIdx, len, translatedText.c_str());
		}
	}

	return DrawString.Original(x, y, paletteIdx, len, text);
};


FuncHook<decltype(DrawStringSmall)> DrawStringSmallHook = [](auto x, auto y, auto a3, auto len, auto text) {
	if (*(uint32_t*)text == 0x600DBEEF)
		return DrawStringSmall(x, y, a3, len, *(const char**)&text[4]);

	auto v5 = text;
	auto v6 = 2 * (a3 >> 4);
	auto v7 = 16 * (a3 & 0xF);
	auto v8 = ((uint8_t*)0x65F5A8)[v6];
	auto _x = x;
	auto v16 = ((uint8_t*)0x65F5A9)[v6];
	auto v10 = *text;
	auto v15 = v8;
	uint16_t v13;

	auto spacing = (uint8_t)(GlyphManager::Get().GetGlyphAdvance() / 3.5 * (10.0 / 12.0));

	for (auto i = (v7 >> 4) & 0x3F | 0x7800; v10; --len) {
		if (!len)
			break;

		if (v10) {
			if (v10 == 1) {
				_x = x;
				y += 9;
			} else {
				if (v10 != 32) {
					if (v10 >= 0)
						v13 = (unsigned __int8)v10 - '!';
					else
						v13 = *(unsigned __int8*)++v5 + (((unsigned __int8)v10 + 128) << 8);

					(*(DrawCommand_UnkIcon0**)0x7E0670)->paletteId = v13;
					sub_516D50(_x, y - 2, 10, 10 - v16, v15, v16, i);
				}

				_x += spacing;
			}
		}

		v10 = *++v5;
	}

	return v5 + 1;
};


FuncHook<decltype(DrawNumTiny)> DrawNumTinyHook = [](auto x, auto y, auto a3, auto a4) {
	char text[5];

	if (a4 == 0xFFFF)
		sprintf_s(text, "?");
	else
		sprintf_s(text, "%4d", a4);

	auto len = GetStringLength(text);

	auto v5 = text;
	auto v6 = 2 * (a3 >> 4);
	auto v7 = 16 * (a3 & 0xF);
	auto v8 = ((uint8_t*)0x65F5A8)[v6];
	auto _x = x;
	auto v16 = ((uint8_t*)0x65F5A9)[v6];
	auto v10 = *text;
	auto v15 = v8;
	uint16_t v13;

	auto spacing = (uint8_t)(GlyphManager::Get().GetGlyphAdvance() / 3.5 * (8.0 / 12.0));

	for (auto i = (v7 >> 4) & 0x3F | 0x7800; v10; --len) {
		if (!len)
			break;

		if (v10) {
			if (v10 == 1) {
				_x = x;
				y += 9;
			} else {
				if (v10 != 32) {
					if (v10 >= 0)
						v13 = (unsigned __int8)v10 - '!';
					else
						v13 = *(unsigned __int8*)++v5 + (((unsigned __int8)v10 + 128) << 8);

					(*(DrawCommand_UnkIcon0**)0x7E0670)->paletteId = v13;
					sub_516D50(_x, y - 2, 8, 8 - v16, v15, v16, i);
				}

				_x += spacing;
			}
		}

		v10 = *++v5;
	}
};


FuncHook<decltype(DrawStringNumFont)> DrawStringNumFontHook = [](auto x, auto y, auto paletteIdx, auto text) {
	DrawString(x, y - 2, paletteIdx, GetStringLength(text), text);
};


Func<0x5A2900, HRESULT, int /* a1 */> sub_5A2900;
FuncHook<decltype(sub_5A2900)> sub_5A2900Hook = [](auto a1) {
	g_IDirect3DDevice3->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFN_LINEAR);
	g_IDirect3DDevice3->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_LINEAR);

	return sub_5A2900.Original(a1);
};


export void EnableRenderHooks() {
	EnableHook(DrawString, DrawStringHook);
	EnableHook(DrawStringNumFont, DrawStringNumFontHook);
	EnableHook(DrawStringSmall, DrawStringSmallHook);
	EnableHook(DrawNumTiny, DrawNumTinyHook);
	EnableHook(sub_5A2900, sub_5A2900Hook);
}
module;

#include <cstdint>
#include <cstdio>
#include <d3d.h>

export module bof3ext.hooks:render;

import bof3ext.helpers;
import bof3ext.math;
import bof3ext.configManager;
import bof3ext.glyphManager;
import bof3ext.textManager;

import bof3.render;
import bof3.text;
import bof3.texture;
import bof3.window;

import std;


template<size_t S>
void Fix2DCoordinates(D3DTLVERTEX(&verts)[S]) {
	if (std::abs(0.01 - verts[0].sz) > std::numeric_limits<float>::epsilon())
		return;

	for (int i = 0; i < S; ++i) {
		verts[i].sx -= 0.5;
		verts[i].sy -= 0.5;
	}
}


auto DrawStringHook(auto x, auto y, auto paletteIdx, auto len, auto text) {
	if (text[0] < 0) {
		auto s = std::stacktrace::current(1, 1);
		const auto handle = (uintptr_t)s[0].native_handle();

		if (handle >= 0x401000 && handle <= 0x5C4000)
			LogDebug("DrawString: %s (%p)\n", text, handle);
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

			return DrawString.Original(x, y, paletteIdx, len, translatedText.c_str());
		}
	}

	return DrawString.Original(x, y, paletteIdx, len, text);
}

auto DrawStringSmallHook(auto x, auto y, auto a3, auto len, auto text) {
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

	auto advance = (int)std::ceil(GlyphManager::Get().GetScaledGlyphAdvance() * SMALL_TEXT_SCALE);

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
						v13 = (unsigned __int8)v10;
					else
						v13 = *(unsigned __int8*)++v5 + (((unsigned __int8)v10) << 8);

					((GpuCommand_TextGlyph*)*g_GpuCommands)->charCode = v13;
					sub_516D50(_x, y - 2, 10, 10 - v16, v15, v16, i);
				}

				_x += advance;
			}
		}

		v10 = *++v5;
	}

	return v5 + 1;
}

auto DrawStringLargeHook(auto x, auto y, auto paletteId, auto text) {
	auto _x = x;
	auto _y = y + 1;

	char v8;

	auto advance = (int)std::ceil(GlyphManager::Get().GetScaledGlyphAdvance() * (16.0 / 12.0));

	do {
		auto c = *text;

		if (*text == '\n') {
			_x = x;
			_y += 12;
		} else if (c != ' ') {
			if (c) {
				((GpuCommand_TextGlyph*)*g_GpuCommands)->charCode = c;
				sub_516D50(_x, y - 1, 16, 16, 0, 0, ((16 * paletteId) >> 4) & 0x3F | 0x7800);
			}
		}

		v8 = text[1];
		_x += advance;
		++text;
	} while (v8);
}

auto DrawNumTinyHook(auto x, auto y, auto a3, auto a4) {
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

	auto advance = (int)std::ceil(GlyphManager::Get().GetScaledGlyphAdvance() * (8.0 / 12.0));

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
						v13 = (unsigned __int8)v10;
					else
						v13 = *(unsigned __int8*)++v5 + (((unsigned __int8)v10) << 8);

					((GpuCommand_TextGlyph*)*g_GpuCommands)->charCode = v13;
					sub_516D50(_x, y - 2, 8, 8 - v16, v15, v16, i);
				}

				_x += advance;
			}
		}

		v10 = *++v5;
	}
}

auto DrawStringNumFontHook(auto x, auto y, auto paletteIdx, auto text) {
	DrawStringSmall(x, y + 2, paletteIdx, GetStringLength(text), text);
}

Func<0x5A2900, void, GpuCommand_TextGlyph* /* gpuCmd */> ProcessGpuCommand_TextGlyph;
auto ProcessGpuCommand_TextGlyphHook(auto gpuCmd) {
	g_IDirect3DDevice3->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFN_LINEAR);
	g_IDirect3DDevice3->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_LINEAR);

	return ProcessGpuCommand_TextGlyph.Original(gpuCmd);
}

Func<0x5A0C40, void, GpuCommand_TexturedQuad* /* gpuCmd */> ProcessGpuCommand_TexturedQuad;
auto ProcessGpuCommand_TexturedQuadHook(auto gpuCmd) {
	D3DCOLOR colour, specular;

	GetD3DCOLOR(gpuCmd->colour.r, gpuCmd->colour.g, gpuCmd->colour.b, gpuCmd->value, gpuCmd->texturePage, &colour, &specular);

	D3DTLVERTEX v[4]{ 0 };

	v[0].sx = gpuCmd->v1.x * *g_RenderScaleX;
	v[0].sy = gpuCmd->v1.y * *g_RenderScaleY;
	v[0].sz = gpuCmd->v1.z;
	v[0].tu = gpuCmd->t1.x / 256.0;
	v[0].tv = gpuCmd->t1.y / 256.0;
	v[0].color = colour;
	v[0].specular = specular;
	v[0].rhw = 0.1 / gpuCmd->v1.z;

	v[1].sx = gpuCmd->v2.x * *g_RenderScaleX;
	v[1].sy = gpuCmd->v2.y * *g_RenderScaleY;
	v[1].sz = gpuCmd->v2.z;
	v[1].tu = (gpuCmd->t2.x + 1) / 256.0;
	v[1].tv = gpuCmd->t2.y / 256.0;
	v[1].color = colour;
	v[1].specular = specular;
	v[1].rhw = 0.1 / gpuCmd->v2.z;

	v[2].sx = gpuCmd->v3.x * *g_RenderScaleX;
	v[2].sy = gpuCmd->v3.y * *g_RenderScaleY;
	v[2].sz = gpuCmd->v3.z;
	v[2].tu = gpuCmd->t3.x / 256.0;
	v[2].tv = (gpuCmd->t3.y + 1) / 256.0;
	v[2].color = colour;
	v[2].specular = specular;
	v[2].rhw = 0.1 / gpuCmd->v3.z;

	v[3].sx = gpuCmd->v4.x * *g_RenderScaleX;
	v[3].sy = gpuCmd->v4.y * *g_RenderScaleY;
	v[3].sz = gpuCmd->v4.z;
	v[3].tu = (gpuCmd->t4.x + 1) / 256.0;
	v[3].tv = (gpuCmd->t4.y + 1) / 256.0;
	v[3].color = colour;
	v[3].specular = specular;
	v[3].rhw = 0.1 / gpuCmd->v4.z;

	Fix2DCoordinates(v);

	SetTexture(gpuCmd->texturePage, gpuCmd->palette);

	SetD3DRenderState_AlphaBlend(gpuCmd->value, gpuCmd->texturePage);
	SetD3DShadeMode(D3DSHADE_FLAT);

	g_IDirect3DDevice3->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, v, 4, 0);
}

Func<0x5A17A0, void, GpuCommand_Line* /* gpuCmd */> ProcessGpuCommand_Line;
auto ProcessGpuCommand_LineHook(auto gpuCmd) {
	D3DCOLOR colour;

	GetD3DCOLOR(gpuCmd->colour.r, gpuCmd->colour.g, gpuCmd->colour.b, gpuCmd->value, g_DrawEnv->tpage, &colour, 0);

	auto xDelta = gpuCmd->v2.x - gpuCmd->v1.x;
	auto yDelta = gpuCmd->v2.y - gpuCmd->v1.y;

	auto theta = std::atan2(yDelta, xDelta)/* * 180 / std::numbers::pi*/;
	auto distance = std::sqrt(xDelta * xDelta + yDelta * yDelta);

	D3DTLVERTEX v[4]{ 0 };

	if (gpuCmd->v1.y == gpuCmd->v2.y) {			// Horizontal line
		auto v1 = (gpuCmd->v1.x < gpuCmd->v2.x) ? gpuCmd->v1 : gpuCmd->v2;

		v[0].sx = v[2].sx = v1.x * *g_RenderScaleX;
		v[1].sx = v[3].sx = (v1.x + distance) * *g_RenderScaleX;

		v[0].sy = v[1].sy = v1.y * *g_RenderScaleY;
		v[2].sy = v[3].sy = (v1.y + 1) * *g_RenderScaleY;
	} else if (gpuCmd->v1.x == gpuCmd->v2.x) {	// Vertical line
		auto v1 = (gpuCmd->v1.y < gpuCmd->v2.y) ? gpuCmd->v1 : gpuCmd->v2;

		v[0].sx = v[2].sx = v1.x * *g_RenderScaleX;
		v[1].sx = v[3].sx = (v1.x + 1) * *g_RenderScaleX;

		v[0].sy = v[1].sy = v1.y * *g_RenderScaleY;
		v[2].sy = v[3].sy = (v1.y + distance) * *g_RenderScaleY;
	} else {
		ProcessGpuCommand_Line.Original(gpuCmd);	// TODO: Handle angled lines
	}

	for (int i = 0; i < 4; ++i) {
		v[i].sz = gpuCmd->v1.z;
		v[i].color = colour;
		v[i].rhw = 0.1 / gpuCmd->v1.z;
	}

	//Fix2DCoordinates(v);

	g_IDirect3DDevice3->SetTexture(0, nullptr);

	SetD3DRenderState_AlphaBlend(gpuCmd->value, g_DrawEnv->tpage);
	SetD3DShadeMode(D3DSHADE_FLAT);

	g_IDirect3DDevice3->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, v, 4, 0);
}

Func<0x5A2300, void, GpuCommand_TexturedRectWH* /* gpuCmd */> ProcessGpuCommand_TexturedRectWH;
auto ProcessGpuCommand_TexturedRectWHHook(auto gpuCmd) {
	D3DCOLOR colour, specular;

	GetD3DCOLOR(gpuCmd->colour.r, gpuCmd->colour.g, gpuCmd->colour.b, gpuCmd->value, g_DrawEnv->tpage, &colour, &specular);

	D3DTLVERTEX v[4]{ 0 };

	v[0].sx = gpuCmd->v1.x * *g_RenderScaleX;
	v[0].sy = gpuCmd->v1.y * *g_RenderScaleY;
	v[0].sz = gpuCmd->v1.z;
	v[0].tu = gpuCmd->t1.x / 256.0;
	v[0].tv = gpuCmd->t1.y / 256.0;
	v[0].color = colour;
	v[0].specular = specular;
	v[0].rhw = 0.1 / gpuCmd->v1.z;

	v[1].sx = (gpuCmd->v1.x + gpuCmd->size.x) * *g_RenderScaleX;
	v[1].sy = gpuCmd->v1.y * *g_RenderScaleY;
	v[1].sz = gpuCmd->v1.z;
	v[1].tu = (gpuCmd->t1.x + gpuCmd->size.x) / 256.0;
	v[1].tv = gpuCmd->t1.y / 256.0;
	v[1].color = colour;
	v[1].specular = specular;
	v[1].rhw = 0.1 / gpuCmd->v1.z;

	v[2].sx = gpuCmd->v1.x * *g_RenderScaleX;
	v[2].sy = (gpuCmd->v1.y + gpuCmd->size.y) * *g_RenderScaleY;
	v[2].sz = gpuCmd->v1.z;
	v[2].tu = gpuCmd->t1.x / 256.0;
	v[2].tv = (gpuCmd->t1.y + gpuCmd->size.y) / 256.0;
	v[2].color = colour;
	v[2].specular = specular;
	v[2].rhw = 0.1 / gpuCmd->v1.z;

	v[3].sx = (gpuCmd->v1.x + gpuCmd->size.x) * *g_RenderScaleX;
	v[3].sy = (gpuCmd->v1.y + gpuCmd->size.y) * *g_RenderScaleY;
	v[3].sz = gpuCmd->v1.z;
	v[3].tu = (gpuCmd->t1.x + gpuCmd->size.x) / 256.0;
	v[3].tv = (gpuCmd->t1.y + gpuCmd->size.y) / 256.0;
	v[3].color = colour;
	v[3].specular = specular;
	v[3].rhw = 0.1 / gpuCmd->v1.z;

	Fix2DCoordinates(v);

	SetTexture(g_DrawEnv->tpage, gpuCmd->palette);

	SetD3DRenderState_AlphaBlend(gpuCmd->value, g_DrawEnv->tpage);
	SetD3DShadeMode(D3DSHADE_FLAT);

	g_IDirect3DDevice3->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, v, 4, 0);
}

Func<0x5A2520, void, GpuCommand_TexturedRect8* /* gpuCmd */> ProcessGpuCommand_TexturedRect8;
auto ProcessGpuCommand_TexturedRect8Hook(auto gpuCmd) {
	D3DCOLOR colour, specular;

	GetD3DCOLOR(gpuCmd->colour.r, gpuCmd->colour.g, gpuCmd->colour.b, gpuCmd->value, g_DrawEnv->tpage, &colour, &specular);

	D3DTLVERTEX v[4]{ 0 };

	v[0].sx = gpuCmd->v1.x * *g_RenderScaleX;
	v[0].sy = gpuCmd->v1.y * *g_RenderScaleY;
	v[0].sz = gpuCmd->v1.z;
	v[0].tu = gpuCmd->t1.x / 256.0;
	v[0].tv = gpuCmd->t1.y / 256.0;
	v[0].color = colour;
	v[0].specular = specular;
	v[0].rhw = 0.1 / gpuCmd->v1.z;

	v[1].sx = (gpuCmd->v1.x + 8) * *g_RenderScaleX;
	v[1].sy = gpuCmd->v1.y * *g_RenderScaleY;
	v[1].sz = gpuCmd->v1.z;
	v[1].tu = (gpuCmd->t1.x + 8) / 256.0;
	v[1].tv = gpuCmd->t1.y / 256.0;
	v[1].color = colour;
	v[1].specular = specular;
	v[1].rhw = 0.1 / gpuCmd->v1.z;

	v[2].sx = gpuCmd->v1.x * *g_RenderScaleX;
	v[2].sy = (gpuCmd->v1.y + 8) * *g_RenderScaleY;
	v[2].sz = gpuCmd->v1.z;
	v[2].tu = gpuCmd->t1.x / 256.0;
	v[2].tv = (gpuCmd->t1.y + 8) / 256.0;
	v[2].color = colour;
	v[2].specular = specular;
	v[2].rhw = 0.1 / gpuCmd->v1.z;

	v[3].sx = (gpuCmd->v1.x + 8) * *g_RenderScaleX;
	v[3].sy = (gpuCmd->v1.y + 8) * *g_RenderScaleY;
	v[3].sz = gpuCmd->v1.z;
	v[3].tu = (gpuCmd->t1.x + 8) / 256.0;
	v[3].tv = (gpuCmd->t1.y + 8) / 256.0;
	v[3].color = colour;
	v[3].specular = specular;
	v[3].rhw = 0.1 / gpuCmd->v1.z;

	Fix2DCoordinates(v);

	SetTexture(g_DrawEnv->tpage, gpuCmd->palette);

	SetD3DRenderState_AlphaBlend(gpuCmd->value, g_DrawEnv->tpage);
	SetD3DShadeMode(D3DSHADE_FLAT);

	g_IDirect3DDevice3->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, v, 4, 0);
}

Func<0x5A2710, void, GpuCommand_TexturedRect16* /* gpuCmd */> ProcessGpuCommand_TexturedRect16;
auto ProcessGpuCommand_TexturedRect16Hook(auto gpuCmd) {
	D3DCOLOR colour, specular;

	GetD3DCOLOR(gpuCmd->colour.r, gpuCmd->colour.g, gpuCmd->colour.b, gpuCmd->value, g_DrawEnv->tpage, &colour, &specular);

	D3DTLVERTEX v[4]{ 0 };

	v[0].sx = gpuCmd->v1.x * *g_RenderScaleX;
	v[0].sy = gpuCmd->v1.y * *g_RenderScaleY;
	v[0].sz = gpuCmd->v1.z;
	v[0].tu = gpuCmd->t1.x / 256.0;
	v[0].tv = gpuCmd->t1.y / 256.0;
	v[0].color = colour;
	v[0].specular = specular;
	v[0].rhw = 0.1 / gpuCmd->v1.z;

	v[1].sx = (gpuCmd->v1.x + 16) * *g_RenderScaleX;
	v[1].sy = gpuCmd->v1.y * *g_RenderScaleY;
	v[1].sz = gpuCmd->v1.z;
	v[1].tu = (gpuCmd->t1.x + 16) / 256.0;
	v[1].tv = gpuCmd->t1.y / 256.0;
	v[1].color = colour;
	v[1].specular = specular;
	v[1].rhw = 0.1 / gpuCmd->v1.z;

	v[2].sx = gpuCmd->v1.x * *g_RenderScaleX;
	v[2].sy = (gpuCmd->v1.y + 16) * *g_RenderScaleY;
	v[2].sz = gpuCmd->v1.z;
	v[2].tu = gpuCmd->t1.x / 256.0;
	v[2].tv = (gpuCmd->t1.y + 16) / 256.0;
	v[2].color = colour;
	v[2].specular = specular;
	v[2].rhw = 0.1 / gpuCmd->v1.z;

	v[3].sx = (gpuCmd->v1.x + 16) * *g_RenderScaleX;
	v[3].sy = (gpuCmd->v1.y + 16) * *g_RenderScaleY;
	v[3].sz = gpuCmd->v1.z;
	v[3].tu = (gpuCmd->t1.x + 16) / 256.0;
	v[3].tv = (gpuCmd->t1.y + 16) / 256.0;
	v[3].color = colour;
	v[3].specular = specular;
	v[3].rhw = 0.1 / gpuCmd->v1.z;

	Fix2DCoordinates(v);

	SetTexture(g_DrawEnv->tpage, gpuCmd->palette);

	SetD3DRenderState_AlphaBlend(gpuCmd->value, g_DrawEnv->tpage);
	SetD3DShadeMode(D3DSHADE_FLAT);

	g_IDirect3DDevice3->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, v, 4, 0);
}

Func<0x5A2EB0, void, GpuCommand_Sprite* /* gpuCmd */> ProcessGpuCommand_Sprite;
auto ProcessGpuCommand_SpriteHook(auto gpuCmd) {
	gpuCmd->word18 = 1;
	gpuCmd->word1A = 1;
	gpuCmd->word1E = 1;

	ProcessGpuCommand_Sprite.Original(gpuCmd);
}

Func<0x5A7AE0, void, int /* x */, int /* y */> SetGeomOffset;
auto SetGeomOffsetHook(auto x, auto y) {
	LogDebug("SetGeomOffset: %i, %i\n", x, y);

	auto width = ConfigManager::Get().GetScaledRenderWidth();
	auto diff = width - 320;

	SetGeomOffset.Original(x + diff / 2, y);
}

Func<0x5A7910, void, DRAWENV* /* env */, short /* x */, short /* y */, short /* w */, short /* h */> SetDefDrawEnv;
auto SetDefDrawEnvHook(auto env, auto x, auto y, auto w, auto h) {
	LogDebug("SetDefDrawEnv: %i, %i, %i, %i\n", x, y, w, h);

	auto width = ConfigManager::Get().GetScaledRenderWidth();
	auto diff = width - w;

	SetDefDrawEnv.Original(env, x + diff, y, width, h);
}

Func<0x5A78E0, void, DISPENV* /* disp */, short /* x */, short /* y */, short /* w */, short /* h */> SetDefDispEnv;
auto SetDefDispEnvHook(auto disp, auto x, auto y, auto w, auto h) {
	LogDebug("SetDefDispEnv: %i, %i, %i, %i\n", x, y, w, h);

	auto width = ConfigManager::Get().GetScaledRenderWidth();
	auto diff = width - w;

	SetDefDispEnv.Original(disp, x + diff, y, width, h);
}


export void EnableRenderHooks() {
	EnableHook(DrawString, DrawStringHook);
	EnableHook(DrawStringNumFont, DrawStringNumFontHook);
	EnableHook(DrawStringSmall, DrawStringSmallHook);
	EnableHook(DrawStringLarge, DrawStringLargeHook);
	EnableHook(DrawNumTiny, DrawNumTinyHook);
	EnableHook(ProcessGpuCommand_TextGlyph, ProcessGpuCommand_TextGlyphHook);
	EnableHook(ProcessGpuCommand_TexturedQuad, ProcessGpuCommand_TexturedQuadHook);
	EnableHook(ProcessGpuCommand_Line, ProcessGpuCommand_LineHook);
	EnableHook(ProcessGpuCommand_TexturedRectWH, ProcessGpuCommand_TexturedRectWHHook);
	EnableHook(ProcessGpuCommand_TexturedRect8, ProcessGpuCommand_TexturedRect8Hook);
	EnableHook(ProcessGpuCommand_TexturedRect16, ProcessGpuCommand_TexturedRect16Hook);
	//EnableHook(ProcessGpuCommand_Sprite, ProcessGpuCommand_SpriteHook);
	EnableHook(SetGeomOffset, SetGeomOffsetHook);
	EnableHook(SetDefDrawEnv, SetDefDrawEnvHook);
	EnableHook(SetDefDispEnv, SetDefDispEnvHook);
	//EnableHook(WndProc_Game, WndProc_GameHook);

	// Set render size
	const auto& cfgMgr = ConfigManager::Get();
	*(uint32_t*)0x66B710 = cfgMgr.GetRenderWidth();
	*(uint32_t*)0x66B714 = cfgMgr.GetRenderHeight();

	//WriteProtectedMemory(0x5A2B71, (uint8_t)3);	// Enable alpha blending for text
	//WriteProtectedMemory(0x5A2B8D, (uint8_t)1);	// Enable alpha blending for text
}
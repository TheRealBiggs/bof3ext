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

			return DrawString::Original(x, y, paletteIdx, len, translatedText.c_str());
		}
	}

	return DrawString::Original(x, y, paletteIdx, len, text);
}

auto DrawStringSmallHook(auto x, auto y, auto a3, auto len, auto text) {
	if (*(uint32_t*)text == 0x600DBEEF)
		return DrawStringSmall::Call(x, y, a3, len, *(const char**)&text[4]);

	auto v5 = text;
	auto v6 = 2 * (a3 >> 4);
	auto v7 = 16 * (a3 & 0xF);
	auto v8 = ((uint8_t*)0x65F5A8)[v6];
	auto _x = x;
	auto v16 = ((uint8_t*)0x65F5A9)[v6];
	auto v10 = *text;
	auto v15 = v8;
	uint16_t v13;

	auto advance = (int)std::floor(GlyphManager::Get().GetScaledGlyphAdvance() * SMALL_TEXT_SCALE);

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

					g_GpuPrims::Get<GpuPrim_TextGlyph>()->charCode = v13;
					sub_516D50::Call(_x, y - 2, 10, 10 - v16, v15, v16, i);
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

	auto advance = (int)std::floor(GlyphManager::Get().GetScaledGlyphAdvance() * (16.0 / 12.0));

	do {
		auto c = *text;

		if (*text == '\n') {
			_x = x;
			_y += 12;
		} else if (c != ' ') {
			if (c) {
				g_GpuPrims::Get<GpuPrim_TextGlyph>()->charCode = c;
				sub_516D50::Call(_x, y - 1, 16, 16, 0, 0, ((16 * paletteId) >> 4) & 0x3F | 0x7800);
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

	auto len = GetStringLength::Call(text);

	auto v5 = text;
	auto v6 = 2 * (a3 >> 4);
	auto v7 = 16 * (a3 & 0xF);
	auto v8 = ((uint8_t*)0x65F5A8)[v6];
	auto _x = x;
	auto v16 = ((uint8_t*)0x65F5A9)[v6];
	auto v10 = *text;
	auto v15 = v8;
	uint16_t v13;

	auto advance = (int)std::floor(GlyphManager::Get().GetScaledGlyphAdvance() * (8.0 / 12.0));

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

					g_GpuPrims::Get<GpuPrim_TextGlyph>()->charCode = v13;
					sub_516D50::Call(_x, y - 2, 8, 8 - v16, v15, v16, i);
				}

				_x += advance;
			}
		}

		v10 = *++v5;
	}
}

auto DrawStringNumFontHook(auto x, auto y, auto paletteIdx, auto text) {
	DrawStringSmall::Call(x, y + 2, paletteIdx, GetStringLength::Call(text), text);
}

typedef Func<0x5A2900, void,
	GpuPrim_TextGlyph*	// prim
> ProcessGpuPrim_TextGlyph;
auto ProcessGpuPrim_TextGlyphHook(auto prim) {
	g_IDirect3DDevice3::Get()->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFN_LINEAR);
	g_IDirect3DDevice3::Get()->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_LINEAR);

	return ProcessGpuPrim_TextGlyph::Original(prim);
}

typedef Func<0x5A0C40, void,
	GpuPrim_TexturedQuad*	// prim
> ProcessGpuPrim_TexturedQuad;
auto ProcessGpuPrim_TexturedQuadHook(auto prim) {
	D3DCOLOR colour, specular;

	GetD3DCOLOR::Call(prim->colour.r, prim->colour.g, prim->colour.b, prim->value, prim->texturePage, &colour, &specular);

	float sx = g_RenderScaleX::Get();
	float sy = g_RenderScaleY::Get();

	D3DTLVERTEX v[4]{ 0 };

	v[0].sx = prim->v1.x * sx;
	v[0].sy = prim->v1.y * sy;
	v[0].sz = prim->v1.z;
	v[0].tu = prim->t1.x / 256.0f;
	v[0].tv = prim->t1.y / 256.0f;
	v[0].color = colour;
	v[0].specular = specular;
	v[0].rhw = 0.1f / prim->v1.z;

	v[1].sx = prim->v2.x * sx;
	v[1].sy = prim->v2.y * sy;
	v[1].sz = prim->v2.z;
	v[1].tu = (prim->t2.x + 1) / 256.0f;
	v[1].tv = prim->t2.y / 256.0f;
	v[1].color = colour;
	v[1].specular = specular;
	v[1].rhw = 0.1f / prim->v2.z;

	v[2].sx = prim->v3.x * sx;
	v[2].sy = prim->v3.y * sy;
	v[2].sz = prim->v3.z;
	v[2].tu = prim->t3.x / 256.0f;
	v[2].tv = (prim->t3.y + 1) / 256.0f;
	v[2].color = colour;
	v[2].specular = specular;
	v[2].rhw = 0.1f / prim->v3.z;

	v[3].sx = prim->v4.x * sx;
	v[3].sy = prim->v4.y * sy;
	v[3].sz = prim->v4.z;
	v[3].tu = (prim->t4.x + 1) / 256.0f;
	v[3].tv = (prim->t4.y + 1) / 256.0f;
	v[3].color = colour;
	v[3].specular = specular;
	v[3].rhw = 0.1f / prim->v4.z;

	Fix2DCoordinates(v);

	SetTexture::Call(prim->texturePage, prim->palette);

	SetD3DRenderState_AlphaBlend::Call(prim->value, prim->texturePage);
	SetD3DShadeMode::Call(D3DSHADE_FLAT);

	g_IDirect3DDevice3::Get()->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, v, 4, 0);
}

typedef Func<0x5A17A0, void,
	GpuPrim_Line*	// prim
> ProcessGpuPrim_Line;
auto ProcessGpuPrim_LineHook(auto prim) {
	D3DCOLOR colour;

	GetD3DCOLOR::Call(prim->colour.r, prim->colour.g, prim->colour.b, prim->value, g_DrawEnv::Get().tpage, &colour, 0);

	auto xDelta = prim->v2.x - prim->v1.x;
	auto yDelta = prim->v2.y - prim->v1.y;

	auto theta = std::atan2(yDelta, xDelta)/* * 180 / std::numbers::pi*/;
	auto distance = std::sqrt(xDelta * xDelta + yDelta * yDelta);

	float sx = g_RenderScaleX::Get();
	float sy = g_RenderScaleY::Get();

	D3DTLVERTEX v[4]{ 0 };

	if (prim->v1.y == prim->v2.y) {			// Horizontal line
		auto v1 = (prim->v1.x < prim->v2.x) ? prim->v1 : prim->v2;

		v[0].sx = v[2].sx = v1.x * sx;
		v[1].sx = v[3].sx = (v1.x + distance) * sx;

		v[0].sy = v[1].sy = v1.y * sy;
		v[2].sy = v[3].sy = (v1.y + 1) * sy;
	} else if (prim->v1.x == prim->v2.x) {	// Vertical line
		auto v1 = (prim->v1.y < prim->v2.y) ? prim->v1 : prim->v2;

		v[0].sx = v[2].sx = v1.x * sx;
		v[1].sx = v[3].sx = (v1.x + 1) * sx;

		v[0].sy = v[1].sy = v1.y * sy;
		v[2].sy = v[3].sy = (v1.y + distance) * sy;
	} else {
		ProcessGpuPrim_Line::Original(prim);	// TODO: Handle angled lines
	}

	for (int i = 0; i < 4; ++i) {
		v[i].sz = prim->v1.z;
		v[i].color = colour;
		v[i].rhw = 0.1f / prim->v1.z;
	}

	//Fix2DCoordinates(v);

	g_IDirect3DDevice3::Get()->SetTexture(0, nullptr);

	SetD3DRenderState_AlphaBlend::Call(prim->value, g_DrawEnv::Get().tpage);
	SetD3DShadeMode::Call(D3DSHADE_FLAT);

	g_IDirect3DDevice3::Get()->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, v, 4, 0);
}

typedef Func<0x5A2300, void,
	GpuPrim_TexturedRectWH*	// prim
> ProcessGpuPrim_TexturedRectWH;
auto ProcessGpuPrim_TexturedRectWHHook(auto prim) {
	D3DCOLOR colour, specular;

	GetD3DCOLOR::Call(prim->colour.r, prim->colour.g, prim->colour.b, prim->value, g_DrawEnv::Get().tpage, &colour, &specular);

	float sx = g_RenderScaleX::Get();
	float sy = g_RenderScaleY::Get();

	D3DTLVERTEX v[4]{ 0 };

	v[0].sx = prim->v1.x * sx;
	v[0].sy = prim->v1.y * sy;
	v[0].sz = prim->v1.z;
	v[0].tu = prim->t1.x / 256.0f;
	v[0].tv = prim->t1.y / 256.0f;
	v[0].color = colour;
	v[0].specular = specular;
	v[0].rhw = 0.1f / prim->v1.z;

	v[1].sx = (prim->v1.x + prim->size.x) * sx;
	v[1].sy = prim->v1.y * sy;
	v[1].sz = prim->v1.z;
	v[1].tu = (prim->t1.x + prim->size.x) / 256.0f;
	v[1].tv = prim->t1.y / 256.0f;
	v[1].color = colour;
	v[1].specular = specular;
	v[1].rhw = 0.1f / prim->v1.z;

	v[2].sx = prim->v1.x * sx;
	v[2].sy = (prim->v1.y + prim->size.y) * sy;
	v[2].sz = prim->v1.z;
	v[2].tu = prim->t1.x / 256.0f;
	v[2].tv = (prim->t1.y + prim->size.y) / 256.0f;
	v[2].color = colour;
	v[2].specular = specular;
	v[2].rhw = 0.1f / prim->v1.z;

	v[3].sx = (prim->v1.x + prim->size.x) * sx;
	v[3].sy = (prim->v1.y + prim->size.y) * sy;
	v[3].sz = prim->v1.z;
	v[3].tu = (prim->t1.x + prim->size.x) / 256.0f;
	v[3].tv = (prim->t1.y + prim->size.y) / 256.0f;
	v[3].color = colour;
	v[3].specular = specular;
	v[3].rhw = 0.1f / prim->v1.z;

	Fix2DCoordinates(v);

	SetTexture::Call(g_DrawEnv::Get().tpage, prim->palette);

	SetD3DRenderState_AlphaBlend::Call(prim->value, g_DrawEnv::Get().tpage);
	SetD3DShadeMode::Call(D3DSHADE_FLAT);

	g_IDirect3DDevice3::Get()->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, v, 4, 0);
}

typedef Func<0x5A2520, void,
	GpuPrim_TexturedRect8*	// prim
> ProcessGpuPrim_TexturedRect8;
auto ProcessGpuPrim_TexturedRect8Hook(auto prim) {
	D3DCOLOR colour, specular;

	GetD3DCOLOR::Call(prim->colour.r, prim->colour.g, prim->colour.b, prim->value, g_DrawEnv::Get().tpage, &colour, &specular);

	float sx = g_RenderScaleX::Get();
	float sy = g_RenderScaleY::Get();

	D3DTLVERTEX v[4]{ 0 };

	v[0].sx = prim->v1.x * sx;
	v[0].sy = prim->v1.y * sy;
	v[0].sz = prim->v1.z;
	v[0].tu = prim->t1.x / 256.0f;
	v[0].tv = prim->t1.y / 256.0f;
	v[0].color = colour;
	v[0].specular = specular;
	v[0].rhw = 0.1f / prim->v1.z;

	v[1].sx = (prim->v1.x + 8) * sx;
	v[1].sy = prim->v1.y * sy;
	v[1].sz = prim->v1.z;
	v[1].tu = (prim->t1.x + 8) / 256.0f;
	v[1].tv = prim->t1.y / 256.0f;
	v[1].color = colour;
	v[1].specular = specular;
	v[1].rhw = 0.1f / prim->v1.z;

	v[2].sx = prim->v1.x * sx;
	v[2].sy = (prim->v1.y + 8) * sy;
	v[2].sz = prim->v1.z;
	v[2].tu = prim->t1.x / 256.0f;
	v[2].tv = (prim->t1.y + 8) / 256.0f;
	v[2].color = colour;
	v[2].specular = specular;
	v[2].rhw = 0.1f / prim->v1.z;

	v[3].sx = (prim->v1.x + 8) * sx;
	v[3].sy = (prim->v1.y + 8) * sy;
	v[3].sz = prim->v1.z;
	v[3].tu = (prim->t1.x + 8) / 256.0f;
	v[3].tv = (prim->t1.y + 8) / 256.0f;
	v[3].color = colour;
	v[3].specular = specular;
	v[3].rhw = 0.1f / prim->v1.z;

	Fix2DCoordinates(v);

	SetTexture::Call(g_DrawEnv::Get().tpage, prim->palette);

	SetD3DRenderState_AlphaBlend::Call(prim->value, g_DrawEnv::Get().tpage);
	SetD3DShadeMode::Call(D3DSHADE_FLAT);

	g_IDirect3DDevice3::Get()->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, v, 4, 0);
}

typedef Func<0x5A2710, void,
	GpuPrim_TexturedRect16*	// prim
> ProcessGpuPrim_TexturedRect16;
auto ProcessGpuPrim_TexturedRect16Hook(auto prim) {
	D3DCOLOR colour, specular;

	GetD3DCOLOR::Call(prim->colour.r, prim->colour.g, prim->colour.b, prim->value, g_DrawEnv::Get().tpage, &colour, &specular);

	float sx = g_RenderScaleX::Get();
	float sy = g_RenderScaleY::Get();

	D3DTLVERTEX v[4]{ 0 };

	v[0].sx = prim->v1.x * sx;
	v[0].sy = prim->v1.y * sy;
	v[0].sz = prim->v1.z;
	v[0].tu = prim->t1.x / 256.0f;
	v[0].tv = prim->t1.y / 256.0f;
	v[0].color = colour;
	v[0].specular = specular;
	v[0].rhw = 0.1f / prim->v1.z;

	v[1].sx = (prim->v1.x + 16) * sx;
	v[1].sy = prim->v1.y * sy;
	v[1].sz = prim->v1.z;
	v[1].tu = (prim->t1.x + 16) / 256.0f;
	v[1].tv = prim->t1.y / 256.0f;
	v[1].color = colour;
	v[1].specular = specular;
	v[1].rhw = 0.1f / prim->v1.z;

	v[2].sx = prim->v1.x * sx;
	v[2].sy = (prim->v1.y + 16) * sy;
	v[2].sz = prim->v1.z;
	v[2].tu = prim->t1.x / 256.0f;
	v[2].tv = (prim->t1.y + 16) / 256.0f;
	v[2].color = colour;
	v[2].specular = specular;
	v[2].rhw = 0.1f / prim->v1.z;

	v[3].sx = (prim->v1.x + 16) * sx;
	v[3].sy = (prim->v1.y + 16) * sy;
	v[3].sz = prim->v1.z;
	v[3].tu = (prim->t1.x + 16) / 256.0f;
	v[3].tv = (prim->t1.y + 16) / 256.0f;
	v[3].color = colour;
	v[3].specular = specular;
	v[3].rhw = 0.1f / prim->v1.z;

	Fix2DCoordinates(v);

	SetTexture::Call(g_DrawEnv::Get().tpage, prim->palette);

	SetD3DRenderState_AlphaBlend::Call(prim->value, g_DrawEnv::Get().tpage);
	SetD3DShadeMode::Call(D3DSHADE_FLAT);

	g_IDirect3DDevice3::Get()->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, v, 4, 0);
}

//Func<0x5A2EB0, void, GpuPrim_Sprite* /* prim */> ProcessGpuPrim_Sprite;
/*auto ProcessGpuPrim_SpriteHook(auto prim) {
	prim->word18 = 1;
	prim->word1A = 1;
	prim->word1E = 1;

	ProcessGpuPrim_Sprite.Original(prim);
}*/

typedef Func<0x5A7AE0, void,
	int,	// x
	int		// y
> SetGeomOffset;
auto SetGeomOffsetHook(auto x, auto y) {
	LogDebug("SetGeomOffset: %i, %i\n", x, y);

	auto width = ConfigManager::Get().GetScaledRenderWidth();
	auto diff = width - 320;

	SetGeomOffset::Original(x + diff / 2, y);
}

typedef Func<0x5A7910, void,
	DRAWENV*,	// env
	short,		// x
	short,		// y
	short,		// w
	short		// h
> SetDefDrawEnv;
auto SetDefDrawEnvHook(auto env, auto x, auto y, auto w, auto h) {
	LogDebug("SetDefDrawEnv: %i, %i, %i, %i\n", x, y, w, h);

	auto width = ConfigManager::Get().GetScaledRenderWidth();
	auto diff = width - w;

	SetDefDrawEnv::Original(env, x + diff, y, width, h);
}

typedef Func<0x5A78E0, void,
	DISPENV*,	// disp
	short,		// x
	short,		// y
	short,		// w
	short		// h
> SetDefDispEnv;
auto SetDefDispEnvHook(auto disp, auto x, auto y, auto w, auto h) {
	LogDebug("SetDefDispEnv: %i, %i, %i, %i\n", x, y, w, h);

	auto width = ConfigManager::Get().GetScaledRenderWidth();
	auto diff = width - w;

	SetDefDispEnv::Original(disp, x + diff, y, width, h);
}

//Func<0x461F00, void> sub_461F00;
//auto sub_461F00Hook() {
//
//}


export void EnableRenderHooks() {
	EnableHook<DrawString>(DrawStringHook);
	EnableHook<DrawStringNumFont>(DrawStringNumFontHook);
	EnableHook<DrawStringSmall>(DrawStringSmallHook);
	EnableHook<DrawStringLarge>(DrawStringLargeHook);
	EnableHook<DrawNumTiny>(DrawNumTinyHook);
	EnableHook<ProcessGpuPrim_TextGlyph>(ProcessGpuPrim_TextGlyphHook);
	EnableHook<ProcessGpuPrim_TexturedQuad>(ProcessGpuPrim_TexturedQuadHook);
	EnableHook<ProcessGpuPrim_Line>(ProcessGpuPrim_LineHook);
	EnableHook<ProcessGpuPrim_TexturedRectWH>(ProcessGpuPrim_TexturedRectWHHook);
	EnableHook<ProcessGpuPrim_TexturedRect8>(ProcessGpuPrim_TexturedRect8Hook);
	EnableHook<ProcessGpuPrim_TexturedRect16>(ProcessGpuPrim_TexturedRect16Hook);
	//EnableHook<ProcessGpuPrim_Sprite>(ProcessGpuPrim_SpriteHook);
	EnableHook<SetGeomOffset>(SetGeomOffsetHook);
	EnableHook<SetDefDrawEnv>(SetDefDrawEnvHook);
	EnableHook<SetDefDispEnv>(SetDefDispEnvHook);
	//EnableHook<sub_461F00>(sub_461F00Hook);

	// Set render size
	const auto& cfgMgr = ConfigManager::Get();
	*(uint32_t*)0x66B710 = cfgMgr.GetRenderWidth();
	*(uint32_t*)0x66B714 = cfgMgr.GetRenderHeight();

	WriteProtectedMemory(0x516E54, (uint8_t)0);	// Disable translucency for text
	WriteProtectedMemory(0x5A2B71, (uint8_t)3);	// Enable alpha blending...
	WriteProtectedMemory(0x5A2B8D, (uint8_t)1);	//   ...for text
}
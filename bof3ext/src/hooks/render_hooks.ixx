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


FuncHook<decltype(DrawString)> DrawStringHook = [](auto x, auto y, auto paletteIdx, auto len, auto text) {
	if (text[0] < 0) {
		auto s = std::stacktrace::current();
		LogDebug("DrawString: %s (%p)\n", text, s[s.size() - 1].native_handle());
	}

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

	auto advance = (int)GlyphManager::Get().GetGlyphAdvance();
	advance = (int)std::ceil(advance / ConfigManager::Get().GetRenderScale() * (10.0 / 12.0));

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

					((DrawCommand_TextGlyph*)*g_DrawCommands)->charCode = v13;
					sub_516D50(_x, y - 2, 10, 10 - v16, v15, v16, i);
				}

				_x += advance;
			}
		}

		v10 = *++v5;
	}

	return v5 + 1;
};

FuncHook<decltype(DrawStringLarge)> DrawStringLargeHook = [](auto x, auto y, auto paletteId, auto text) {
	auto _x = x;
	auto _y = y + 1;

	char v8;

	auto advance = (int)GlyphManager::Get().GetGlyphAdvance();
	advance = (int)std::ceil(advance / ConfigManager::Get().GetRenderScale() * (16.0 / 12.0));

	do {
		auto c = *text;

		if (*text == '\n') {
			_x = x;
			_y += 12;
		}
		else if (c != ' ') {
			if (c) {
				((DrawCommand_TextGlyph*)*g_DrawCommands)->charCode = c - '!';
				sub_516D50(_x, y - 1, 16, 16, 0, 0, ((16 * paletteId) >> 4) & 0x3F | 0x7800);
			}
		}

		v8 = text[1];
		_x += advance;
		++text;
	} while (v8);
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

	auto advance = (int)GlyphManager::Get().GetGlyphAdvance();
	advance = (int)std::ceil(advance / ConfigManager::Get().GetRenderScale() * (8.0 / 12.0));

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

					((DrawCommand_TextGlyph*)*g_DrawCommands)->charCode = v13;
					sub_516D50(_x, y - 2, 8, 8 - v16, v15, v16, i);
				}

				_x += advance;
			}
		}

		v10 = *++v5;
	}
};

FuncHook<decltype(DrawStringNumFont)> DrawStringNumFontHook = [](auto x, auto y, auto paletteIdx, auto text) {
	DrawStringSmall(x, y + 2, paletteIdx, GetStringLength(text), text);
};

Func<0x5A2900, HRESULT, int /* a1 */> sub_5A2900;
FuncHook<decltype(sub_5A2900)> sub_5A2900Hook = [](auto a1) {
	g_IDirect3DDevice3->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFN_LINEAR);
	g_IDirect3DDevice3->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_LINEAR);

	return sub_5A2900.Original(a1);
};

Func<0x5A0C40, void, DrawCommand_TexturedQuad* /* drawCmd */> ProcessDrawCommand_TexturedQuad;
FuncHook<decltype(ProcessDrawCommand_TexturedQuad)> ProcessDrawCommand_TexturedQuadHook = [](auto drawCmd) {
	D3DCOLOR colour, specular;

	GetD3DCOLOR(drawCmd->colour.r, drawCmd->colour.g, drawCmd->colour.b, drawCmd->typeAndFlags, drawCmd->vertices[1].metadata, &colour, &specular);

	D3DTLVERTEX v[4]{ 0 };

	v[0].sx = drawCmd->vertices[0].position.x * *g_RenderScaleX;
	v[0].sy = drawCmd->vertices[0].position.y * *g_RenderScaleY;
	v[0].sz = drawCmd->vertices[0].position.z;
	v[0].tu = drawCmd->vertices[0].texCoords.x / 256.0;
	v[0].tv = drawCmd->vertices[0].texCoords.y / 256.0;
	v[0].color = colour;
	v[0].specular = specular;
	v[0].rhw = 0.1 / drawCmd->vertices[0].position.z;

	v[1].sx = drawCmd->vertices[1].position.x * *g_RenderScaleX;
	v[1].sy = drawCmd->vertices[1].position.y * *g_RenderScaleY;
	v[1].sz = drawCmd->vertices[1].position.z;
	v[1].tu = (drawCmd->vertices[1].texCoords.x + 1) / 256.0;
	v[1].tv = drawCmd->vertices[1].texCoords.y / 256.0;
	v[1].color = colour;
	v[1].specular = specular;
	v[1].rhw = 0.1 / drawCmd->vertices[1].position.z;

	v[2].sx = drawCmd->vertices[2].position.x * *g_RenderScaleX;
	v[2].sy = drawCmd->vertices[2].position.y * *g_RenderScaleY;
	v[2].sz = drawCmd->vertices[2].position.z;
	v[2].tu = drawCmd->vertices[2].texCoords.x / 256.0;
	v[2].tv = (drawCmd->vertices[2].texCoords.y + 1) / 256.0;
	v[2].color = colour;
	v[2].specular = specular;
	v[2].rhw = 0.1 / drawCmd->vertices[2].position.z;

	v[3].sx = drawCmd->vertices[3].position.x * *g_RenderScaleX;
	v[3].sy = drawCmd->vertices[3].position.y * *g_RenderScaleY;
	v[3].sz = drawCmd->vertices[3].position.z;
	v[3].tu = (drawCmd->vertices[3].texCoords.x + 1) / 256.0;
	v[3].tv = (drawCmd->vertices[3].texCoords.y + 1) / 256.0;
	v[3].color = colour;
	v[3].specular = specular;
	v[3].rhw = 0.1 / drawCmd->vertices[3].position.z;

	Fix2DCoordinates(v);

	SetTexture(drawCmd->vertices[1].metadata, drawCmd->vertices[0].metadata);

	SetD3DRenderState_AlphaBlend(drawCmd->typeAndFlags, drawCmd->vertices[1].metadata);
	SetD3DShadeMode(D3DSHADE_FLAT);

	g_IDirect3DDevice3->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, v, 4, 0);
};

Func<0x5A17A0, void, DrawCommand_Line* /* drawCmd */> ProcessDrawCommand_Line;
FuncHook<decltype(ProcessDrawCommand_Line)> ProcessDrawCommand_LineHook = [](auto drawCmd) {
	D3DCOLOR colour;

	GetD3DCOLOR(drawCmd->colour.r, drawCmd->colour.g, drawCmd->colour.b, drawCmd->typeAndFlags, stru_7DED00->word14, &colour, 0);

	auto xDelta = drawCmd->v2.x - drawCmd->v1.x;
	auto yDelta = drawCmd->v2.y - drawCmd->v1.y;

	auto theta = std::atan2(yDelta, xDelta)/* * 180 / std::numbers::pi*/;
	auto distance = std::sqrt(xDelta * xDelta + yDelta * yDelta);

	D3DTLVERTEX v[4]{ 0 };

	if (drawCmd->v1.y == drawCmd->v2.y) {			// Horizontal line
		auto v1 = (drawCmd->v1.x < drawCmd->v2.x) ? drawCmd->v1 : drawCmd->v2;

		v[0].sx = v[2].sx = v1.x * *g_RenderScaleX;
		v[1].sx = v[3].sx = (v1.x + distance) * *g_RenderScaleX;

		v[0].sy = v[1].sy = v1.y * *g_RenderScaleY;
		v[2].sy = v[3].sy = (v1.y + 1) * *g_RenderScaleY;
	} else if (drawCmd->v1.x == drawCmd->v2.x) {	// Vertical line
		auto v1 = (drawCmd->v1.y < drawCmd->v2.y) ? drawCmd->v1 : drawCmd->v2;

		v[0].sx = v[2].sx = v1.x * *g_RenderScaleX;
		v[1].sx = v[3].sx = (v1.x + 1) * *g_RenderScaleX;

		v[0].sy = v[1].sy = v1.y * *g_RenderScaleY;
		v[2].sy = v[3].sy = (v1.y + distance) * *g_RenderScaleY;
	} else {
		ProcessDrawCommand_Line.Original(drawCmd);	// TODO: Handle angled lines
	}

	for (int i = 0; i < 4; ++i) {
		v[i].sz = drawCmd->v1.z;
		v[i].color = colour;
		v[i].rhw = 0.1 / drawCmd->v1.z;
	}

	//Fix2DCoordinates(v);

	g_IDirect3DDevice3->SetTexture(0, nullptr);

	SetD3DRenderState_AlphaBlend(drawCmd->typeAndFlags, stru_7DED00->word14);
	SetD3DShadeMode(D3DSHADE_FLAT);

	g_IDirect3DDevice3->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, v, 4, 0);
};

Func<0x5A2300, void, DrawCommand_TexturedPlane* /* drawCmd */> ProcessDrawCommand_TexturedPlane;
FuncHook<decltype(ProcessDrawCommand_TexturedPlane)> ProcessDrawCommand_TexturedPlaneHook = [](auto drawCmd) {
	D3DCOLOR colour, specular;

	GetD3DCOLOR(drawCmd->colour.r, drawCmd->colour.g, drawCmd->colour.b, drawCmd->typeAndFlags, stru_7DED00->word14, &colour, &specular);

	D3DTLVERTEX v[4]{ 0 };

	v[0].sx = drawCmd->tlVert.position.x * *g_RenderScaleX;
	v[0].sy = drawCmd->tlVert.position.y * *g_RenderScaleY;
	v[0].sz = drawCmd->tlVert.position.z;
	v[0].tu = drawCmd->tlVert.texCoords.x / 256.0;
	v[0].tv = drawCmd->tlVert.texCoords.y / 256.0;
	v[0].color = colour;
	v[0].specular = specular;
	v[0].rhw = 0.1 / drawCmd->tlVert.position.z;

	v[1].sx = (drawCmd->tlVert.position.x + drawCmd->size.x) * *g_RenderScaleX;
	v[1].sy = drawCmd->tlVert.position.y * *g_RenderScaleY;
	v[1].sz = drawCmd->tlVert.position.z;
	v[1].tu = (drawCmd->tlVert.texCoords.x + drawCmd->size.x) / 256.0;
	v[1].tv = drawCmd->tlVert.texCoords.y / 256.0;
	v[1].color = colour;
	v[1].specular = specular;
	v[1].rhw = 0.1 / drawCmd->tlVert.position.z;

	v[2].sx = drawCmd->tlVert.position.x * *g_RenderScaleX;
	v[2].sy = (drawCmd->tlVert.position.y + drawCmd->size.y) * *g_RenderScaleY;
	v[2].sz = drawCmd->tlVert.position.z;
	v[2].tu = drawCmd->tlVert.texCoords.x / 256.0;
	v[2].tv = (drawCmd->tlVert.texCoords.y + drawCmd->size.y) / 256.0;
	v[2].color = colour;
	v[2].specular = specular;
	v[2].rhw = 0.1 / drawCmd->tlVert.position.z;

	v[3].sx = (drawCmd->tlVert.position.x + drawCmd->size.x) * *g_RenderScaleX;
	v[3].sy = (drawCmd->tlVert.position.y + drawCmd->size.y) * *g_RenderScaleY;
	v[3].sz = drawCmd->tlVert.position.z;
	v[3].tu = (drawCmd->tlVert.texCoords.x + drawCmd->size.x) / 256.0;
	v[3].tv = (drawCmd->tlVert.texCoords.y + drawCmd->size.y) / 256.0;
	v[3].color = colour;
	v[3].specular = specular;
	v[3].rhw = 0.1 / drawCmd->tlVert.position.z;

	Fix2DCoordinates(v);

	SetTexture(stru_7DED00->word14, drawCmd->tlVert.metadata);

	SetD3DRenderState_AlphaBlend(drawCmd->typeAndFlags, stru_7DED00->word14);
	SetD3DShadeMode(D3DSHADE_FLAT);

	g_IDirect3DDevice3->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, v, 4, 0);
};

Func<0x5A2520, void, DrawCommand_TexturedPlane8x8* /* drawCmd */> ProcessDrawCommand_TexturedPlane8x8;
FuncHook<decltype(ProcessDrawCommand_TexturedPlane8x8)> ProcessDrawCommand_TexturedPlane8x8Hook = [](auto drawCmd) {
	D3DCOLOR colour, specular;

	GetD3DCOLOR(drawCmd->colour.r, drawCmd->colour.g, drawCmd->colour.b, drawCmd->typeAndFlags, stru_7DED00->word14, &colour, &specular);

	D3DTLVERTEX v[4]{ 0 };

	v[0].sx = drawCmd->tlVert.position.x * *g_RenderScaleX;
	v[0].sy = drawCmd->tlVert.position.y * *g_RenderScaleY;
	v[0].sz = drawCmd->tlVert.position.z;
	v[0].tu = drawCmd->tlVert.texCoords.x / 256.0;
	v[0].tv = drawCmd->tlVert.texCoords.y / 256.0;
	v[0].color = colour;
	v[0].specular = specular;
	v[0].rhw = 0.1 / drawCmd->tlVert.position.z;

	v[1].sx = (drawCmd->tlVert.position.x + 8) * *g_RenderScaleX;
	v[1].sy = drawCmd->tlVert.position.y * *g_RenderScaleY;
	v[1].sz = drawCmd->tlVert.position.z;
	v[1].tu = (drawCmd->tlVert.texCoords.x + 8) / 256.0;
	v[1].tv = drawCmd->tlVert.texCoords.y / 256.0;
	v[1].color = colour;
	v[1].specular = specular;
	v[1].rhw = 0.1 / drawCmd->tlVert.position.z;

	v[2].sx = drawCmd->tlVert.position.x * *g_RenderScaleX;
	v[2].sy = (drawCmd->tlVert.position.y + 8) * *g_RenderScaleY;
	v[2].sz = drawCmd->tlVert.position.z;
	v[2].tu = drawCmd->tlVert.texCoords.x / 256.0;
	v[2].tv = (drawCmd->tlVert.texCoords.y + 8) / 256.0;
	v[2].color = colour;
	v[2].specular = specular;
	v[2].rhw = 0.1 / drawCmd->tlVert.position.z;

	v[3].sx = (drawCmd->tlVert.position.x + 8) * *g_RenderScaleX;
	v[3].sy = (drawCmd->tlVert.position.y + 8) * *g_RenderScaleY;
	v[3].sz = drawCmd->tlVert.position.z;
	v[3].tu = (drawCmd->tlVert.texCoords.x + 8) / 256.0;
	v[3].tv = (drawCmd->tlVert.texCoords.y + 8) / 256.0;
	v[3].color = colour;
	v[3].specular = specular;
	v[3].rhw = 0.1 / drawCmd->tlVert.position.z;

	Fix2DCoordinates(v);

	SetTexture(stru_7DED00->word14, drawCmd->tlVert.metadata);

	SetD3DRenderState_AlphaBlend(drawCmd->typeAndFlags, stru_7DED00->word14);
	SetD3DShadeMode(D3DSHADE_FLAT);

	g_IDirect3DDevice3->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, v, 4, 0);
};

Func<0x5A2710, void, DrawCommand_TexturedPlane16x16* /* drawCmd */> ProcessDrawCommand_TexturedPlane16x16;
FuncHook<decltype(ProcessDrawCommand_TexturedPlane16x16)> ProcessDrawCommand_TexturedPlane16x16Hook = [](auto drawCmd) {
	D3DCOLOR colour, specular;

	GetD3DCOLOR(drawCmd->colour.r, drawCmd->colour.g, drawCmd->colour.b, drawCmd->typeAndFlags, stru_7DED00->word14, &colour, &specular);

	D3DTLVERTEX v[4]{ 0 };

	v[0].sx = drawCmd->tlVert.position.x * *g_RenderScaleX;
	v[0].sy = drawCmd->tlVert.position.y * *g_RenderScaleY;
	v[0].sz = drawCmd->tlVert.position.z;
	v[0].tu = drawCmd->tlVert.texCoords.x / 256.0;
	v[0].tv = drawCmd->tlVert.texCoords.y / 256.0;
	v[0].color = colour;
	v[0].specular = specular;
	v[0].rhw = 0.1 / drawCmd->tlVert.position.z;

	v[1].sx = (drawCmd->tlVert.position.x + 16) * *g_RenderScaleX;
	v[1].sy = drawCmd->tlVert.position.y * *g_RenderScaleY;
	v[1].sz = drawCmd->tlVert.position.z;
	v[1].tu = (drawCmd->tlVert.texCoords.x + 16) / 256.0;
	v[1].tv = drawCmd->tlVert.texCoords.y / 256.0;
	v[1].color = colour;
	v[1].specular = specular;
	v[1].rhw = 0.1 / drawCmd->tlVert.position.z;

	v[2].sx = drawCmd->tlVert.position.x * *g_RenderScaleX;
	v[2].sy = (drawCmd->tlVert.position.y + 16) * *g_RenderScaleY;
	v[2].sz = drawCmd->tlVert.position.z;
	v[2].tu = drawCmd->tlVert.texCoords.x / 256.0;
	v[2].tv = (drawCmd->tlVert.texCoords.y + 16) / 256.0;
	v[2].color = colour;
	v[2].specular = specular;
	v[2].rhw = 0.1 / drawCmd->tlVert.position.z;

	v[3].sx = (drawCmd->tlVert.position.x + 16) * *g_RenderScaleX;
	v[3].sy = (drawCmd->tlVert.position.y + 16) * *g_RenderScaleY;
	v[3].sz = drawCmd->tlVert.position.z;
	v[3].tu = (drawCmd->tlVert.texCoords.x + 16) / 256.0;
	v[3].tv = (drawCmd->tlVert.texCoords.y + 16) / 256.0;
	v[3].color = colour;
	v[3].specular = specular;
	v[3].rhw = 0.1 / drawCmd->tlVert.position.z;

	Fix2DCoordinates(v);

	SetTexture(stru_7DED00->word14, drawCmd->tlVert.metadata);

	SetD3DRenderState_AlphaBlend(drawCmd->typeAndFlags, stru_7DED00->word14);
	SetD3DShadeMode(D3DSHADE_FLAT);

	g_IDirect3DDevice3->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, v, 4, 0);
};

Func<0x5A2EB0, void, DrawCommand_Sprite* /* drawCmd */> ProcessDrawCommand_CharacterSprite;
FuncHook<decltype(ProcessDrawCommand_CharacterSprite)> ProcessDrawCommand_CharacterSpriteHook = [](auto drawCmd) {
	drawCmd->word18 = 1;
	drawCmd->word1A = 1;
	drawCmd->word1E = 1;

	ProcessDrawCommand_CharacterSprite.Original(drawCmd);
};

Func<0x5A7AE0, void, int /* x */, int /* y */> SetGeomOffset;
FuncHook<decltype(SetGeomOffset)> SetGeomOffsetHook = [](auto x, auto y) {
	LogDebug("SetGeomOffset: %i, %i\n", x, y);

	auto renderScale = ConfigManager::Get().GetRenderScale();
	auto wndSize = ConfigManager::Get().GetWindowSize();

	auto width = (wndSize.x / 320.0) / renderScale * 320;
	auto diff = width - 320;

	SetGeomOffset.Original(x + diff / 2, y);
};


struct PSX_RECT {
	short x, y;
	short w, h;
};

struct DR_ENV {
	unsigned int* tag;
	unsigned int code[15];
};

struct DRAWENV {
	PSX_RECT clip;
	short ofs[2];
	PSX_RECT tw;
	short tpage;
	byte dtd;
	byte dfe;
	byte isbg;
	byte r0;
	byte g0;
	byte b0;
	DR_ENV dr_env;
};

struct DISPENV {
	PSX_RECT disp;
	PSX_RECT screen;
	byte isinter;
	byte isrgb24;
	byte pad0;
	byte pad1;
};


Func<0x5A7910, void, DRAWENV* /* env */, short /* x */, short /* y */, short /* w */, short /* h */> SetDefDrawEnv;
FuncHook<decltype(SetDefDrawEnv)> SetDefDrawEnvHook = [](auto env, auto x, auto y, auto w, auto h) {
	LogDebug("SetDefDrawEnv: %i, %i, %i, %i\n", x, y, w, h);

	auto renderScale = ConfigManager::Get().GetRenderScale();
	auto wndSize = ConfigManager::Get().GetWindowSize();

	auto width = (wndSize.x / 320.0) / renderScale * 320;
	auto diff = width - w;

	SetDefDrawEnv.Original(env, x + diff, y, width, h);
};

Func<0x5A78E0, void, DISPENV* /* disp */, short /* x */, short /* y */, short /* w */, short /* h */> SetDefDispEnv;
FuncHook<decltype(SetDefDispEnv)> SetDefDispEnvHook = [](auto disp, auto x, auto y, auto w, auto h) {
	LogDebug("SetDefDispEnv: %i, %i, %i, %i\n", x, y, w, h);

	auto renderScale = ConfigManager::Get().GetRenderScale();
	auto wndSize = ConfigManager::Get().GetWindowSize();

	auto width = (wndSize.x / 320.0) / renderScale * 320;
	auto diff = width - w;

	SetDefDispEnv.Original(disp, x + diff, y, width, h);
};


export void EnableRenderHooks() {
	EnableHook(DrawString, DrawStringHook);
	EnableHook(DrawStringNumFont, DrawStringNumFontHook);
	EnableHook(DrawStringSmall, DrawStringSmallHook);
	EnableHook(DrawStringLarge, DrawStringLargeHook);
	EnableHook(DrawNumTiny, DrawNumTinyHook);
	EnableHook(sub_5A2900, sub_5A2900Hook);
	EnableHook(ProcessDrawCommand_TexturedQuad, ProcessDrawCommand_TexturedQuadHook);
	EnableHook(ProcessDrawCommand_Line, ProcessDrawCommand_LineHook);
	EnableHook(ProcessDrawCommand_TexturedPlane, ProcessDrawCommand_TexturedPlaneHook);
	EnableHook(ProcessDrawCommand_TexturedPlane8x8, ProcessDrawCommand_TexturedPlane8x8Hook);
	EnableHook(ProcessDrawCommand_TexturedPlane16x16, ProcessDrawCommand_TexturedPlane16x16Hook);
	//EnableHook(ProcessDrawCommand_CharacterSprite, ProcessDrawCommand_CharacterSpriteHook);
	EnableHook(SetGeomOffset, SetGeomOffsetHook);
	EnableHook(SetDefDrawEnv, SetDefDrawEnvHook);
	EnableHook(SetDefDispEnv, SetDefDispEnvHook);
	//EnableHook(WndProc_Game, WndProc_GameHook);

	// Set render size
	auto renderScale = ConfigManager::Get().GetRenderScale();
	auto wndSize = ConfigManager::Get().GetWindowSize();
	auto renderWidth = (wndSize.x / 320.0) / renderScale * 320;

	*(uint32_t*)0x66B710 = renderWidth * renderScale;
	*(uint32_t*)0x66B714 = 240 * renderScale;

	//WriteProtectedMemory(0x5A2B71, (uint8_t)3);	// Enable alpha blending for text
	//WriteProtectedMemory(0x5A2B8D, (uint8_t)1);	// Enable alpha blending for text
}
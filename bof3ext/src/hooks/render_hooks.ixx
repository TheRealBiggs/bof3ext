module;

#include <cstdint>
#include <cstdio>
#include <d3d.h>
#include <intrin.h>

export module bof3ext.hooks:render;

import bof3ext.helpers;
import bof3ext.math;
import bof3ext.configManager;
import bof3ext.glyphManager;
import bof3ext.textManager;

import bof3.render;
import bof3.text;
import bof3.texture;

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


Func<0x516D50, void, int16_t /* x */, int16_t /* y */, int16_t /* w */, int16_t /* h */, int16_t /* a5 */, int8_t /* a6 */, uint16_t /* a7 */> sub_516D50;
Func<0x59FBA0, void, uint32_t /* r */, uint32_t /* g */, uint32_t /* b */, uint8_t /* typeAndFlags */, uint32_t /* a5 */, D3DCOLOR* /* outColour */, D3DCOLOR* /* outSpecular */> GetD3DCOLOR;


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

					(*(DrawCommand_UnkIcon0**)0x7E0670)->paletteId = v13;
					sub_516D50(_x, y - 2, 10, 10 - v16, v15, v16, i);
				}

				_x += advance;
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

					(*(DrawCommand_UnkIcon0**)0x7E0670)->paletteId = v13;
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


struct UnkStruct_C {
	uint8_t gap0[12];
	int16_t shortC;
	int16_t shortE;
	int16_t short10;
	int16_t short12;
	uint16_t word14;
	uint8_t byte16;
	uint8_t byte17;
	uint8_t gap1[1];
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t gap[64];
};


Vec2f RotatePoint(float cx, float cy, float angleInRads, Vec2f p) {
	float s = sin(angleInRads);
	float c = cos(angleInRads);

	// translate point back to origin:
	p.x -= cx;
	p.y -= cy;

	// rotate point
	float xnew = p.x * c - p.y * s;
	float ynew = p.x * s + p.y * c;

	// translate point back:
	p.x = xnew + cx;
	p.y = ynew + cy;

	return p;
}


Accessor<0x7DED00, UnkStruct_C> stru_7DED00;
ArrayAccessor<0x7CAE42, uint16_t> word_7CAE42;


Func<0x59FCA0, void, uint8_t /* typeAndFlags */, uint16_t /* a2 */> SetD3DRenderState_AlphaBlend;
Func<0x59FD80, void, D3DSHADEMODE /* mode */> SetD3DShadeMode;


Func<0x5A0C40, void, DrawCommand_TexturedQuad* /* drawCmd */> ProcessDrawCommand_TexturedQuad;
FuncHook<decltype(ProcessDrawCommand_TexturedQuad)> ProcessDrawCommand_TexturedQuadHook = [](auto drawCmd) {
	D3DCOLOR colour, specular;

	GetD3DCOLOR(drawCmd->colour.r, drawCmd->colour.g, drawCmd->colour.b, drawCmd->type, drawCmd->vertices[1].metadata, &colour, &specular);

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

	SetD3DRenderState_AlphaBlend(drawCmd->type, drawCmd->vertices[1].metadata);
	SetD3DShadeMode(D3DSHADE_FLAT);

	g_IDirect3DDevice3->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, v, 4, 0);
};

Func<0x5A17A0, void, DrawCommand_Line* /* drawCmd */> ProcessDrawCommand_Line;
FuncHook<decltype(ProcessDrawCommand_Line)> ProcessDrawCommand_LineHook = [](auto drawCmd) {
	D3DCOLOR colour;

	GetD3DCOLOR(drawCmd->colour.r, drawCmd->colour.g, drawCmd->colour.b, drawCmd->type, stru_7DED00->word14, &colour, 0);

	auto xDelta = drawCmd->v2.x - drawCmd->v1.x;
	auto yDelta = drawCmd->v2.y - drawCmd->v1.y;

	auto theta = std::atan2(yDelta, xDelta)/* * 180 / std::numbers::pi*/;
	auto distance = std::sqrt(xDelta * xDelta + yDelta * yDelta);

	D3DTLVERTEX v[4]{ 0 };

	v[0].sx = drawCmd->v1.x * *g_RenderScaleX;
	v[0].sy = drawCmd->v1.y * *g_RenderScaleY;
	v[0].sz = drawCmd->v1.z;
	v[0].color = colour;
	v[0].rhw = 0.1 / drawCmd->v1.z;

	v[1].sx = (drawCmd->v1.x + distance) * *g_RenderScaleX;
	v[1].sy = drawCmd->v1.y * *g_RenderScaleY;
	v[1].sz = drawCmd->v1.z;
	v[1].color = colour;
	v[1].rhw = 0.1 / drawCmd->v1.z;

	v[2].sx = drawCmd->v1.x * *g_RenderScaleX;
	v[2].sy = (drawCmd->v1.y + 1) * *g_RenderScaleY;
	v[2].sz = drawCmd->v1.z;
	v[2].color = colour;
	v[2].rhw = 0.1 / drawCmd->v1.z;

	v[3].sx = (drawCmd->v1.x + distance) * *g_RenderScaleX;
	v[3].sy = (drawCmd->v1.y + 1) * *g_RenderScaleY;
	v[3].sz = drawCmd->v1.z;
	v[3].color = colour;
	v[3].rhw = 0.1 / drawCmd->v1.z;

	auto p = RotatePoint(v[0].sx, v[0].sy, theta, { v[1].sx, v[1].sy });
	v[1].sx = p.x;
	v[1].sy = p.y;

	p = RotatePoint(v[0].sx, v[0].sy, theta, { v[2].sx, v[2].sy });
	v[2].sx = p.x;
	v[2].sy = p.y;

	p = RotatePoint(v[0].sx, v[0].sy, theta, { v[3].sx, v[3].sy });
	v[3].sx = p.x;
	v[3].sy = p.y;

	if (drawCmd->v2.y > drawCmd->v1.y) {
		for (int i = 0; i < 4; ++i)
			v[i].sx += 1 * *g_RenderScaleX;
	}

	if (drawCmd->v2.x < drawCmd->v1.x) {
		for (int i = 0; i < 4; ++i)
			v[i].sy += 1 * *g_RenderScaleY;
	}

	Fix2DCoordinates(v);

	g_IDirect3DDevice3->SetTexture(0, nullptr);

	SetD3DRenderState_AlphaBlend(drawCmd->type, stru_7DED00->word14);
	SetD3DShadeMode(D3DSHADE_FLAT);

	g_IDirect3DDevice3->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, v, 4, 0);
};

Func<0x5A2300, void, DrawCommand_TexturedPlane* /* drawCmd */> ProcessDrawCommand_TexturedPlane;
FuncHook<decltype(ProcessDrawCommand_TexturedPlane)> ProcessDrawCommand_TexturedPlaneHook = [](auto drawCmd) {
	D3DCOLOR colour, specular;

	GetD3DCOLOR(drawCmd->colour.r, drawCmd->colour.g, drawCmd->colour.b, drawCmd->type, stru_7DED00->word14, &colour, &specular);

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

	SetD3DRenderState_AlphaBlend(drawCmd->type, stru_7DED00->word14);
	SetD3DShadeMode(D3DSHADE_FLAT);

	g_IDirect3DDevice3->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, v, 4, 0);
};

Func<0x5A2520, void, DrawCommand_TexturedPlane8x8* /* drawCmd */> ProcessDrawCommand_TexturedPlane8x8;
FuncHook<decltype(ProcessDrawCommand_TexturedPlane8x8)> ProcessDrawCommand_TexturedPlane8x8Hook = [](auto drawCmd) {
	D3DCOLOR colour, specular;

	GetD3DCOLOR(drawCmd->colour.r, drawCmd->colour.g, drawCmd->colour.b, drawCmd->type, stru_7DED00->word14, &colour, &specular);

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

	SetD3DRenderState_AlphaBlend(drawCmd->type, stru_7DED00->word14);
	SetD3DShadeMode(D3DSHADE_FLAT);

	g_IDirect3DDevice3->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, v, 4, 0);
};

Func<0x5A2710, void, DrawCommand_TexturedPlane16x16* /* drawCmd */> ProcessDrawCommand_TexturedPlane16x16;
FuncHook<decltype(ProcessDrawCommand_TexturedPlane16x16)> ProcessDrawCommand_TexturedPlane16x16Hook = [](auto drawCmd) {
	D3DCOLOR colour, specular;

	GetD3DCOLOR(drawCmd->colour.r, drawCmd->colour.g, drawCmd->colour.b, drawCmd->type, stru_7DED00->word14, &colour, &specular);

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

	SetD3DRenderState_AlphaBlend(drawCmd->type, stru_7DED00->word14);
	SetD3DShadeMode(D3DSHADE_FLAT);

	g_IDirect3DDevice3->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, v, 4, 0);
};

//Func<0x5A2EB0, void, DrawCommand_CharacterSprite* /* drawCmd */> ProcessDrawCommand_CharacterSprite;
//FuncHook<decltype(ProcessDrawCommand_CharacterSprite)> ProcessDrawCommand_CharacterSpriteHook = [](auto drawCmd) {
//	D3DCOLOR colour, specular;
//
//	GetD3DCOLOR(
//		drawCmd->colour.r,
//		drawCmd->colour.g,
//		drawCmd->colour.b,
//		drawCmd->type,
//		drawCmd->word1E,
//		&colour,
//		&specular);
//
//	auto v2 = 20 * sub_5A3160(drawCmd->word18, drawCmd->word1A, drawCmd->paletteIdx, drawCmd->word1E);
//
//	if ((drawCmd->word1E & 0x400) != 0) {
//		a1a = (DrawCommand_CharacterSprite*)word_7CAE42[v2];
//		g_DCVerts[2].sx = (drawCmd->position.x - (double)word_7CAE40[v2] * drawCmd->scale.x) * g_renderScaleX;
//		g_DCVerts[0].sx = g_DCVerts[2].sx;
//		v3 = drawCmd->position.x - (double)(int)a1a * drawCmd->scale.x;
//	} else {
//		a1b = (DrawCommand_CharacterSprite*)word_7CAE42[v2];
//		g_DCVerts[2].sx = ((double)word_7CAE40[v2] * drawCmd->scale.x + drawCmd->position.x) * g_renderScaleX;
//		g_DCVerts[0].sx = g_DCVerts[2].sx;
//		v3 = (double)(int)a1b * drawCmd->scale.x + drawCmd->position.x;
//	}
//
//	a1c = (DrawCommand_CharacterSprite*)word_7CAE44[v2];
//	v4 = word_7CAE46[v2];
//	g_DCVerts[3].sx = v3 * g_renderScaleX;
//	g_DCVerts[1].sx = g_DCVerts[3].sx;
//	v5 = word_7CAE3C[v2];
//	g_DCVerts[1].sy = ((double)(int)a1c * drawCmd->scale.y + drawCmd->position.y) * g_renderScaleY;
//	g_DCVerts[0].sy = g_DCVerts[1].sy;
//	a1d = (DrawCommand_CharacterSprite*)v5;
//	v6 = word_7CAE38[v2];
//	v7 = (double)v4 * drawCmd->scale.y;
//	v13 = v6;
//	v8 = v7 + drawCmd->position.y;
//
//	g_DCVerts[3].color = colour;
//	g_DCVerts[2].color = colour;
//	g_DCVerts[1].color = colour;
//	g_DCVerts[0].color = colour;
//
//	g_DCVerts[3].sz = 0.99000001;
//
//	g_DCVerts[3].specular = specular;
//	g_DCVerts[2].specular = specular;
//	g_DCVerts[1].specular = specular;
//	g_DCVerts[0].specular = specular;
//
//	g_DCVerts[3].sy = v8 * g_renderScaleY;
//	g_DCVerts[2].sy = g_DCVerts[3].sy;
//	v9 = (double)(int)a1d;
//
//	g_DCVerts[2].sz = 0.99000001;
//
//	LOWORD(v4) = word_7CAE3E[v2];
//
//	g_DCVerts[1].sz = 0.99000001;
//
//	a1e = (DrawCommand_CharacterSprite*)(unsigned __int16)v4;
//	v10 = (unsigned __int16)word_7CAE3A[v2];
//
//	g_DCVerts[0].sz = 0.99000001;
//
//	g_DCVerts[3].rhw = 0.1;
//	g_DCVerts[2].rhw = 0.1;
//	g_DCVerts[1].rhw = 0.1;
//	g_DCVerts[0].rhw = 0.1;
//
//	g_DCVerts[2].tu = 0.5 / v9;
//	g_DCVerts[0].tu = g_DCVerts[2].tu;
//	a1f = (float)(int)a1e;
//	g_DCVerts[1].tv = 0.5 / a1f;
//	g_DCVerts[0].tv = g_DCVerts[1].tv;
//	v13 = v10;
//	g_DCVerts[3].tu = ((double)v6 - 0.5) / v9;
//	g_DCVerts[1].tu = g_DCVerts[3].tu;
//	g_DCVerts[3].tv = ((double)v10 - 0.5) / a1f;
//	g_DCVerts[2].tv = g_DCVerts[3].tv;
//
//	nullsub_1();
//
//	SetD3DRenderState_AlphaBlend(drawCmd->type, drawCmd->word1E);
//	SetD3DShadeMode(D3DSHADE_FLAT);
//
//	g_IDirect3DDevice3->DrawPrimitive(g_IDirect3DDevice3, D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, g_DCVerts, 4, 0);
//};


export void EnableRenderHooks() {
	EnableHook(DrawString, DrawStringHook);
	EnableHook(DrawStringNumFont, DrawStringNumFontHook);
	EnableHook(DrawStringSmall, DrawStringSmallHook);
	EnableHook(DrawNumTiny, DrawNumTinyHook);
	EnableHook(sub_5A2900, sub_5A2900Hook);
	EnableHook(ProcessDrawCommand_TexturedQuad, ProcessDrawCommand_TexturedQuadHook);
	EnableHook(ProcessDrawCommand_Line, ProcessDrawCommand_LineHook);
	EnableHook(ProcessDrawCommand_TexturedPlane, ProcessDrawCommand_TexturedPlaneHook);
	EnableHook(ProcessDrawCommand_TexturedPlane8x8, ProcessDrawCommand_TexturedPlane8x8Hook);
	EnableHook(ProcessDrawCommand_TexturedPlane16x16, ProcessDrawCommand_TexturedPlane16x16Hook);
	//EnableHook(ProcessDrawCommand_CharacterSprite, ProcessDrawCommand_CharacterSpriteHook);

	// Set render size
	auto renderScale = ConfigManager::Get().GetRenderScale();

	*(uint32_t*)0x66B710 = 320 * renderScale;
	*(uint32_t*)0x66B714 = 240 * renderScale;

	//WriteProtectedMemory(0x5A2B71, (uint8_t)3);	// Enable alpha blending for text
	//WriteProtectedMemory(0x5A2B8D, (uint8_t)1);	// Enable alpha blending for text
}
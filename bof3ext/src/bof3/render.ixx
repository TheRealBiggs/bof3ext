module;

#include <cstdint>
#include <d3d.h>
#include <ddraw.h>

export module bof3.render;

import bof3ext.helpers;
import bof3ext.math;
import bof3.dat;


export enum class DrawCommandType {
	Tri = 8,
	TexturedTri,
	Quad,
	TexturedQuad,
	GradatedTri,
	GradatedTexturedTri,
	GradatedQuad,
	GradatedTexturedQuad,
	Line,
	_UNUSED_0,
	DoubleLine,
	TripleLine,
	GradatedLine,
	_UNUSED_1,
	GradatedDoubleLine,
	GradatedTripleLine,
	Plane,
	TexturedPlane,
	Point,
	TextGlyph,
	_UNUSED_2,			// Plane8x8?
	TexturedPlane8x8,
	_UNUSED_3,			// Plane16x16?
	TexturedPlane16x16,
	_UNUSED_4,
	CharacterSprite,

	SetBlendMode = 58
};


export struct Colour {
	uint8_t r, g, b;
};

export struct Vertf {
	Vec3f position;
	Vec2b texCoords;
	uint16_t metadata;
};


export struct DrawCommand {
	uint8_t gap0[4];
	Colour colour;
	union {
		uint8_t typeAndFlags;
		struct {
			uint8_t flags : 2;
			uint8_t type : 6;
		};
	};
};

export struct DrawCommand_SetBlendMode : public DrawCommand {
	void* vp8;
};

export struct DrawCommand_Plane : public DrawCommand {
	Vec3f position;
	Vec2f size;
};

export struct DrawCommand_TexturedPlane : DrawCommand {
	Vertf tlVert;
	Vec2s size;
};

export struct DrawCommand_Quad : public DrawCommand {
	Vec3f vertices[4];
};

export struct DrawCommand_TexturedQuad : DrawCommand {
	Vertf vertices[4];
};

export struct DrawCommand_TexturedPlane8x8 : DrawCommand {
	Vertf tlVert;
};

export struct DrawCommand_TexturedPlane16x16 : DrawCommand_TexturedPlane8x8 {

};

export struct DrawCommand_Line : public DrawCommand {
	Vec3f v1;
	Vec3f v2;
};

export struct DrawCommand_TextGlyph : DrawCommand {
	int16_t x1;
	int16_t y1;
	uint8_t tx1;
	uint8_t ty1;
	uint16_t paletteIdx;
	int16_t x2;
	int16_t y2;
	uint8_t tx2;
	uint8_t ty2;
	uint16_t charCode;
	int16_t x3;
	int16_t y3;
	uint8_t tx3;
	uint8_t ty3;
	uint8_t gap4[2];
	int16_t x4;
	int16_t y4;
	uint8_t tx4;
	uint8_t ty4;
	uint8_t gap[2];
};


export struct DrawCommand_Sprite : DrawCommand {
	Vec2f position;
	Vec2f scale;
	uint16_t word18;
	uint16_t word1A;
	uint16_t paletteIdx;
	uint16_t word1E;
};


export struct FontGlyph {
	uint16_t charCode;
	uint16_t paletteIndex;
	uint32_t dword4;
	IDirectDrawSurface4* surface;
	IDirect3DTexture2* texture;
	uint16_t word10;
};


export struct UnkStruct_C {
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


export struct struct_dword_905B84 {
	uint8_t byte0;
	uint8_t gap0;
	uint8_t byte1;
	uint8_t byte2;
	uint16_t x;
	uint16_t y;
	uint8_t gap8[2];
	uint8_t byteA;
	uint8_t byteB;
	uint8_t gap1;
	uint8_t byteD;
};


export Func<0x444340, void,
	int16_t,	// x
	int16_t,	// y
	uint8_t,	// a3
	uint16_t	// a4
> DrawNumTiny;

export Func<0x461E50, void,
	uint8_t,	// index
	uint8_t		// cmdSize
> PushDrawCommand;

export Func<0x516B30, uint8_t*,
	int16_t,	// x
	int16_t,	// y
	uint32_t,	// paletteIdx
	uint8_t,	// len
	const char*	// text
> DrawString;

export Func<0x516D50, void,
	int16_t,	// x
	int16_t,	// y
	int16_t,	// w
	int16_t,	// h
	int16_t,	// a5
	int8_t,		// a6
	uint16_t	// a7
> sub_516D50;

export Func<0x516E70, const char*,
	int16_t,	// x
	int16_t,	// y
	uint8_t,	// paletteIdx
	uint8_t,	// len
	const char* // text
> DrawStringSmall;

export Func<0x516F60, void,
	int16_t,	// x
	int16_t,	// y
	uint8_t,	// paletteId
	const char*	// text
> DrawStringLarge;

export Func<0x517090, void,
	int16_t,	// x
	int16_t,	// y
	uint32_t,	// paletteIdx
	const char* // text
> DrawStringNumFont;

export Func<0x59FBA0, void,
	uint32_t,	// r
	uint32_t,	// g
	uint32_t,	// b
	uint8_t,	// typeAndFlags
	uint32_t,	// a5
	D3DCOLOR*,	// outColour
	D3DCOLOR*	// outSpecular
> GetD3DCOLOR;

export Func<0x59FCA0, void,
	uint8_t,	// typeAndFlags
	uint16_t	// a2
> SetD3DRenderState_AlphaBlend;

export Func<0x59FD80, void,
	D3DSHADEMODE	// mode
> SetD3DShadeMode;

export Func<0x5A04C0, uint16_t*,
	uint16_t	// id
> GetPalette;

export Func<0x5A3160, int,
	uint16_t,	// a1
	uint16_t,	// a2
	uint16_t,	// a3
	uint16_t	// a4
> sub_5A3160; // Something to do with getting character sprites?

export Func<0x5A7710, void,
	DrawCommand_TexturedPlane*	// drawCmd
> Init_DrawCommand_TexturedPlane;

export Func<0x5A77C0, void,
	DrawCommand_SetBlendMode*,	// drawCmd
	BOOL,						// a2
	BOOL,						// a3
	uint16_t,					// flags
	DrawCommand*				// a5
> Init_DrawCommand_SetBlendMode;

export Func<0x5A79A0, uint16_t,
	char,	// a1
	char,	// a2
	int,	// a3
	int		// a4
> CreateBlendModeFlags;


export Accessor<		0x7C9F4C, float>				g_RenderScaleX;
export Accessor<		0x7C9F48, float>				g_RenderScaleY;
export Accessor<		0x7DED00, UnkStruct_C>			stru_7DED00;
export ArrayAccessor<	0x7C9F50, FontGlyph>			g_FontGlyphs;
export ArrayAccessor<	0x7CAE42, uint16_t>				word_7CAE42;
export PointerAccessor<	0x7E0670, DrawCommand>			g_DrawCommands;
export PointerAccessor<	0x7CC334, IDirectDraw4>			g_IDirectDraw4;
export PointerAccessor<	0x7CC350, IDirect3DDevice3>		g_IDirect3DDevice3;
export PointerAccessor<	0x905B84, struct_dword_905B84> dword_905B84;


void SetDrawCommandBlend(DrawCommand* drawCmd, bool value) {
	if (value)
		drawCmd->flags |= 2;
}


export void DrawPlane(uint16_t x, uint16_t y, uint16_t width, uint16_t height, Colour colour, bool blend) {
	auto drawCmd = (DrawCommand_Plane*)*g_DrawCommands;
	drawCmd->colour = colour;
	drawCmd->type = (int)DrawCommandType::Plane;

	drawCmd->position = { (float)x, (float)y, 0.01f };
	drawCmd->size = { (float)width, (float)height };

	SetDrawCommandBlend(drawCmd, blend);

	PushDrawCommand(1, sizeof(DrawCommand_Plane));
}

export void DrawQuad(Vec2i p1, Vec2i p2, Vec2i p3, Vec2i p4, Colour colour, bool blend) {
	auto drawCmd = (DrawCommand_Quad*)*g_DrawCommands;
	drawCmd->colour = colour;
	drawCmd->type = (int)DrawCommandType::Quad;

	drawCmd->vertices[0] = { (float)p1.x, (float)p1.y, 0.01f };
	drawCmd->vertices[1] = { (float)p2.x, (float)p2.y, 0.01f };
	drawCmd->vertices[2] = { (float)p3.x, (float)p3.y, 0.01f };
	drawCmd->vertices[3] = { (float)p4.x, (float)p4.y, 0.01f };

	SetDrawCommandBlend(drawCmd, blend);

	PushDrawCommand(1, sizeof(DrawCommand_Quad));
}

export void DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, Colour colour) {
	auto drawCmd = (DrawCommand_Line*)*g_DrawCommands;
	drawCmd->colour = colour;
	drawCmd->type = (int)DrawCommandType::Line;

	drawCmd->v1 = { (float)x1, (float)y1, 0.01f };
	drawCmd->v2 = { (float)x2, (float)y2, 0.01f };

	PushDrawCommand(1, sizeof(DrawCommand_Line));
}

export void SetBlendMode(int a1, int a2, int a3, int a4) {
	auto drawCmd = (DrawCommand_SetBlendMode*)*g_DrawCommands;
	drawCmd->type = (int)DrawCommandType::SetBlendMode;

	auto flags = CreateBlendModeFlags(a1, a2, a3, a4);
	Init_DrawCommand_SetBlendMode(drawCmd, 0, 0, flags, 0);

	PushDrawCommand(1, sizeof(DrawCommand_SetBlendMode));
}


export void DrawBorderedPanel(int16_t x, int16_t y, int16_t width, int16_t height) {
	SetBlendMode(0, 0, 960, 0);

	auto wndColourIdx = ((uint8_t*)0x9039E0)[122] * 64;
	auto wndColour = *(uint16_t*)&g_DatChunk_0_8200[wndColourIdx + 40];
	Colour colour = {
		(uint8_t)(8 * (wndColour & 0x1F)),
		(uint8_t)(8 * ((wndColour >> 5) & 0x1F)),
		(uint8_t)(8 * ((wndColour >> 10) & 0x1F))
	};

	DrawPlane(x, y + 2, width, height - 4, colour, true);	// Background
	DrawQuad(
		{ x + 2,			y },
		{ x + width - 2,	y },
		{ x,				y + 2 },
		{ x + width,		y + 2 },
		colour,
		true
	);	// Background top
	DrawQuad(
		{ x,				y + height - 2 },
		{ x + width,		y + height - 2 },
		{ x + 2,			y + height },
		{ x + width - 2,	y + height },
		colour,
		true
	);	// Background bottom

	SetBlendMode(0, 1, 960, 0);
	DrawPlane(x + 2, y + 2, width - 4, 1, colour, true);			// Top line
	DrawPlane(x + 2, y + 2, 1, height - 4, colour, true);			// Left line

	SetBlendMode(0, 0, 960, 0);
	DrawPlane(x + 2, y + height - 3, width - 4, 1, colour, true);	// Bottom line
	DrawPlane(x + width - 3, y + 2, 1, height - 4, colour, true);	// Right line
}
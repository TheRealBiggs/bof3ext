module;

#include <cstdint>
#include <d3d.h>
#include <ddraw.h>

export module bof3.render;

import bof3ext.helpers;
import bof3ext.math;
import bof3.dat;


export enum class GpuCommandType {
	Tri = 8,
	TexturedTri,
	Quad,
	TexturedQuad,
	ShadedTri,
	ShadedTexturedTri,
	ShadedQuad,
	ShadedTexturedQuad,
	Line,
	_UNUSED_0,
	Line2,
	Line3,
	ShadedLine,
	_UNUSED_1,
	ShadedLine2,
	ShadedLine3,
	RectWH,
	TexturedRectWH,
	Point,
	TextGlyph,
	_UNUSED_2,
	TexturedRect8,
	_UNUSED_3,
	TexturedRect16,
	_UNUSED_4,
	Sprite,

	SetDrawEnv = 58
};

export enum class GpuCommandBlendType {
	RawTexture = 0x1,
	Translucent = 0x2,
};


export enum class SetDrawEnvFlags : uint8_t {
	AllowDraw = 0x1,
	EnableDither = 0x2,
};


export struct Colour {
	uint8_t r, g, b;
};

export struct DR_ENV {
	uint32_t* tag;
	uint32_t code[15];
};

export struct DRAWENV {
	PSX_RECT clip;
	uint16_t ofs[2];
	PSX_RECT tw;
	uint16_t tpage;
	uint8_t dtd;
	uint8_t dfe;
	uint8_t isbg;
	uint8_t r0;
	uint8_t g0;
	uint8_t b0;
	DR_ENV dr_env;
};

export struct DISPENV {
	PSX_RECT disp;
	PSX_RECT screen;
	uint8_t isinter;
	uint8_t isrgb24;
	uint8_t pad0;
	uint8_t pad1;
};


export struct GpuCommand {
	uint8_t gap0[4];
	Colour colour;
	union {
		uint8_t value;
		struct {
			uint8_t flags : 2;
			uint8_t command : 6;
		};
	};
};

export struct GpuCommand_Tri : GpuCommand {
	Vec3f v1;
	Vec3f v2;
	Vec3f v3;
};

export struct GpuCommand_TexturedTri : GpuCommand {
	Vec3f v1;
	Vec2b t1;
	uint16_t palette;
	Vec3f v2;
	Vec2b t2;
	uint16_t texturePage;
	Vec3f v3;
	Vec2b t3;
	uint16_t _unused36;
};

export struct GpuCommand_Quad : GpuCommand_Tri {
	Vec3f v4;
};

export struct GpuCommand_TexturedQuad : GpuCommand_TexturedTri {
	Vec3f v4;
	Vec2b t4;
	uint16_t _unused46;
};

export struct GpuCommand_ShadedTri : GpuCommand {
	Vec3f v1;
	Colour c2;
	Vec3f v2;
	Colour c3;
	Vec3f v3;
};

export struct GpuCommand_ShadedTexturedTri : GpuCommand {
	Vec3f v1;
	Vec2b t1;
	uint16_t palette;
	Colour c2;
	Vec3f v2;
	Vec2b t2;
	uint16_t texturePage;
	Colour c3;
	Vec3f v3;
	Vec2b t3;
	uint16_t _unused3E;
};

export struct GpuCommand_ShadedQuad : GpuCommand_ShadedTri {
	Colour c4;
	Vec3f v4;
};

export struct GpuCommand_ShadedTexturedQuad : GpuCommand_ShadedTri {
	Colour c4;
	Vec3f v4;
	Vec2b t4;
	uint16_t _unused46;
};

export struct GpuCommand_Line : GpuCommand {
	Vec3f v1;
	Vec3f v2;
};

export struct GpuCommand_Line2 : GpuCommand_Line {
	Vec3f v3;
};

export struct GpuCommand_Line3 : GpuCommand_Line2 {
	Vec3f v4;
};

export struct GpuCommand_ShadedLine : GpuCommand {
	Vec3f v1;
	Colour c2;
	Vec3f v2;
};

export struct GpuCommand_ShadedLine2 : GpuCommand_ShadedLine {
	Colour c3;
	Vec3f v3;
};

export struct GpuCommand_ShadedLine3 : GpuCommand_ShadedLine2 {
	Colour c4;
	Vec3f v4;
};

export struct GpuCommand_Point : GpuCommand {
	Vec3f v1;
};

export struct GpuCommand_Rect8 : GpuCommand {
	Vec3f v1;
};

export struct GpuCommand_Rect16 : GpuCommand_Rect8 {};

export struct GpuCommand_RectWH : GpuCommand_Rect8 {
	Vec2f size;
};

export struct GpuCommand_TexturedRect8 : GpuCommand {
	Vec3f v1;
	Vec2b t1;
	uint16_t palette;
};

export struct GpuCommand_TexturedRect16 : GpuCommand_TexturedRect8 {};

export struct GpuCommand_TexturedRectWH : GpuCommand_TexturedRect8 {
	Vec2s size;
};

export struct GpuCommand_SetDrawEnv {
	uint8_t gap0[4];
	uint16_t texturePage;
	SetDrawEnvFlags flags;
	uint8_t command;
	PSX_RECT* rect;
};

export struct GpuCommand_TextGlyph : GpuCommand {
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

export struct GpuCommand_236 : GpuCommand {
	PSX_RECT rect;
	uint32_t x;
	uint32_t y;
};

export struct GpuCommand_Sprite : GpuCommand {
	Vec2f position;
	Vec2f scale;
	uint16_t word18;
	uint16_t word1A;
	uint16_t palette;
	uint16_t texturePage;
};


export struct FontGlyph {
	uint16_t charCode;
	uint16_t paletteIndex;
	uint32_t dword4;
	IDirectDrawSurface4* surface;
	IDirect3DTexture2* texture;
	uint16_t word10;
};


export struct struct_dword_905B84 {
	uint8_t byte0;
	uint8_t gap0;
	uint8_t byte1;
	uint8_t byte2;
	int16_t x;
	int16_t y;
	uint8_t gap8[2];
	uint8_t byteA;
	uint8_t byteB;
	uint8_t flags;
	uint8_t byteD;
	uint8_t gap2;
	uint8_t byteF;
	uint8_t gap3[2];
	uint16_t word12;
	uint16_t word14;
	uint8_t gap5[6];
	uint16_t word1C;
	uint8_t gap6[2];
	void* vp20;
};


export constexpr float SMALL_TEXT_SCALE = 10.f / 12.f;


export Func<0x444340, void,
	int16_t,	// x
	int16_t,	// y
	uint8_t,	// a3
	uint16_t	// a4
> DrawNumTiny;

export Func<0x461E50, void,
	uint8_t,	// index
	uint8_t		// cmdSize
> PushGpuCommand;

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
	uint8_t,	// command
	uint32_t,	// a5
	D3DCOLOR*,	// outColour
	D3DCOLOR*	// outSpecular
> GetD3DCOLOR;

export Func<0x59FCA0, void,
	uint8_t,	// command
	uint16_t	// texturePage
> SetD3DRenderState_AlphaBlend;

export Func<0x59FD80, void,
	D3DSHADEMODE	// mode
> SetD3DShadeMode;

export Func<0x5A04C0, uint16_t*,
	uint16_t	// id
> GetCLUT;

export Func<0x5A3160, int,
	uint16_t,	// a1
	uint16_t,	// a2
	uint16_t,	// a3
	uint16_t	// a4
> sub_5A3160; // Something to do with getting character sprites?

export Func<0x5A7710, void,
	GpuCommand_TexturedRectWH*	// gpuCmd
> Init_GpuCommand_TexturedRectWH;

export Func<0x5A77C0, void,
	GpuCommand_SetDrawEnv*,	// gpuCmd
	BOOL,					// allowDraw
	BOOL,					// enableDither
	uint16_t,				// texturePage
	PSX_RECT*				// rect
> Init_GpuCommand_SetDrawEnv;

export Func<0x5A79A0, uint16_t,
	char,	// a1
	char,	// a2
	int,	// a3
	int		// a4
> CreateTexturePageValue;


export Accessor<		0x7C9F4C, float>				g_RenderScaleX;
export Accessor<		0x7C9F48, float>				g_RenderScaleY;
export Accessor<		0x7DED00, DRAWENV>				g_DrawEnv;
export ArrayAccessor<	0x7C9F50, FontGlyph>			g_FontGlyphs;
export PointerAccessor<	0x7E0670, GpuCommand>			g_GpuCommands;
export PointerAccessor<	0x7CC334, IDirectDraw4>			g_IDirectDraw4;
export PointerAccessor<	0x7CC350, IDirect3DDevice3>		g_IDirect3DDevice3;
export PointerAccessor<	0x905B84, struct_dword_905B84>	dword_905B84;


void SetGpuCommandTranslucent(GpuCommand* gpuCmd, bool value) {
	if (value)
		gpuCmd->flags |= (int)GpuCommandBlendType::Translucent;
}


export void DrawRectWH(uint16_t x, uint16_t y, uint16_t width, uint16_t height, Colour colour, bool blend) {
	auto gpuCmd = (GpuCommand_RectWH*)*g_GpuCommands;
	gpuCmd->colour = colour;
	gpuCmd->command = (int)GpuCommandType::RectWH;

	gpuCmd->v1 = { (float)x, (float)y, 0.01f };
	gpuCmd->size = { (float)width, (float)height };

	SetGpuCommandTranslucent(gpuCmd, blend);

	PushGpuCommand(1, sizeof(GpuCommand_RectWH));
}

export void DrawQuad(Vec2i p1, Vec2i p2, Vec2i p3, Vec2i p4, Colour colour, bool blend) {
	auto gpuCmd = (GpuCommand_Quad*)*g_GpuCommands;
	gpuCmd->colour = colour;
	gpuCmd->command = (int)GpuCommandType::Quad;

	gpuCmd->v1 = { (float)p1.x, (float)p1.y, 0.01f };
	gpuCmd->v2 = { (float)p2.x, (float)p2.y, 0.01f };
	gpuCmd->v3 = { (float)p3.x, (float)p3.y, 0.01f };
	gpuCmd->v4 = { (float)p4.x, (float)p4.y, 0.01f };

	SetGpuCommandTranslucent(gpuCmd, blend);

	PushGpuCommand(1, sizeof(GpuCommand_Quad));
}

export void DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, Colour colour) {
	auto gpuCmd = (GpuCommand_Line*)*g_GpuCommands;
	gpuCmd->colour = colour;
	gpuCmd->command = (int)GpuCommandType::Line;

	gpuCmd->v1 = { (float)x1, (float)y1, 0.01f };
	gpuCmd->v2 = { (float)x2, (float)y2, 0.01f };

	PushGpuCommand(1, sizeof(GpuCommand_Line));
}

export void SetBlendMode(int a1, int a2, int a3, int a4) {
	auto gpuCmd = (GpuCommand_SetDrawEnv*)*g_GpuCommands;
	gpuCmd->command = (int)GpuCommandType::SetDrawEnv;

	auto flags = CreateTexturePageValue(a1, a2, a3, a4);
	Init_GpuCommand_SetDrawEnv(gpuCmd, 0, 0, flags, 0);

	PushGpuCommand(1, sizeof(GpuCommand_SetDrawEnv));
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

	DrawRectWH(x, y + 2, width, height - 4, colour, true);	// Background
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
	DrawRectWH(x + 2, y + 2, width - 4, 1, colour, true);			// Top line
	DrawRectWH(x + 2, y + 2, 1, height - 4, colour, true);			// Left line

	SetBlendMode(0, 0, 960, 0);
	DrawRectWH(x + 2, y + height - 3, width - 4, 1, colour, true);	// Bottom line
	DrawRectWH(x + width - 3, y + 2, 1, height - 4, colour, true);	// Right line
}
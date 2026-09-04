module;

#include <cstdint>
#include <d3d.h>
#include <ddraw.h>

export module bof3.render;

import bof3ext.helpers;
import bof3.math;
import bof3.dat;


export enum class GpuPrimType {
	Tri = 8,
	TriTexture,
	Quad,
	QuadTexture,
	TriGouraud,
	TriTextureGouraud,
	QuadGouraud,
	QuadTextureGouraud,
	Line,
	_UNUSED_0,
	TwoLines,
	ThreeLines,
	LineGouraud,
	_UNUSED_1,
	TwoLinesGouraud,
	ThreeLinesGouraud,
	RectWH,
	RectWHTexture,
	Point,
	TextGlyph,
	_UNUSED_2,
	Rect8Texture,
	_UNUSED_3,
	Rect16Texture,
	_UNUSED_4,
	Sprite,

	SetDrawEnv = 58
};

export enum class GpuPrimBlendType {
	RawTexture = 0x1,
	Translucent = 0x2,
};

export enum class SetDrawEnvFlags : uint8_t {
	AllowDraw = 0x1,
	EnableDither = 0x2,
};


export union TexturePageAttribute {
	struct {
		uint16_t x : 4;
		uint16_t y : 1;
		uint16_t blend : 2;
		uint16_t format : 2;
		uint16_t dither : 1;
	};

	uint16_t value;
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

export struct ENV {
	DISPENV disp;
	DRAWENV env;
	void* ordering_table[8];
};


export struct GpuPrim {
	void* tag;
	Vec3b colour;
	union {
		uint8_t value;
		struct {
			uint8_t flags : 2;
			uint8_t command : 6;
		};
	};
};

export struct GpuPrim_Tri : GpuPrim {
	Vec3f v1, v2, v3;
};

export struct GpuPrim_TriTexture : GpuPrim {
	Vec3f v1;
	Vec2b t1;
	uint16_t palette;
	Vec3f v2;
	Vec2b t2;
	uint16_t texturePage;
	Vec3f v3;
	Vec2b t3;
	uint16_t _unused_0;
};

export struct GpuPrim_Quad : GpuPrim_Tri {
	Vec3f v4;
};

export struct GpuPrim_QuadTexture : GpuPrim_TriTexture {
	Vec3f v4;
	Vec2b t4;
	uint16_t _unused_1;
};

export struct GpuPrim_TriGouraud : GpuPrim {
	Vec3f v1;
	Vec3b c2;
	Vec3f v2;
	Vec3b c3;
	Vec3f v3;
};

export struct GpuPrim_TriTextureGouraud : GpuPrim {
	Vec3f v1;
	Vec2b t1;
	uint16_t palette;
	Vec3b c2;
	Vec3f v2;
	Vec2b t2;
	uint16_t texturePage;
	Vec3b c3;
	Vec3f v3;
	Vec2b t3;
	uint16_t _unused_0;
};

export struct GpuPrim_QuadGouraud : GpuPrim_TriGouraud {
	Vec3b c4;
	Vec3f v4;
};

export struct GpuPrim_QuadTextureGouraud : GpuPrim_TriTextureGouraud {
	Vec3b c4;
	Vec3f v4;
	Vec2b t4;
	uint16_t _unused_1;
};

export struct GpuPrim_Line : GpuPrim {
	Vec3f v1, v2;
};

export struct GpuPrim_TwoLines : GpuPrim_Line {
	Vec3f v3;
};

export struct GpuPrim_ThreeLines : GpuPrim_TwoLines {
	Vec3f v4;
};

export struct GpuPrim_LineGouraud : GpuPrim {
	Vec3f v1;
	Vec3b c2;
	Vec3f v2;
};

export struct GpuPrim_TwoLinesGouraud : GpuPrim_LineGouraud {
	Vec3b c3;
	Vec3f v3;
};

export struct GpuPrim_ThreeLinesGouraud : GpuPrim_TwoLinesGouraud {
	Vec3b c4;
	Vec3f v4;
};

export struct GpuPrim_Point : GpuPrim {
	Vec3f v1;
};

export struct GpuPrim_Rect8 : GpuPrim {
	Vec3f v1;
};

export struct GpuPrim_Rect16 : GpuPrim_Rect8 {

};

export struct GpuPrim_RectWH : GpuPrim_Rect8 {
	Vec2f size;
};

export struct GpuPrim_Rect8Texture : GpuPrim_Rect8 {
	Vec2b t1;
	uint16_t palette;
};

export struct GpuPrim_Rect16Texture : GpuPrim_Rect8Texture {

};

export struct GpuPrim_RectWHTexture : GpuPrim_Rect8Texture {
	Vec2us size;
};

export struct GpuPrim_SetDrawEnv {
	uint8_t gap0[4];
	uint16_t texturePage;
	SetDrawEnvFlags flags;
	uint8_t command;
	PSX_RECT* rect;
};

export struct GpuPrim_TextGlyph : GpuPrim {
	Vec2s v1;
	Vec2b t1;
	uint16_t paletteIdx;
	Vec2s v2;
	Vec2b t2;
	uint16_t charCode;
	Vec2s v3;
	Vec2b t3;
	uint16_t _unused_0;
	Vec2s v4;
	Vec2b t4;
	uint16_t _unused_1;
};

export struct GpuPrim_236 : GpuPrim {
	PSX_RECT rect;
	uint32_t x;
	uint32_t y;
};

export struct GpuPrim_Sprite : GpuPrim {
	Vec2f position;
	Vec2f scale;
	uint16_t blockIndex;
	uint16_t blockCount;
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

struct UnkStruct_UI {
	uint8_t byte0;
	uint8_t gap0;
	uint8_t byte1;
	uint8_t byte2;
	int16_t x;
	int16_t y;
	uint8_t flags;
	uint8_t byte9;
	uint8_t index;
	uint8_t byteB;
	uint8_t flags2;
	uint8_t byteD;
	uint8_t gap2;
	uint8_t byteF;
	uint16_t word10;
	uint16_t word12;
	uint16_t word14;
	uint8_t gap5[6];
	uint16_t word1C;
	uint8_t gap6[2];
	void* vp20;
};

export struct UnkStruct_7 {
	uint32_t dword0;
	void* vp4;
};

export struct TextureBlockInfo {
	int16_t relX;
	int8_t relY;
	uint8_t size;
	uint16_t texturePage;
	uint8_t tx;
	uint8_t ty;
};


export constexpr float SMALL_TEXT_SCALE = 10.f / 12.f;
export constexpr float LARGE_TEXT_SCALE = 16.f / 12.f;


export typedef Func<0x444340, void,
	int16_t,	// x
	int16_t,	// y
	uint8_t,	// a3
	uint16_t	// a4
> DrawNumTiny;

export typedef Func<0x461E50, void,
	uint8_t,	// index
	uint8_t		// cmdSize
> PushGpuPrim_0;

export typedef Func<0x516B30, uint8_t*,
	int16_t,	// x
	int16_t,	// y
	uint32_t,	// colourIdx
	uint8_t,	// len
	const char*	// text
> DrawString;

export typedef Func<0x516D50, void,
	int16_t,	// x
	int16_t,	// y
	int16_t,	// w
	int16_t,	// h
	int16_t,	// a5
	int8_t,		// a6
	uint16_t	// a7
> sub_516D50;

export typedef Func<0x516E70, const char*,
	int16_t,	// x
	int16_t,	// y
	uint8_t,	// colourIdx
	uint8_t,	// len
	const char* // text
> DrawStringSmall;

export typedef Func<0x516F60, void,
	int16_t,	// x
	int16_t,	// y
	uint8_t,	// colourIdx
	const char*	// text
> DrawStringLarge;

export typedef Func<0x517090, void,
	int16_t,	// x
	int16_t,	// y
	uint32_t,	// colourIdx
	const char* // text
> DrawStringNumFont;

export typedef Func<0x59FBA0, void,
	uint32_t,	// r
	uint32_t,	// g
	uint32_t,	// b
	uint8_t,	// command
	uint32_t,	// a5
	D3DCOLOR*,	// outColour
	D3DCOLOR*	// outSpecular
> GetD3DCOLOR;

export typedef Func<0x59FCA0, void,
	uint8_t,	// command
	uint16_t	// texturePage
> SetD3DRenderState_AlphaBlend;

export typedef Func<0x59FD80, void,
	D3DSHADEMODE	// mode
> SetD3DShadeMode;

export typedef Func<0x5A04C0, uint16_t*,
	uint16_t	// id
> GetCLUT;

export typedef Func<0x5A3160, int,
	uint16_t,	// a1
	uint16_t,	// a2
	uint16_t,	// a3
	uint16_t	// a4
> sub_5A3160; // Something to do with getting character sprites?

export typedef Func<0x5A0C40, void,
	GpuPrim_QuadTexture*	// prim
> ProcessGpuPrim_QuadTexture;

export typedef Func<0x5A17A0, void,
	GpuPrim_Line*	// prim
> ProcessGpuPrim_Line;

export typedef Func<0x5A2300, void,
	GpuPrim_RectWHTexture*	// prim
> ProcessGpuPrim_RectWHTexture;

export typedef Func<0x5A2520, void,
	GpuPrim_Rect8Texture*	// prim
> ProcessGpuPrim_Rect8Texture;

export typedef Func<0x5A2710, void,
	GpuPrim_Rect16Texture*	// prim
> ProcessGpuPrim_Rect16Texture;

export typedef Func<0x5A2900, void,
	GpuPrim_TextGlyph*	// prim
> ProcessGpuPrim_TextGlyph;

export typedef Func<0x5A2EB0, void,
	GpuPrim_Sprite*	// prim
> ProcessGpuPrim_Sprite;

export typedef Func<0x5A7710, void,
	GpuPrim_RectWHTexture*	// prim
> Init_GpuPrim_RectWHTexture;

export typedef Func<0x5A7650, void,
	GpuPrim_Line*	// prim
> Init_GpuPrim_Line;

export typedef Func<0x5A77C0, void,
	GpuPrim_SetDrawEnv*,	// prim
	BOOL,					// allowDraw
	BOOL,					// enableDither
	uint16_t,				// texturePage
	PSX_RECT*				// rect
> Init_GpuPrim_SetDrawEnv;

export typedef Func<0x5A78E0, void,
	DISPENV*,	// disp
	short,		// x
	short,		// y
	short,		// w
	short		// h
> SetDefDispEnv;

export typedef Func<0x5A7910, void,
	DRAWENV*,	// env
	short,		// x
	short,		// y
	short,		// w
	short		// h
> SetDefDrawEnv;

export typedef Func<0x5A79A0, uint16_t,
	char,	// a1
	char,	// a2
	int,	// a3
	int		// a4
> CreateTexturePageValue;

export typedef Func<0x5A79E0, uint16_t,
	uint16_t,	// x
	uint16_t	// y
> CreatePaletteIdx;

export typedef Func<0x5A7AE0, void,
	int,	// x
	int		// y
> SetGeomOffset;


export typedef Accessor<        0x7C9F4C, float>            g_RenderScaleX;
export typedef Accessor<        0x7C9F48, float>            g_RenderScaleY;
export typedef Accessor<        0x7DED00, DRAWENV>          g_DrawEnv;

export typedef ArrayAccessor<   0x6BEA18, TextureBlockInfo> g_TextureBlockInfos;
export typedef ArrayAccessor<   0x6C2A40, UnkStruct_7>      g_CLUT;
export typedef ArrayAccessor<   0x7C9F50, FontGlyph>        g_FontGlyphs;

export typedef PointerAccessor< 0x905B84, UnkStruct_UI>     struct_905B84;
export typedef PointerAccessor< 0x7CC334, IDirectDraw4>     g_IDirectDraw4;
export typedef PointerAccessor< 0x7CC350, IDirect3DDevice3>	g_IDirect3DDevice3;
export typedef PointerAccessor< 0x7E0670, GpuPrim>          g_GpuPrims;
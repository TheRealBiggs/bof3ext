module;

#include <cstdint>
#include <d3d.h>
#include <ddraw.h>

export module bof3.render;

import bof3ext.helpers;


export struct Colour {
	uint8_t r, g, b;
};

export struct Vec2f {
	float x, y;
};

export struct Vec2i {
	int32_t x, y;
};

struct Vec3f {
	float x, y, z;
};

struct DrawCommand {
	uint8_t gap0[4];
	Colour colour;
	uint8_t type;
};

struct DrawCommand_SetBlendMode : public DrawCommand {
	void* vp8;
};

struct DrawCommand_UntexturedPlane : public DrawCommand {
	Vec3f position;
	Vec2f size;
};

struct DrawCommand_UntexturedQuad : public DrawCommand {
	Vec3f vertices[4];
};

struct DrawCommand_Line : public DrawCommand {
	Vec3f v1;
	Vec3f v2;
};

export struct DrawCommand_UnkIcon0 : public DrawCommand {
	float x;
	float y;
	float float10;
	uint8_t spriteIdx;
	uint8_t spritesheetId;
	uint16_t paletteId;
	uint8_t gap[8];
};

export struct FontGlyph {
	uint16_t charCode;
	uint16_t paletteIndex;
	uint32_t dword4;
	IDirectDrawSurface4* surface;
	IDirect3DTexture2* texture;
	uint16_t word10;
};


export Func<0x516B30, uint8_t*, int16_t /* x */, int16_t /* y */, uint32_t /* paletteIdx */, uint8_t /* len */, const char* /* text */>		DrawString;
export Func<0x516E70, const char*, int16_t /* x */, int16_t /* y */, uint8_t /* paletteIdx */, uint8_t /* len */, const char* /* text */>	DrawStringSmall;
export Func<0x444340, void, int16_t /* x */, int16_t /* y */, uint8_t /* a3 */, uint16_t /* a4 */>											DrawNumTiny;
export Func<0x517090, void, int16_t /* x */, int16_t /* y */, uint32_t /* paletteIdx */, const char* /* text */>							DrawStringNumFont;
export Func<0x5A04C0, uint16_t*, uint16_t /* id */>																							GetPalette;

Func<0x461E50, void, uint8_t /* index */, uint8_t /* cmdSize */> PushDrawCommand;


export Accessor<		0x905B84, uint32_t>			dword_905B84;
export ArrayAccessor<	0x7C9F50, FontGlyph>		g_FontGlyphs;
export PointerAccessor<	0x7E0670, DrawCommand>		g_DrawCommands;
export PointerAccessor<	0x7CC334, IDirectDraw4>		g_IDirectDraw4;
export PointerAccessor<	0x7CC350, IDirect3DDevice3> g_IDirect3DDevice3;


void SetDrawCommandBlend(DrawCommand* drawCmd, bool value) {
	if (value)
		drawCmd->type |= 2;
}


export void DrawUntexturedPlane(uint16_t x, uint16_t y, uint16_t width, uint16_t height, Colour colour, bool blend) {
	auto drawCmd = (DrawCommand_UntexturedPlane*)*g_DrawCommands;
	drawCmd->colour = colour;
	drawCmd->type = 96;

	drawCmd->position = { (float)x, (float)y, 0.01f };
	drawCmd->size = { (float)width, (float)height };

	SetDrawCommandBlend(drawCmd, blend);

	PushDrawCommand(1, sizeof(DrawCommand_UntexturedPlane));
}


export void DrawUntexturedQuad(Vec2i p1, Vec2i p2, Vec2i p3, Vec2i p4, Colour colour, bool blend) {
	auto drawCmd = (DrawCommand_UntexturedQuad*)*g_DrawCommands;
	drawCmd->colour = colour;
	drawCmd->type = 40;

	drawCmd->vertices[0] = { (float)p1.x, (float)p1.y, 0.01f };
	drawCmd->vertices[1] = { (float)p2.x, (float)p2.y, 0.01f };
	drawCmd->vertices[2] = { (float)p3.x, (float)p3.y, 0.01f };
	drawCmd->vertices[3] = { (float)p4.x, (float)p4.y, 0.01f };

	SetDrawCommandBlend(drawCmd, blend);

	PushDrawCommand(1, sizeof(DrawCommand_UntexturedQuad));
}


export void DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, Colour colour) {
	auto drawCmd = (DrawCommand_Line*)*g_DrawCommands;
	drawCmd->colour = colour;
	drawCmd->type = 40;

	drawCmd->v1 = { (float)x1, (float)y1, 0.01f };
	drawCmd->v2 = { (float)x2, (float)y2, 0.01f };

	PushDrawCommand(1, sizeof(DrawCommand_Line));
}


export void SetBlendMode(int a1, int a2, int a3, int a4) {
	auto drawCmd = (DrawCommand_SetBlendMode*)*g_DrawCommands;
	drawCmd->type = 232;

	// Looks like `colour` is reused for something else
	uint16_t flags = (a4 >> 4) & 0x10 | (a3 >> 6) & 0xF | (4 * (a4 & 0x200 | (8 * (a2 & 3 | (4 * (a1 & 3))))));
	drawCmd->colour.r = flags & 0xFF;
	drawCmd->colour.g = (flags >> 8) & 0xFF;
	drawCmd->colour.b = 0;

	drawCmd->vp8 = nullptr;

	PushDrawCommand(1, sizeof(DrawCommand_SetBlendMode));
}
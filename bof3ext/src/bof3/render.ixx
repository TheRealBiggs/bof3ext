module;

#include <cstdint>
#include <d3d.h>
#include <ddraw.h>

export module bof3.render;

import bof3ext.helpers;
import bof3ext.math;


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
	uint8_t type;
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

export struct DrawCommand_UnkIcon0 : public DrawCommand {
	float x;
	float y;
	float float10;
	uint8_t spriteIdx;
	uint8_t spritesheetId;
	uint16_t paletteId;
	uint8_t gap[8];
};

export struct DrawCommand_CharacterSprite : DrawCommand {
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


export Func<0x516B30, uint8_t*, int16_t /* x */, int16_t /* y */, uint32_t /* paletteIdx */, uint8_t /* len */, const char* /* text */>		DrawString;
export Func<0x516E70, const char*, int16_t /* x */, int16_t /* y */, uint8_t /* paletteIdx */, uint8_t /* len */, const char* /* text */>	DrawStringSmall;
export Func<0x444340, void, int16_t /* x */, int16_t /* y */, uint8_t /* a3 */, uint16_t /* a4 */>											DrawNumTiny;
export Func<0x517090, void, int16_t /* x */, int16_t /* y */, uint32_t /* paletteIdx */, const char* /* text */>							DrawStringNumFont;
export Func<0x5A04C0, uint16_t*, uint16_t /* id */>																							GetPalette;
export Func<0x5A3160, int, uint16_t /* a1 */, uint16_t /* a2 */, uint16_t /* a3 */, uint16_t /* a4 */>										sub_5A3160; // Something to do with getting character sprites?

Func<0x461E50, void, uint8_t /* index */, uint8_t /* cmdSize */> PushDrawCommand;


export Accessor<		0x905B84, uint32_t>			dword_905B84;
export Accessor<		0x7C9F4C, float>			g_RenderScaleX;
export Accessor<		0x7C9F48, float>			g_RenderScaleY;
export ArrayAccessor<	0x7C9F50, FontGlyph>		g_FontGlyphs;
export PointerAccessor<	0x7E0670, DrawCommand>		g_DrawCommands;
export PointerAccessor<	0x7CC334, IDirectDraw4>		g_IDirectDraw4;
export PointerAccessor<	0x7CC350, IDirect3DDevice3> g_IDirect3DDevice3;


void SetDrawCommandBlend(DrawCommand* drawCmd, bool value) {
	if (value)
		drawCmd->type |= 2;
}


export void DrawPlane(uint16_t x, uint16_t y, uint16_t width, uint16_t height, Colour colour, bool blend) {
	auto drawCmd = (DrawCommand_Plane*)*g_DrawCommands;
	drawCmd->colour = colour;
	drawCmd->type = 96;

	drawCmd->position = { (float)x, (float)y, 0.01f };
	drawCmd->size = { (float)width, (float)height };

	SetDrawCommandBlend(drawCmd, blend);

	PushDrawCommand(1, sizeof(DrawCommand_Plane));
}


export void DrawQuad(Vec2i p1, Vec2i p2, Vec2i p3, Vec2i p4, Colour colour, bool blend) {
	auto drawCmd = (DrawCommand_Quad*)*g_DrawCommands;
	drawCmd->colour = colour;
	drawCmd->type = 40;

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
	drawCmd->type = 40;	// TODO: Fix this

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
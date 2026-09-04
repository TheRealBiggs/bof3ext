module;

#include <cstdint>

export module bof3ext.renderHelpers;

import bof3.dat;
import bof3.render;


namespace {
	void SetGpuPrimTranslucent(struct GpuPrim* prim, bool value) {
		if (value)
			prim->flags |= (int)GpuPrimBlendType::Translucent;
	}
}


export void DrawRectWH(uint16_t x, uint16_t y, uint16_t width, uint16_t height, Vec3b colour, bool blend) {
	auto prim = (GpuPrim_RectWH*)g_GpuPrims::Get();
	prim->colour = colour;
	prim->command = (int)GpuPrimType::RectWH;

	prim->v1 = { (float)x, (float)y, 0.01f };
	prim->size = { (float)width, (float)height };

	SetGpuPrimTranslucent(prim, blend);

	PushGpuPrim_0::Call(1, sizeof(GpuPrim_RectWH));
}

export void DrawQuad(Vec2i p1, Vec2i p2, Vec2i p3, Vec2i p4, Vec3b colour, bool blend) {
	auto prim = (GpuPrim_Quad*)g_GpuPrims::Get();
	prim->colour = colour;
	prim->command = (int)GpuPrimType::Quad;

	prim->v1 = { (float)p1.x, (float)p1.y, 0.01f };
	prim->v2 = { (float)p2.x, (float)p2.y, 0.01f };
	prim->v3 = { (float)p3.x, (float)p3.y, 0.01f };
	prim->v4 = { (float)p4.x, (float)p4.y, 0.01f };

	SetGpuPrimTranslucent(prim, blend);

	PushGpuPrim_0::Call(1, sizeof(GpuPrim_Quad));
}

export void DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, Vec3b colour) {
	auto prim = (GpuPrim_Line*)g_GpuPrims::Get();
	prim->colour = colour;
	prim->command = (int)GpuPrimType::Line;

	prim->v1 = { (float)x1, (float)y1, 0.01f };
	prim->v2 = { (float)x2, (float)y2, 0.01f };

	PushGpuPrim_0::Call(1, sizeof(GpuPrim_Line));
}

export void SetBlendMode(int a1, int a2, int a3, int a4) {
	auto prim = (GpuPrim_SetDrawEnv*)g_GpuPrims::Get();
	prim->command = (int)GpuPrimType::SetDrawEnv;

	auto flags = CreateTexturePageValue::Call(a1, a2, a3, a4);
	Init_GpuPrim_SetDrawEnv::Call(prim, 0, 0, flags, 0);

	PushGpuPrim_0::Call(1, sizeof(GpuPrim_SetDrawEnv));
}

export void DrawBorderedPanel(int16_t x, int16_t y, int16_t width, int16_t height) {
	SetBlendMode(0, 0, 960, 0);

	auto wndColourIdx = ((uint8_t*)0x9039E0)[122] * 64;
	auto wndColour = *(uint16_t*)&g_DatChunk_0_8200::At(wndColourIdx + 40);
	Vec3b colour = {
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
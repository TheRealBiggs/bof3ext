module;

#include <cmath>
#include <cstdint>

export module bof3ext.hooks:widescreen;

import bof3ext.helpers;
import bof3ext.configManager;
import bof3.render;


struct DrawRanges {
	float terrainMin1, terrainMax1;
	float terrainMin2, terrainMax2;
	float objectMin1, objectMax1;
	float objectMin2, objectMax2;
	float spriteMin1, spriteMax1;
	int16_t spriteMin2, spriteMax2;
	float unknownMin1, unknownMax1;
	float unknownMax2;
};


void PatchDrawRangesForWidescreen(float diff) {
	static DrawRanges r{ 0 };

	r = {
		.terrainMin1 = -50.f - diff,
		.terrainMax1 = 370.f + diff,
		.terrainMin2 = -200.f - diff,
		.terrainMax2 = 520.f + diff,
		.objectMin1 = -100.f - diff,
		.objectMax1 = 420.f + diff,
		.objectMin2 = -80.f - diff,
		.objectMax2 = 400.f + diff,
		.spriteMin1 = -60.f - diff,
		.spriteMax1 = 380.f + diff,
		.spriteMin2 = (int16_t)(384 + diff),
		.spriteMax2 = (int16_t)(-64 - diff),
		.unknownMin1 = -20.f - diff,
		.unknownMax1 = 340.f + diff,
		.unknownMax2 = 320.f + diff
	};

	WriteProtectedMemory(0x5109BB, &r.terrainMin1);
	WriteProtectedMemory(0x5109D2, &r.terrainMax1);
	WriteProtectedMemory(0x51097E, &r.terrainMin2);
	WriteProtectedMemory(0x510991, &r.terrainMax2);
	WriteProtectedMemory(0x56EDFA, &r.terrainMin1);
	WriteProtectedMemory(0x56EE14, &r.terrainMax1);

	WriteProtectedMemory(0x4CEC09, &r.objectMin1);
	WriteProtectedMemory(0x4CEC1F, &r.objectMax1);
	WriteProtectedMemory(0x5700D1, &r.objectMin1);
	WriteProtectedMemory(0x5700E7, &r.objectMax1);
	WriteProtectedMemory(0x570319, &r.objectMin2);
	WriteProtectedMemory(0x570333, &r.objectMax2);

	WriteProtectedMemory(0x4CF319, &r.spriteMin1);
	WriteProtectedMemory(0x4CF335, &r.spriteMax1);
	WriteProtectedMemory(0x4FF6A3, &r.spriteMin1);
	WriteProtectedMemory(0x4FF6BF, &r.spriteMax1);
	WriteProtectedMemory(0x571366, &r.spriteMin1);
	WriteProtectedMemory(0x571386, &r.spriteMax1);
	WriteProtectedMemory(0x59360B, r.spriteMin2);
	WriteProtectedMemory(0x593615, r.spriteMax2);

	WriteProtectedMemory(0x5054E3, &r.unknownMin1);
	WriteProtectedMemory(0x5054FA, &r.unknownMax1);
	WriteProtectedMemory(0x59293A, &r.unknownMax2);
}


Func<0x462560, DrawCommand_TexturedPlane*,
	int16_t,	// x
	int16_t,	// y
	uint8_t,	// a3
	uint8_t,	// a4
	uint8_t		// a5
> sub_462560;
FuncHook<decltype(sub_462560)> sub_462560Hook = [](auto x, auto y, auto a3, auto a4, auto a5) {
	auto renderScale = ConfigManager::Get().GetRenderScale();
	auto wndSize = ConfigManager::Get().GetWindowSize();

	auto renderWidth = (wndSize.x / 320.0) / renderScale * 320;
	auto diff = (float)(renderWidth - 320.f);
	auto offset = diff / 2;

	return sub_462560.Original(x + offset, y, a3, a4, a5);
};


Func<0x576960, void,
	uint8_t,	// a1
	int16_t,	// x
	int16_t,	// y
	void*		// a4
> DrawSaveEntry;
FuncHook<decltype(DrawSaveEntry)> DrawSaveEntryHook = [](auto a1, auto x, auto y, auto a4) {
	auto renderScale = ConfigManager::Get().GetRenderScale();
	auto wndSize = ConfigManager::Get().GetWindowSize();

	auto renderWidth = (wndSize.x / 320.0) / renderScale * 320;
	auto diff = (float)(renderWidth - 320.f);
	auto offset = diff / 2;

	return DrawSaveEntry.Original(a1, x + offset, y, a4);
};


Func<0x573CE0, void,
	int16_t,	// x
	int16_t,	// y
	int16_t,	// w
	int16_t,	// h
	int,		// a5
	int			// a6
> DrawSelectedFrameOutline;
FuncHook<decltype(DrawSelectedFrameOutline)> DrawSelectedFrameOutlineHook = [](auto x, auto y, auto w, auto h, auto a5, auto a6) {
	auto renderScale = ConfigManager::Get().GetRenderScale();
	auto wndSize = ConfigManager::Get().GetWindowSize();

	auto renderWidth = (wndSize.x / 320.0) / renderScale * 320;
	auto diff = (float)(renderWidth - 320.f);
	auto offset = diff / 2;

	DrawSelectedFrameOutline.Original(x + offset, y, w, h, a5, a6);
};


export void ApplyWidescreenPatches() {
	EnableHook(sub_462560, sub_462560Hook);
	EnableHook(DrawSaveEntry, DrawSaveEntryHook);
	EnableHook(DrawSelectedFrameOutline, DrawSelectedFrameOutlineHook);

	auto renderScale = ConfigManager::Get().GetRenderScale();
	auto wndSize = ConfigManager::Get().GetWindowSize();

	auto renderWidth = (wndSize.x / 320.0) / renderScale * 320;
	auto diff = (float)(renderWidth - 320.f);

	PatchDrawRangesForWidescreen(diff);

	auto offset = diff / 2;

	// Widescreen fix for start menu BG
	WriteProtectedMemory(0x588C26, (uint8_t)(26 + offset));
	WriteProtectedMemory(0x588C36, (uint16_t)(266 + offset));
	WriteProtectedMemory(0x588C69, (uint8_t)(-6 + offset));
	WriteProtectedMemory(0x588C76, (uint16_t)(218 + offset));

	// Widescreen fix for start menu items
	WriteProtectedMemory(0x58893B, (uint32_t)(renderWidth / 2));
	WriteProtectedMemory(0x588A19, (uint32_t)(renderWidth / 2));

	// Set width of all start menu items to 254
	WriteProtectedMemory(0x5888E8, (uint8_t)254);
	WriteProtectedMemory(0x5888ED, (uint8_t)254);
	WriteProtectedMemory(0x5888F2, (uint8_t)254);

	// Widescreen fix for save menu
	WriteProtectedMemory(0x5881AC, (uint8_t)(20 + offset));	// Top panel X
	WriteProtectedMemory(0x5881C7, (uint8_t)(28 + offset));	// Top panel text
	WriteProtectedMemory(0x58820D, (uint8_t)(20 + offset));
	WriteProtectedMemory(0x588228, (uint8_t)(28 + offset));
	WriteProtectedMemory(0x58835C, (uint8_t)(20 + offset));
	WriteProtectedMemory(0x588377, (uint8_t)(28 + offset));
	WriteProtectedMemory(0x5883DD, (uint8_t)(20 + offset));
	WriteProtectedMemory(0x5884BE, (uint8_t)(20 + offset));
	WriteProtectedMemory(0x588515, (uint8_t)(28 + offset));
	WriteProtectedMemory(0x58857C, (uint8_t)(20 + offset));
	WriteProtectedMemory(0x5885A2, (uint8_t)(28 + offset));
	WriteProtectedMemory(0x5887C2, (uint8_t)(20 + offset));
	WriteProtectedMemory(0x5887E8, (uint8_t)(28 + offset));

	// Widescreen fix for black fade
	WriteProtectedMemory(0x4957CF, (float)renderWidth);

	// Widescreen fix for menu background
	WriteProtectedMemory(0x575732, (uint32_t)std::ceil(renderWidth / 32 / 2));	// Repeat background pattern to fit extended screen width.
																				// Each column is 32px wide and alternates between 2 patterns, meaning 64px per repeat.
}
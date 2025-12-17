module;

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>

export module bof3ext.hooks:gui.menu;

import bof3ext.helpers;
import bof3ext.configManager;
import bof3ext.glyphManager;
import bof3ext.textManager;
import bof3.character;
import bof3.item;
import bof3.gui;
import bof3.render;
import bof3.text;


struct UnkStruct_D {
	uint8_t gap0[4];
	uint16_t x;
	uint16_t y;
	uint8_t gap1;
	uint8_t flags;
	uint8_t category;
	uint8_t gap3;
	uint8_t gap4;
	uint8_t gap5;
	uint8_t gap6[2];
	uint16_t gap7;
	uint16_t gap;
};


Func<0x5918E0, uint8_t, uint8_t /* a1 */> sub_5918E0;
Func<0x5919B0, uint16_t, uint8_t /* a1 */, uint8_t /* a2 */, uint8_t /* a3 */> sub_5919B0;
Func<0x590AB0, void, uint8_t /* characterId */, char* /* newEquip */, uint8_t* /* outColours */, uint16_t* /* outStats */> GetNewStats;
Func<0x57D360, void, int16_t /* x */, int16_t /* y */, uint8_t /* id */, bool /* greyed */> DrawItemIcon;
Func<0x57D860, void, int16_t /* x */, int16_t /* y */, int /* a3 */, char /* a4 */> sub_57D860;
Func<0x591940, void, uint8_t /* a1 */, uint8_t /* a2 */, char* /* a3 */> sub_591940;


Func<0x574890, void, int16_t /* x */, int16_t /* y */, uint8_t /* index */, uint8_t /* selectedTabIdx */> DrawMenuTabs;
auto DrawMenuTabsHook(auto x, auto y, auto index, auto selectedTabIdx) {
	auto count = byte_66383C[index * 5];
	auto startIndex = index * 5 + 1;

	auto advance = GlyphManager::Get().GetScaledGlyphAdvance();

	for (int i = 0; i < count; ++i) {
		DrawWindowBackground(x + 48 * i, y, 45, 20, 0, *(uint8_t*)0x903A5A);

		auto id = byte_66383C[startIndex + i];

		auto& text = TextManager::Get().GetMenuTabText(id);
		auto len = text.length();

		auto offset = std::round(advance / 2 * len);

		auto _x = (int16_t)(x + 22 - offset + i * 48);

		auto paletteIdx = 0;

		if ((selectedTabIdx < count && i != selectedTabIdx)
			|| (id == 18 && (*(char*)0x905BA2 & 1) == 0)
			|| (id == 8 && !sub_5918E0(15) && !sub_5919B0(0, 88, 0)))
			paletteIdx = 7;

		DrawString(_x, y + 3, paletteIdx, (uint8_t)len, text.c_str());

		auto a3 = (char*)0x663428;

		if (i == selectedTabIdx)
			a3 = (char*)0x663440;

		DrawUIGroup(x + 48 * i, y, a3, 0);
	}
}

Func<0x574EC0, void, uint8_t /* characterId */, int16_t /* x */, int16_t /* y */, char* /* a4 */, int16_t /* a5 */, uint8_t* /* a6 */> DrawEquipmentWindow;
auto DrawEquipmentWindowHook(auto characterId, auto x, auto y, auto a4, auto a5, auto a6) {
	DrawWindowBackground(x + 4, y + 4, 120, 163, 0, *(uint8_t*)0x903A5A);

	auto* character = &g_Characters[characterId];

	DrawString(x + 32, y + 7, 0, 5, character->name);
	DrawString(x + 5, y + 26, 0, 5, "Power");
	DrawString(x + 5, y + 39, 0, 7, "Defense");
	DrawString(x + 5, y + 52, 0, 12, "Intellect");
	DrawString(x + 5, y + 65, 0, 7, "Agility");

	char buf[6];

	auto advance = std::ceil(GlyphManager::Get().GetScaledGlyphAdvance());
	auto statXPos = x + 120 - advance * 3 - 5;

	if (a5 == 0)
		statXPos -= advance * 4;

	sprintf_s(buf, "%i", character->power);
	DrawString(statXPos, y + 26, 0, 3, buf);

	sprintf_s(buf, "%i", character->defence);
	DrawString(statXPos, y + 39, 0, 3, buf);

	sprintf_s(buf, "%i", character->intelligence);
	DrawString(statXPos, y + 52, 0, 3, buf);

	sprintf_s(buf, "%i", character->agility);
	DrawString(statXPos, y + 65, 0, 3, buf);

	if (a5 == 0) {
		uint8_t colours[4];
		uint16_t newStats[4];

		uint16_t arrow = EncodeUnicodeCharacter(u'»');
		GetNewStats(characterId, a4, colours, newStats);

		for (int i = 0; i < 4; ++i) {
			auto _y = y + 26 + 13 * i;

			DrawString(statXPos + advance * 3, _y, 0, 1, (char*)&arrow);

			auto colour = colours[i];
			auto newStat = newStats[i];

			sprintf_s(buf, "%i", newStat);
			DrawString(statXPos + advance * 4, _y, colour, 3, buf);
		}
	}

	const char* equipNames[6] = {
		GetItemName(1, character->weaponId),
		GetItemName(2, character->armorId1),
		GetItemName(2, character->armorId2),
		GetItemName(2, character->armorId3),
		GetItemName(3, character->accessoryId1),
		GetItemName(3, character->accessoryId2)
	};

	for (int i = 0; i < 6; ++i) {
		auto _y = y + i + 12 * i;

		DrawItemIcon(x + 6, _y + 87, g_EquipWindowItemIconIds[i], false);

		auto len = GetStringLength(equipNames[i]);
		DrawString(x + 17, _y + 85, 0, len, equipNames[i]);
	}

	DrawUIGroup(x, y, (char*)0x663458, 1);

	for (int i = 0; i < 7; ++i) {
		auto _y = y + 8 * i + 24;

		sub_57D860(x, _y, 4, 1);		// Left side of frame, top portion
		sub_57D860(x + 120, _y, 8, 1);	// Right side of frame, top portion
	}

	for (int i = 0; i < 9; ++i) {
		auto _y = y + 8 * i + 88;

		sub_57D860(x, _y, 4, 1);		// Left side of frame, bottom portion
		sub_57D860(x + 120, _y, 8, 1);	// Right side of frame, bottom portion
	}

	for (int i = 0; i < 14; ++i) {
		auto _x = x + 8 * i + 8;

		sub_57D860(_x, y + 80, 14, 1);	// Bottom of frame, top portion
		sub_57D860(_x, y + 160, 17, 1); // Bottom of frame, bottom portion
	}

	sub_57D860(x, y + 80, 13, 1);
	sub_57D860(x + 120, y + 80, 15, 1);
	sub_57D860(x, y + 160, 16, 1);
	sub_57D860(x + 120, y + 160, 18, 1);
}

Func<0x574610, void, int16_t /* x */, int16_t /* y */, int /* a3 */, int /* a4 */> DrawZennyPanel;
auto DrawZennyPanelHook(auto x, auto y, auto a3, auto a4) {
	auto diff = ConfigManager::Get().GetScaledRenderWidth() - 320;

	const auto& glyphMgr = GlyphManager::Get();
	auto advance = (int)std::ceil(GlyphManager::Get().GetScaledGlyphAdvance());
	auto largeAdvance = (int)std::ceil(GlyphManager::Get().GetScaledGlyphAdvance() * (16.0 / 12.0));

	DrawWindowBackground(x + 3 + diff, y + 3, 101, 16, 0, ((uint8_t*)0x9039E0)[122]);

	char buf[8];
	sprintf_s(buf, "%7d", a4);
	DrawStringLarge(x + 101 - 5 - advance * 2 - 7 * largeAdvance + diff, y + 4, 0, buf);

	*(uint16_t*)&buf[0] = EncodeUnicodeCharacter(u'ƶ');
	DrawString(x + 101 - 5 - advance + diff, y + 4, 0, 1, buf);
	DrawUIGroup(x + diff, y, (char*)0x6633A8, 0);

	for (int i = 0; i < 11; ++i)
		DrawUIGroup(x + 8 * i + diff, y, (char*)0x6633B4, 0);

	DrawUIGroup(x + diff, y, (char*)0x6633C0, 0);
}

Func<0x5738A0, void, int16_t /* x */, int16_t /* y */, uint8_t /* charId */> DrawStatusWindow;
auto DrawStatusWindowHook(auto x, auto y, auto charId) {
	DrawWindowBackground(x + 5, y + 3, 164, 45, 0, ((uint8_t*)0x9039E0)[122]);

	char buf[4];

	DrawStringNumFont(x + 11, y + 10, 0, "Power");
	sprintf_s(buf, "%3d", g_Characters[charId].power);
	DrawStringNumFont(x + 56, y + 10, 0, buf);

	DrawStringNumFont(x + 11, y + 23, 0, "Defense");
	sprintf_s(buf, "%3d", g_Characters[charId].defence);
	DrawStringNumFont(x + 56, y + 23, 0, buf);

	DrawStringNumFont(x + 83 + 6, y + 10, 0, "Intellect");
	sprintf_s(buf, "%3d", g_Characters[charId].intelligence);
	DrawStringNumFont(x + 146, y + 10, 0, buf);

	DrawStringNumFont(x + 83 + 6, y + 23, 0, "Agility");
	sprintf_s(buf, "%3d", g_Characters[charId].agility);
	DrawStringNumFont(x + 146, y + 23, 0, buf);

	// Status effect maybe?
	if (g_Characters[charId].byte1F != 255) {
		auto v3 = (char*)GetText(g_Characters[charId].byte1F + 273);
		sub_591940(0, 8u, v3);
		auto v4 = (char*)GetText(0x34u);
		DrawString(x + 11, y + 34, 0, 255, v4);
	}

	DrawUIGroup(x, y, (char*)0x663358, 0);

	for (int i = 0; i < 18; ++i)
		DrawUIGroup(x + 8 * i, y, (char*)0x663384, 0);

	DrawUIGroup(x + 16, y, (char*)0x663390, 0);
}


static const float HALF = 0.5f;


template<int X>
static void __declspec(naked) FixTextCentering() {
	static const int x = X;

	__asm {
		push ecx;				// Save ECX register (textLen)
		call GlyphManager::Get;
		mov ecx, eax;			// Move GlyphManager instance into ECX for __thiscall
		call GlyphManager::GetScaledGlyphAdvance;
		fmul[HALF];				// Divide ST0 (advance) by 2
		fimul[esp];				// Multiply ST0 by textLen
		fistp[esp];				// Move and truncate ST0 into space on stack (reserved by previous `push ecx`)
		mov edx, x;				// 77 is X offset of center of textbox
		pop ecx;				// Pop converted float (half textLen * advance) into ECX
		sub edx, ecx;			// Subtract converted float from 77 to get final X offset
		ret;
	}
}

static void __declspec(naked) FixTextCenteringMainMenu() {
	__asm {
		push edx;				// Save EDX register (&dword_905B84)
		push eax;				// Save EAX register (textLen)
		call GlyphManager::Get;
		mov ecx, eax;			// Move GlyphManager instance into ECX for __thiscall
		call GlyphManager::GetScaledGlyphAdvance;
		fmul[HALF];				// Divide ST0 (advance) by 2
		fimul[esp];				// Multiply ST0 by textLen
		fistp[esp];				// Move and truncate ST0 into space on stack (reserved by previous `push EAX`)
		pop ecx;				// Pop converted float (half textLen * advance) into ECX
		pop edx;				// Restore EDX
		mov ax, [edx + 4];		// Move dword_905B84->x into EAX
		ret;
	}
}


export void EnableGuiMenuHooks() {
	EnableHook(DrawMenuTabs, DrawMenuTabsHook);
	EnableHook(DrawEquipmentWindow, DrawEquipmentWindowHook);
	EnableHook(DrawZennyPanel, DrawZennyPanelHook);
	EnableHook(DrawStatusWindow, DrawStatusWindowHook);

	// Increase width of status window selection outline
	WriteProtectedMemory(0x66B090, (uint16_t)288);

	// Fix text centering for category in inventory window
	uint8_t code[12];
	code[0] = 0xE8;					// Call relative
	std::memset(&code[5], 0x90, 7);	// NOPs

	*(uint32_t*)&code[1] = (uint32_t)(&FixTextCentering<77>) - (0x575C7A + 5 /* 5 = size of CALL instruction */);

	WriteProtectedMemory(0x575C7A, code);

	// Fix text centering for category in equip window
	*(uint32_t*)&code[1] = (uint32_t)(&FixTextCentering<77>) - (0x5766EA + 5);

	WriteProtectedMemory(0x5766EA, code);

	// Fix text centering for unique item count in inventory window
	auto advance = GlyphManager::Get().GetScaledGlyphAdvance();
	auto offset = advance * SMALL_TEXT_SCALE / 2 * 7;	// 7 = strlen("xxx/yyy")
	offset = std::floor(113 - offset);

	WriteProtectedMemory(0x575CE4, (uint8_t)offset);

	// TODO: Fix text alignment for item count in item info

	// Fix text centering for submenu text in pause menu
	uint8_t code2[9];
	code2[0] = 0xE8;
	std::memset(&code2[5], 0x90, 4);
	*(uint32_t*)&code2[1] = (uint32_t)&FixTextCenteringMainMenu - (0x59A015 + 5);

	WriteProtectedMemory(0x59A015, code2);
	WriteProtectedMemory(0x59A023, (uint8_t)36);
}
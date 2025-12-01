module;

#include <cstdint>
#include <cstdio>

export module bof3ext.hooks:gui.menu;

import bof3ext.glyphManager;
import bof3ext.textManager;
import bof3ext.helpers;
import bof3.character;
import bof3.item;
import bof3.gui;
import bof3.render;
import bof3.text;

import std;


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
Func<0x57DF00, char, char* /* a1 */, uint8_t* /* a2 */, uint8_t* /* a3 */, uint8_t* /* a4 */> sub_57DF00;
Func<0x57D9A0, bool, char /* a1 */, char /* a2 */, uint8_t /* itemType */, uint8_t /* itemIdx */> sub_57D9A0;
Func<0x57DD10, void, int /* a1 */, uint8_t /* a2 */, int16_t /* x */, int16_t /* y */, uint8_t /* a5 */, uint8_t /* a6 */, uint8_t /* a7 */> sub_57DD10;
Func<0x57DBF0, void, int16_t /* x */, int16_t /* y */, uint32_t /* paletteIdx */, uint8_t /* itemType */, uint8_t /* itemIdx */, uint8_t /* itemCount */, bool /* iconGreyed */> DrawItemInventoryInfo;


Func<0x574890, void, int16_t /* x */, int16_t /* y */, uint8_t /* index */, uint8_t /* selectedTabIdx */> DrawMenuTabs;
FuncHook<decltype(DrawMenuTabs)> DrawMenuTabsHook = [](auto x, auto y, auto index, auto selectedTabIdx) {
	auto count = byte_66383C[index * 5];
	auto startIndex = index * 5 + 1;

	for (int i = 0; i < count; ++i) {
		DrawWindowBackground(x + 48 * i, y, 45, 20, 0, *(uint8_t*)0x903A5A);

		auto id = byte_66383C[startIndex + i];

		auto& text = TextManager::Get().GetMenuTabText(id);
		auto len = text.length();
		auto advance = (float)GlyphManager::Get().GetGlyphAdvance();
		advance /= 3.5;

		auto _x = (int16_t)(x + 22 - (len * advance / 2) + i * 48);

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
};


Func<0x574EC0, void, uint8_t /* characterId */, int16_t /* x */, int16_t /* y */, char* /* a4 */, int16_t /* a5 */, uint8_t* /* a6 */> DrawEquipmentWindow;
FuncHook<decltype(DrawEquipmentWindow)> DrawEquipmentWindowHook = [](auto characterId, auto x, auto y, auto a4, auto a5, auto a6) {
	DrawWindowBackground(x + 4, y + 4, 120, 163, 0, *(uint8_t*)0x903A5A);

	auto* character = &g_Characters[characterId];

	DrawString(x + 32, y + 7, 0, 5, character->name);
	DrawString(x + 5, y + 26, 0, 5, "Power");
	DrawString(x + 5, y + 39, 0, 7, "Defense");
	DrawString(x + 5, y + 52, 0, 12, "Intellect");
	DrawString(x + 5, y + 65, 0, 7, "Agility");

	char buf[6];

	auto advance = GlyphManager::Get().GetGlyphAdvance();
	advance /= 2;
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

		uint16_t arrow = u'»' | 0x8000;
		arrow = (arrow >> 8) | ((arrow & 0xFF) << 8);
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
};


Func<0x5759C0, void, UnkStruct_D* /* a1 */> DrawInventoryWindow;
FuncHook<decltype(DrawInventoryWindow)> DrawInventoryWindowHook = [](auto a1) {
	DrawWindowBackground(a1->x + 3, a1->y + 3, 153, 154, a1->flags, *(uint8_t*)0x903A5A);

	uint8_t v37;
	uint8_t v33;

	auto v31 = sub_57DF00((char*)&a1->gap3, &v37, &v33, (uint8_t*)&a1->gap);

	if (v33 + 9 > 0) {
		auto v5 = *((uint32_t*)0x656B00 + a1->category);
		auto itemIds = (uint8_t*)(v5 + v31);

		auto v6 = (uint8_t**)0x656B14;
		auto itemCounts = &v6[a1->category][v31];

		// TODO: Grey out key items, add scrolling support, and double-draw selected item
		for (int i = 0; i < v33 + 9; ++i) {
			if (itemIds[i] == 0)
				continue;

			auto v7 = ((uint8_t*)0x66972C)[((uint8_t*)0x904060)[*(uint8_t*)0x929F06 + 2]];
			auto notUsable = a1->flags || !sub_57D9A0(a1->gap1, v7, a1->category, itemIds[i]);

			DrawItemInventoryInfo(a1->x + 7, a1->y + 26 + i * 13, notUsable ? 7 : 0, a1->category, itemIds[i], itemCounts[i], notUsable);
		}
	}

	//DrawWindowBackground(a1->x + 3, a1->y + 3, 153, 20, a1->flags, *(uint8_t*)0x903A5A);
	//DrawWindowBackground(a1->x + 3, a1->y + 146, 153, 8, a1->flags, *(uint8_t*)0x903A5A);

	const auto& categoryText = TextManager::Get().GetCategoryText(a1->category);
	auto len = categoryText.length();
	auto advance = GlyphManager::Get().GetGlyphAdvance();
	advance /= 2;

	auto _x = (int16_t)(a1->x + 78 - (len * advance / 2));

	auto paletteIdx = a1->flags != 0 ? 7 : 0;

	DrawString(_x, a1->y + 7, paletteIdx, (uint8_t)len, categoryText.c_str());

	char buf[8];

	auto itemCount = GetUniqueItemCount(a1->category);
	sprintf_s(buf, "%3d/%3d", itemCount, a1->category == 4 ? 32 : 128);
	DrawString(a1->x + 113 - (7 * advance / 2), a1->y + 144, paletteIdx, 7, buf);

	auto v19 = (char*)0x66349C;

	if ((a1->gap7 & 2) == 0)
		v19 = (char*)0x663484;

	DrawUIGroup(a1->x, a1->y, v19, 1);	// Left side of category section

	v19 = (char*)0x6634A8;

	if ((a1->gap7 & 1) == 0)
		v19 = (char*)0x663490;

	DrawUIGroup(a1->x, a1->y, v19, 1);	// Right side of category section

	if ((a1->gap7 & 0xF0) != 0)
		a1->gap7 -= 16;
	else
		a1->gap7 = 0;

	for (int i = 0; i < 10; ++i)	// Center of category section
		sub_57D860(a1->x + 40 + 8 * i, a1->y, 1, 1);

	for (int i = 0; i < 15; ++i)	// Left side of frame
		sub_57D860(a1->x, a1->y + 24 + 8 * i, 4, 1);

	sub_57D860(a1->x, a1->y + 144, 25, 1);	// Bottom-left corner

	sub_57D860(a1->x + 144, a1->y + 24, 22, 1);	// Top section of scrollbar

	for (int i = 0; i < 12; ++i)	// Middle section of scrollbar
		sub_57D860(a1->x + 144, a1->y + 40 + 8 * i, 23, 1);

	sub_57D860(a1->x + 144, a1->y + 136, 24, 1);	// Bottom section of scrollbar

	for (int i = 0; i < 9; ++i)	// Left side of bottom section
		sub_57D860(a1->x + 8 + 8 * i, a1->y + 144, 26, 1);

	sub_57D860(a1->x + 80, a1->y + 144, 27, 1);	// Left side of item count section

	for (int i = 0; i < 6; ++i)	// Center of item count section
		sub_57D860(a1->x + 88 + 8 * i, a1->y + 144, 28, 1);

	sub_57D860(a1->x + 136, a1->y + 144, 29, 1);	// Right side of item count section

	if (a1->category == 4) {
		sub_57D860(a1->x + 8, a1->y, 50, 1);	// Cover left arrow in category section for key items
		sub_57D860(a1->x + 144, a1->y, 51, 1);	// Cover right arrow in category section for key items
	}

	sub_57DD10(*((uint32_t*)0x656B00 + a1->category), a1->gap3, a1->x + 144, a1->y + 24, 9, a1->category == 4 ? 32 : 128, 116);	// Scrollbar
};


FuncHook<decltype(DrawItemInventoryInfo)> DrawItemInventoryInfoHook = [](auto x, auto y, auto paletteIdx, auto itemType, auto itemIdx, auto itemCount, auto iconGreyed) {
	auto iconId = GetItemIconId(itemType, itemIdx);
	DrawItemIcon(x, y + 2, ((uint8_t*)0x663D60)[iconId], iconGreyed);

	auto name = GetItemName(itemType, itemIdx);
	auto len = GetStringLength(name);
	DrawString(x + 10, y, paletteIdx, len, name);

	if (itemCount > 1) {
		char buf[4];

		sprintf_s(buf, "x%2d", itemCount);
		DrawString(x + 109, y, paletteIdx, 3, buf);
	}
};


export void EnableGuiMenuHooks() {
	EnableHook(DrawMenuTabs, DrawMenuTabsHook);
	EnableHook(DrawEquipmentWindow, DrawEquipmentWindowHook);
	EnableHook(DrawInventoryWindow, DrawInventoryWindowHook);
	EnableHook(DrawItemInventoryInfo, DrawItemInventoryInfoHook);
}
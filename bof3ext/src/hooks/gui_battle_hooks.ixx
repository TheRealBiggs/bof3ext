module;

#include <cstdint>

export module bof3ext.hooks:gui.battle;

import bof3ext.helpers;
import bof3ext.textManager;
import bof3.battle;
import bof3.render;
import bof3.text;

import std;


struct RectS {
	short Left, Top, Right, Bottom;
};


Func<0x5A79A0, int, char /* a1 */, char /* a2 */, int /* a3 */, int /* a4 */>																		sub_5A79A0;
Func<0x5A77C0, void, int /* a1 */, int /* a2 */, int /* a3 */, short /* a4 */, int /* a5 */>														sub_5A77C0;
Func<0x461E50, void, uint8_t /* a1 */, uint8_t /* a2 */>																							sub_461E50;
Func<0x444900, void, int16_t /* x */, int16_t /* y */, uint8_t /* a3 */, uint8_t /* a4 */>															sub_444900;
Func<0x4449E0, void, int16_t /* x */, int16_t /* y */, uint8_t /* a3 */, uint32_t /* a4 */, uint8_t /* a5 */>										sub_4449E0;
Func<0x4447B0, void, int16_t /* x */, int16_t /* y */, uint8_t /* a3 */, uint8_t /* a4 */>															sub_4447B0;
Func<0x444A90, void, int16_t /* x */, int16_t /* y */, uint8_t /* a3 */, uint8_t /* a4 */, bool /* a5 */>											sub_444A90;
Func<0x444EB0, bool, uint8_t /* a1 */>																												sub_444EB0;
Func<0x444E00, bool, int16_t /* x */, int16_t /* y */, int16_t /* a3 */, int16_t /* a4 */, uint8_t /* a5 */, uint8_t /* a6 */, uint8_t /* a7 */>	sub_444E00;
Func<0x444D50, bool, int16_t /* x */, int16_t /* y */, int16_t /* a3 */, int16_t /* a4 */, uint8_t /* a5 */, uint8_t /* a6 */, uint8_t /* a7 */>	sub_444D50;


Func<0x443D90, void, int16_t /* x */, int16_t /* y */, uint8_t /* slot */> DrawBattleEnemyPanel;
FuncHook<decltype(DrawBattleEnemyPanel)> DrawBattleEnemyPanelHook = [](auto x, auto y, auto slot) {
	auto v3 = sub_5A79A0(0, 0, 960, 0);
	sub_5A77C0(*(int*)0x7E0670, 0, 0, v3, 0);
	sub_461E50(1u, 0xCu);
	auto v4 = (unsigned int)&((uint8_t*)0x80B780)[64 * *(uint8_t*)0x903A5A + 40];
	auto v5 = *(uint8_t*)v4 & 0x1F;
	auto v6 = y + 2;
	auto v4a = *(uint16_t*)v4;
	auto v8 = 8 * ((v4a >> 10) & 0x1F);
	auto v10 = 8 * v5;
	auto v9 = 8 * ((v4a >> 5) & 0x1F);
	auto _slot = slot - 3;

	sub_444900(x, y + 2, 8u, 1u);			// Panel background
	sub_4449E0(x + 6, y + 11, 9u, 0, 0);	// Health bar background
	sub_4447B0(x, y, 2u, 1u);				// Round top corners of panel
	sub_4447B0(x, y + 19, 3u, 1u);			// Round bottom corners of panel

	auto v7 = _slot;

	if (g_EnemyBattleDatas[v7].gap0 == 1)
		sub_444A90(x + 6, y + 10, *(uint8_t*)(*dword_905B84 + 11), *(uint8_t*)(*dword_905B84 + 13), true);	// Health bar
	else
		DrawNumTiny(x + 36, y + 12, 0, 0xFFFF);	// Unknown health bar ( Just draws question mark )

	if (sub_444EB0(_slot)) {
		const char* name = g_EnemyBattleDatas[v7].name;
		auto enemyIndex = g_EnemyBattleDatas[v7].gap[0x4C];
		auto areaId = *(uint8_t*)0x904EFC;

		auto& txtMgr = TextManager::Get();

		if (!txtMgr.EnemyNamesFileIsLoaded(areaId))
			txtMgr.LoadEnemyNames(areaId);

		if (txtMgr.HasEnemyName(areaId, enemyIndex))
			name = txtMgr.GetEnemyName(areaId, enemyIndex).c_str();

		auto len = GetStringLength(name);

		DrawStringSmall(x + 4, y + 3, 0, len, name);
	}

	sub_444E00(x + 2, v6, x + 73, v6, v10, v9, v8);
	sub_444E00(x + 2, y + 3, x + 2, y + 18, v10, v9, v8);
	sub_444D50(x + 73, v6, x + 73, y + 18, v10, v9, v8);
	sub_444D50(x + 2, y + 18, x + 73, y + 18, v10, v9, v8);
};

Func<0x4439A0, void, uint8_t /* index */> DrawBattleCommandTextPanel;
FuncHook<decltype(DrawBattleCommandTextPanel)> DrawBattleCommandTextPanelHook = [](auto index) {
	auto v1 = sub_5A79A0(0, 0, 960, 0);
	sub_5A77C0(*(int*)0x7E0670, 0, 0, v1, 0);
	sub_461E50(1u, 0xCu);

	auto v2 = ((uint8_t*)0x9039E0)[122] << 6;
	auto top = ((RectS*)0x64E2C8)[index].Top;
	auto left = ((RectS*)0x64E2C8)[index].Left;
	auto v3 = *(uint16_t*)(0x80B780 + v2 + 40);
	auto v7 = 8 * (*(uint8_t*)(0x80B780 + v2 + 40) & 0x1F);
	auto v6 = 8 * ((v3 >> 5) & 0x1F);
	auto v8 = 8 * ((v3 >> 10) & 0x1F);

	constexpr auto BG_WIDTH = 54;
	constexpr auto BG_HEIGHT = 14;

	auto wndColour = *(uint16_t*)(0x80B780 + 64 * ((uint8_t*)0x9039E0)[122] + 8 + 32);
	Colour bgColour = {
		(uint8_t)(8 * (wndColour & 0x1F)),
		(uint8_t)(8 * ((wndColour >> 5) & 0x1F)),
		(uint8_t)(8 * ((wndColour >> 10) & 0x1F))
	};

	DrawPlane(left, top + 2, BG_WIDTH, BG_HEIGHT, bgColour, true);	// Background

	DrawQuad(
		{ left + 2,				top },
		{ left + BG_WIDTH - 2,	top },
		{ left,					top + 2 },
		{ left + BG_WIDTH,		top + 2 },
		bgColour,
		true
	);	// Background top

	DrawQuad(
		{ left,					top + 2 + BG_HEIGHT },
		{ left + BG_WIDTH,		top + 2 + BG_HEIGHT },
		{ left + 2,				top + 2 + BG_HEIGHT + 2 },
		{ left + BG_WIDTH - 2,	top + 2 + BG_HEIGHT + 2 },
		bgColour,
		true
	);	// Background bottom

	SetBlendMode(0, 1, 960, 0);
	DrawPlane(left + 2, top + 2, BG_WIDTH - 4, 1, bgColour, true);
	DrawPlane(left + 2, top + 2, 1, BG_HEIGHT, bgColour, true);

	SetBlendMode(0, 0, 960, 0);
	DrawPlane(left + 2, top + 2 + BG_HEIGHT - 1, BG_WIDTH - 4, 1, bgColour, true);
	DrawPlane(left + BG_WIDTH - 2 - 1, top + 2, 1, BG_HEIGHT, bgColour, true);

	DrawString(left + 8, top + 3, 0, 8u, ((char**)0x669D60)[index]);
};

//Func<0x597230, void> sub_597230;
//FuncHook<decltype(sub_597230)> sub_597230Hook = []() {
//
//};


export void EnableGuiBattleHooks() {
	//EnableHook(DrawBattleEnemyPanel, DrawBattleEnemyPanelHook);
	EnableHook(DrawBattleCommandTextPanel, DrawBattleCommandTextPanelHook);
	//EnableHook(sub_597230, sub_597230Hook);

	WriteProtectedMemory(0x443ECE, (uint8_t)(4 - 2));	// Move enemy name in battle left by 2 pixels
}
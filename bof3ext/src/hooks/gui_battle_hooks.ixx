module;

#include <cmath>
#include <cstdint>
#include <cstring>

export module bof3ext.hooks:gui.battle;

import bof3ext.helpers;
import bof3ext.math;
import bof3ext.configManager;
import bof3ext.glyphManager;
import bof3ext.textManager;
import bof3.battle;
import bof3.render;
import bof3.text;

import std;


struct RectS {
	short Left, Top, Right, Bottom;
};

struct UnkStruct_G {
	uint8_t gap0[2];
	bool isCharacterName;
	uint8_t gap1;
	char* text;
	uint8_t gap2;
	uint8_t gap3;
	uint8_t byteA;
	uint8_t gap;
};


Accessor<0x904B80, uint16_t> word_904B80;
ArrayAccessor<0x93B8E0, UnkStruct_G> stru_93B8E0;
ArrayAccessor<0x656A34, Vec2s> g_EnemyBattlePanelPositions;


Func<0x444900, void, int16_t /* x */, int16_t /* y */, uint8_t /* a3 */, uint8_t /* a4 */>														sub_444900;
Func<0x4449E0, void, int16_t /* x */, int16_t /* y */, uint8_t /* a3 */, uint32_t /* a4 */, uint8_t /* a5 */>									sub_4449E0;
Func<0x4447B0, void, int16_t /* x */, int16_t /* y */, uint8_t /* a3 */, uint8_t /* a4 */>														sub_4447B0;
Func<0x444A90, void, int16_t /* x */, int16_t /* y */, uint8_t /* a3 */, uint8_t /* a4 */, bool /* a5 */>										sub_444A90;
Func<0x444EB0, bool, uint8_t /* a1 */>																											sub_444EB0;
Func<0x444E00, bool, int16_t /* x1 */, int16_t /* y1 */, int16_t /* x2 */, int16_t /* y2 */, uint8_t /* r */, uint8_t /* g */, uint8_t /* b */>	sub_444E00;
Func<0x444D50, bool, int16_t /* x1 */, int16_t /* y1 */, int16_t /* x2 */, int16_t /* y2 */, uint8_t /* r */, uint8_t /* g */, uint8_t /* n */>	sub_444D50;


Func<0x443D90, void, int16_t /* x */, int16_t /* y */, uint8_t /* slot */> DrawBattleEnemyPanel;
auto DrawBattleEnemyPanelHook(auto x, auto y, auto slot) {
	DrawBorderedPanel(x, y, 74, 22);
	
	auto _slot = slot - 3;

	if (g_EnemyBattleDatas[_slot].gap0 == 1)
		sub_444A90(x + 6, y + 10, dword_905B84->byteB, dword_905B84->byteD, true);	// Health bar
	else
		DrawNumTiny(x + 36, y + 12, 0, 0xFFFF);	// Unknown health bar ( Just draws question mark )

	if (sub_444EB0(_slot)) {
		const char* name = g_EnemyBattleDatas[_slot].name;
		auto enemyIndex = g_EnemyBattleDatas[_slot].gap[0x4C];
		auto areaId = *(uint8_t*)0x904EFC;

		auto& txtMgr = TextManager::Get();

		if (!txtMgr.EnemyNamesFileIsLoaded(areaId))
			txtMgr.LoadEnemyNames(areaId);

		if (txtMgr.HasEnemyName(areaId, enemyIndex))
			name = txtMgr.GetEnemyName(areaId, enemyIndex).c_str();

		auto len = GetStringLength(name);

		DrawStringSmall(x + 4, y + 3, 0, len, name);
	}
}

Func<0x4439A0, void, uint8_t /* index */> DrawBattleCommandTextPanel;
auto DrawBattleCommandTextPanelHook(auto index) {
	auto x = ((RectS*)0x64E2C8)[index].Left;
	auto y = ((RectS*)0x64E2C8)[index].Top;

	DrawBorderedPanel(x, y, 54, 18);
	DrawString(x + 8, y + 3, 0, 8u, ((char**)0x669D60)[index]);
}

// Draws the panel at the top of the screen that displays current character or skill name
Func<0x597230, void> DrawBattleActionTextPanel;
auto DrawBattleActionTextPanelHook() {
	auto& stru = stru_93B8E0[dword_905B84->byteA];

	auto textLen = std::strlen(stru.text);
	auto x = dword_905B84->x;
	auto y = dword_905B84->y;

	const auto& cfgMgr = ConfigManager::Get();
	auto renderWidth = cfgMgr.GetScaledRenderWidth();

	auto diff = (float)(renderWidth - 320.f);
	auto offset = diff / 2;

	x += offset;

	auto advance = std::ceil(GlyphManager::Get().GetScaledGlyphAdvance());

	if (stru.isCharacterName) {
		DrawBorderedPanel(x - 16, y, 102, 18);
		auto _x = (x - 16 + 51) - (textLen * advance) / 2;
		DrawString(_x, y + 3, stru.byteA, textLen, stru.text);
	} else {
		DrawBorderedPanel(x, y, 70, 18);
		auto _x = (x + 35) - (textLen * advance) / 2;
		DrawString(_x, y + 3, stru.byteA, textLen, stru.text);
	}
}

Func<0x42F680, void> sub_42F680;
auto sub_42F680Hook() {
	sub_42F680.Original();

	if (*word_904B80 != 151) {
		const auto& skillName = TextManager::Get().GetSkillName(*word_904B80);
		strncpy_s((char*)0x904EA0, 32, skillName.c_str(), skillName.length());
	}
}


export void EnableGuiBattleHooks() {
	//EnableHook(DrawBattleEnemyPanel, DrawBattleEnemyPanelHook);
	EnableHook(DrawBattleCommandTextPanel, DrawBattleCommandTextPanelHook);
	EnableHook(DrawBattleActionTextPanel, DrawBattleActionTextPanelHook);
	EnableHook(sub_42F680, sub_42F680Hook);

	WriteProtectedMemory(0x443ECE, (uint8_t)(4 - 2));	// Move enemy name in battle left by 2 pixels

	auto diff = ConfigManager::Get().GetScaledRenderWidth() - 320;

	g_EnemyBattlePanelPositions[0].x += diff;
	g_EnemyBattlePanelPositions[3].x += diff;
}
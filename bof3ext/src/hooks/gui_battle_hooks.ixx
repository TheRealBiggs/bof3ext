module;

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>

export module bof3ext.hooks:gui.battle;

import bof3ext.helpers;
import bof3ext.configManager;
import bof3ext.glyphManager;
import bof3ext.renderHelpers;
import bof3ext.textManager;
import bof3.battle;
import bof3.gui;
import bof3.math;
import bof3.render;
import bof3.text;

import std;


struct RectS {
	short Left, Top, Right, Bottom;
};


typedef Accessor<0x904B80, uint16_t> word_904B80;
//typedef ArrayAccessor<0x656A34, Vec2us> g_EnemyBattlePanelPositions;
typedef ArrayAccessor<0x802DC0, UnkStruct_D> stru_802DC0;
typedef ArrayAccessor<0x93B8E0, UnkStruct_G> stru_93B8E0;


typedef Func<0x444900, void, int16_t /* x */, int16_t /* y */, uint8_t /* a3 */, uint8_t /* a4 */>														sub_444900;
typedef Func<0x4449E0, void, int16_t /* x */, int16_t /* y */, uint8_t /* a3 */, uint32_t /* a4 */, uint8_t /* a5 */>									sub_4449E0;
typedef Func<0x4447B0, void, int16_t /* x */, int16_t /* y */, uint8_t /* a3 */, uint8_t /* a4 */>														sub_4447B0;
typedef Func<0x444A90, void, int16_t /* x */, int16_t /* y */, uint8_t /* a3 */, uint8_t /* a4 */, bool /* a5 */>										sub_444A90;
typedef Func<0x444EB0, bool, uint8_t /* a1 */>																											sub_444EB0;
typedef Func<0x444E00, bool, int16_t /* x1 */, int16_t /* y1 */, int16_t /* x2 */, int16_t /* y2 */, uint8_t /* r */, uint8_t /* g */, uint8_t /* b */>	sub_444E00;
typedef Func<0x444D50, bool, int16_t /* x1 */, int16_t /* y1 */, int16_t /* x2 */, int16_t /* y2 */, uint8_t /* r */, uint8_t /* g */, uint8_t /* n */>	sub_444D50;


typedef Func<0x443D90, void, int16_t /* x */, int16_t /* y */, uint8_t /* slot */> DrawBattleEnemyPanel;
auto DrawBattleEnemyPanelHook(auto x, auto y, auto slot) {
	DrawBorderedPanel(x, y, 74, 22);

	auto _slot = slot - 3;

	if (g_EnemyBattleDatas::At(_slot).gap0 == 1)
		sub_444A90::Call(x + 6, y + 10, struct_905B84::Get()->byteB, struct_905B84::Get()->byteD, true);	// Health bar
	else
		DrawNumTiny::Call(x + 36, y + 12, 0, 0xFFFF);	// Unknown health bar ( Just draws question mark )

	if (sub_444EB0::Call(_slot)) {
		const char* name = g_EnemyBattleDatas::At(_slot).name;
		auto enemyIndex = g_EnemyBattleDatas::At(_slot).enemyId;
		auto areaId = *(uint8_t*)0x904EFC;

		auto& txtMgr = TextManager::Get();

		if (!txtMgr.EnemyNamesFileIsLoaded(areaId))
			txtMgr.LoadEnemyNames(areaId);

		if (txtMgr.HasEnemyName(areaId, enemyIndex))
			name = txtMgr.GetEnemyName(areaId, enemyIndex).c_str();

		auto len = GetStringLength::Call(name);

		DrawStringSmall::Call(x + 4, y + 3, 0, len, name);
	}
}

typedef Func<0x4439A0, void, uint8_t /* index */> DrawBattleCommandTextPanel;
auto DrawBattleCommandTextPanelHook(auto index) {
	auto x = ((RectS*)0x64E2C8)[index].Left;
	auto y = ((RectS*)0x64E2C8)[index].Top;

	if (index == 0 || index == 3 || index == 5)
		x -= 14;
	else if (index == 1 || index == 2)
		x -= 7;

	DrawBorderedPanel(x, y, 54, 18);

	auto text = ((char**)0x669D60)[index];
	auto textLen = GetStringLength::Call(text);

	auto advance = GlyphManager::Get().GetScaledGlyphAdvance();
	advance /= 2;

	DrawString::Call(static_cast<int16_t>(x + 27 - advance * textLen), y + 3, 0, textLen, text);
}

// Draws the panel at the top of the screen that displays current character or skill name
typedef Func<0x597230, void> DrawBattleActionTextPanel;
auto DrawBattleActionTextPanelHook() {
	auto& stru = stru_93B8E0::At(struct_905B84::Get()->index);

	auto textLen = std::strlen(stru.text);
	auto x = struct_905B84::Get()->x;
	auto y = struct_905B84::Get()->y;

	const auto& cfgMgr = ConfigManager::Get();
	auto renderWidth = cfgMgr.GetScaledRenderWidth();

	auto diff = (float)(renderWidth - 320.f);
	auto offset = diff / 2;

	x += static_cast<uint16_t>(offset);

	auto advance = std::ceil(GlyphManager::Get().GetScaledGlyphAdvance());

	if (stru.isCharacterName) {
		DrawBorderedPanel(x - 16, y, 102, 18);
		auto _x = static_cast<int16_t>((x - 16 + 51) - (textLen * advance) / 2);
		DrawString::Call(_x, y + 3, stru.byteA, static_cast<uint8_t>(textLen), stru.text);
	} else {
		DrawBorderedPanel(x, y, 70, 18);
		auto _x = static_cast<int16_t>((x + 35) - (textLen * advance) / 2);
		DrawString::Call(_x, y + 3, stru.byteA, static_cast<uint8_t>(textLen), stru.text);
	}
}

typedef Func<0x59E160, void, int16_t /* x */, int16_t /* y */, int8_t /* a3 */> DrawBattleInventoryTabs;
auto DrawBattleInventoryTabsHook(auto x, auto y, auto a3) {
	const auto& txtMgr = TextManager::Get();

	DrawWindowBackground::Call(x, y, 45u, 20u, 0, ((uint8_t*)0x9039E0)[122]);

	uint8_t txtColour;

	if (a3 == -1 || !a3)
		txtColour = 0;
	else
		txtColour = 7;

	auto advance = GlyphManager::Get().GetScaledGlyphAdvance();
	advance /= 2;

	const auto& useText = txtMgr.GetMenuTabText(0);
	DrawString::Call(static_cast<int16_t>(x + 22 - advance * useText.length()), y + 3, txtColour, 16u, useText.c_str());

	auto v4 = (char*)0x66B500;

	if (a3)
		v4 = (char*)0x66B4F8;

	DrawUIGroup::Call(x, y, v4, 1);
	DrawWindowBackground::Call(x + 48, y, 45u, 20u, 0, ((uint8_t*)0x9039E0)[122]);

	if (a3 == -1 || a3 == 1)
		txtColour = 0;
	else
		txtColour = 7;

	const auto& equipText = txtMgr.GetMenuTabText(3);
	DrawString::Call(static_cast<int16_t>(x + 70 - advance * equipText.length()), y + 3, txtColour, 16u, equipText.c_str());

	v4 = (char*)0x66B500;

	if (a3 != 1)
		v4 = (char*)0x66B4F8;

	DrawUIGroup::Call(x + 48, y, v4, 1);
}

typedef Func<0x5985A0, void> DrawBattleXPResultPanel;
auto DrawBattleXPResultPanelHook() {
	const auto& txtMgr = TextManager::Get();

	DrawBorderedPanel(20, 40, 280, struct_905B84::Get()->byte9);

	auto y = 44;

	for (auto i = 0; i < *(uint8_t*)0x904AB0; ++i) {
		const auto& c = stru_802DC0::At(i);

		// Character name
		DrawString::Call(25, y, 0, 5, c.name);

		uint16_t textId;

		if (c.level == 99)
			textId = 65;	// " has reached MAX LVL!"
		else {
			char buf[7];

			sprintf_s(buf, "%2d", c.level + 1);
			DrawStringLarge::Call(85, y, 0, buf);

			auto advance = GlyphManager::Get().GetScaledGlyphAdvance();

			auto x = static_cast<int16_t>(20 + 280 - 7 - std::ceil(6 * advance * LARGE_TEXT_SCALE));

			sprintf_s(buf, "%6d", GetXPToNextLevel::Call(i));
			DrawStringLarge::Call(x, y, 0, buf);

			textId = 21;	// "EXP to next level:"
		}

		const auto text = GetText::Call(textId);
		DrawString::Call(133, y, 0, 255, text);

		y += 16;
	}
}

typedef Func<0x42F680, void> sub_42F680;
auto sub_42F680Hook() {
	sub_42F680::Original();

	if (word_904B80::Get() != 151) {
		const auto& skillName = TextManager::Get().GetSkillName(word_904B80::Get());
		strncpy_s((char*)0x904EA0, 32, skillName.c_str(), skillName.length());
	}
}

typedef Func<0x44A960, void, uint8_t /* index */> sub_44A960;
auto sub_44A960Hook(auto index) {
	auto enemyId = g_EnemyBattleDatas::At(index - 3).enemyId;
	auto areaId = *(uint8_t*)0x904EFC;

	const auto& name = TextManager::Get().GetEnemyName(areaId, enemyId);

	std::memcpy((void*)0x904CE0, name.c_str(), std::min(32U, name.length()));
}


static const float HALF = 0.5f;


static void __declspec(naked) FixTextCenteringInventoryCategory() {
	__asm {
		push edx;				// Save EDX register (textLen)
		call GlyphManager::Get;
		mov ecx, eax;			// Move GlyphManager instance into ECX for __thiscall
		call GlyphManager::GetScaledGlyphAdvance;
		fmul[HALF];				// Divide ST0 (advance) by 2
		fimul[esp];				// Multiply ST0 by textLen
		fistp[esp];				// Move and truncate ST0 into space on stack (reserved by previous `push edx`)
		mov eax, 77;			// 77 is X offset of center of textbox
		pop edx;				// Pop converted float (half textLen * advance) into EDX
		sub eax, edx;			// Subtract converted float from 77 to get final X offset
		ret;
	}
}

static void __declspec(naked) FixTextCenteringSkillCategory() {
	__asm {
		push ecx;				// Save ECX register (textLen)
		call GlyphManager::Get;
		mov ecx, eax;			// Move GlyphManager instance into ECX for __thiscall
		call GlyphManager::GetScaledGlyphAdvance;
		fmul[HALF];				// Divide ST0 (advance) by 2
		fimul[esp];				// Multiply ST0 by textLen
		fistp[esp];				// Move and truncate ST0 into space on stack (reserved by previous `push ecx`)
		mov ecx, 77;			// 77 is X offset of center of textbox
		pop eax;				// Pop converted float (half textLen * advance) into EAX
		sub ecx, eax;			// Subtract converted float from 77 to get final X offset
		ret;
	}
}

static void FixGetBattleLearnedSkillName() {
	auto a = *(uint8_t*)0x904B34 - 3;
	auto skillId = g_EnemyBattleDatas::At(a).word86;

	const auto& skillName = TextManager::Get().GetSkillName(skillId);
	std::memcpy((void*)0x904D00, skillName.c_str(), std::min(32U, skillName.length()));
}


export void EnableGuiBattleHooks() {
	//EnableHook(DrawBattleEnemyPanel, DrawBattleEnemyPanelHook);
	EnableHook<DrawBattleCommandTextPanel>(DrawBattleCommandTextPanelHook);
	EnableHook<DrawBattleActionTextPanel>(DrawBattleActionTextPanelHook);
	EnableHook<DrawBattleInventoryTabs>(DrawBattleInventoryTabsHook);
	EnableHook<DrawBattleXPResultPanel>(DrawBattleXPResultPanelHook);
	EnableHook<sub_42F680>(sub_42F680Hook);
	EnableHook<sub_44A960>(sub_44A960Hook);

	WriteProtectedMemory(0x443ECE, (uint8_t)(4 - 2));	// Move enemy name in battle left by 2 pixels

	auto diff = ConfigManager::Get().GetScaledRenderWidth() - 320;

	g_EnemyBattlePanelPositions::At(0).x += diff;
	g_EnemyBattlePanelPositions::At(3).x += diff;

	WriteCallAndNops<7>(0x59CF6E, FixTextCenteringInventoryCategory);
	WriteCallAndNops<7>(0x59D43F, FixTextCenteringSkillCategory);

	WriteCallAndNops<73>(0x42FED7, FixGetBattleLearnedSkillName);
}
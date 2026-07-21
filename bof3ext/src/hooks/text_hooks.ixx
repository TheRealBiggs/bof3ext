module;

#include <cmath>
#include <cstdint>
#include <cstdlib>

export module bof3ext.hooks:text;

import bof3ext.helpers;
import bof3ext.configManager;
import bof3ext.glyphManager;
import bof3ext.textManager;

import bof3.dat;
import bof3.text;

import std;


typedef Func<0x497770, void
> sub_497770;


auto GetTextHook(auto index) {
	//LogDebug("GetText: %i, %i\n", index, LastLoadedDatFileId);

	auto fileNum = index >> 14;
	index = index & 0x3FFF;

	auto& txtMgr = TextManager::Get();

	if (!txtMgr.TextFileIsLoaded(LastLoadedDatFileId, fileNum))
		txtMgr.LoadTextFile(LastLoadedDatFileId, fileNum);

	if (txtMgr.HasText(LastLoadedDatFileId, fileNum, index))
		return txtMgr.GetText(LastLoadedDatFileId, fileNum, index).c_str();

	auto buf = new char[32] {0};
	_itoa_s(index, buf, 32, 10);

	return (const char*)buf;
}

typedef Func<0x4976D0, void,
	uint16_t	// index
> LoadDialogue;
auto LoadDialogueHook(auto index) {
	//LogDebug("LoadDialogue: %i\n", index);

	auto areaId = *(uint8_t*)0x904EFC;

	auto& txtMgr = TextManager::Get();

	if (!txtMgr.DialogueFileIsLoaded(areaId))
		txtMgr.LoadDialogue(areaId);

	if (txtMgr.HasDialogue(areaId, index)) {
		auto& text = txtMgr.GetDialogue(areaId, index);

		*(const char**)0x7DEE4C = text.c_str();
		*(const char**)0x7DEE50 = *(const char**)0x7DEE4C;
		sub_497770::Call();
		*(uint16_t*)0x7DEE48 = index;
	} else
		LoadDialogue::Original(index);
}


const char charNameRyu[] = "Ryu";
const char charNameNina[] = "Nina";
const char charNameGarr[] = "Garr";
const char charNameTeepo[] = "Teepo";
const char charNameRei[] = "Rei";
const char charNameMomo[] = "Momo";
const char* defaultCharNames[] = { charNameRyu, charNameNina, charNameGarr, charNameTeepo, charNameRei, charNameMomo, "" };

const char master[] = "Master";

const char* battleCommandText[] = { "Attack", "Ability", "Item", "Examine", "Defend", "Charge", "Escape" };
const char* battleCommandText2[] = { "Attack", " \x81\x01", "Examine", "DDD", "EEE", "FFF", "Reprisal", "Critical", "III", "JJJ", "Escape", "LLL" };
const char* configActionText[] = { "Speak", "Move", "Action", "Menu", "View", "Change" };

const char* skillNoteSortText[] = { "Sort", "Look", "High AP", "Low AP" };


export void EnableTextHooks() {
	EnableHook<GetText>(GetTextHook);
	EnableHook<LoadDialogue>(LoadDialogueHook);

	auto advance = (int)std::floor(GlyphManager::Get().GetScaledGlyphAdvance());

	WriteProtectedMemory(0x516CE5, (uint8_t)advance);						// Glyph spacing for UI text
	WriteProtectedMemory(0x497A47, (uint8_t)advance);						// Glyph spacing for dialogue
	//WriteProtectedMemory(0x516F35, (uint8_t)(advance * (2.0 / 3.0))));	// Glyph spacing for small text
	WriteProtectedMemory(0x5962E7, (uint8_t)advance);						// Glyph spacing for dialogue choices
	WriteProtectedMemory(0x4979B7, (int16_t)-advance);						// X offset for '"' and '<' in dialogue
	WriteProtectedMemory(0x4978DD, (uint8_t)12);							// Line height in dialogue

	*(uint16_t*)0x66A31C = EncodeUnicodeCharacter(u'ƶ');

	WriteProtectedMemory(0x516C99, (uint8_t)0xEB);	// Change JBE to JMP to skip checking of unicode characters above 0x0A00
	WriteProtectedMemory(0x516C90, (uint8_t)0);		// Allow all ASCII characters
	//WriteProtectedMemory(0x516F05, (uint8_t)'!');	// Allow characters '!', '"', '#', '$', and '%' in small text
	WriteProtectedMemory(0x4979A3, (uint8_t)'"');	// Move double-quotes back a space in dialogue

	WriteProtectedMemory(0x443EC9, (uint8_t)14);	// Increase max enemy name length in battle

	WriteProtectedMemory(0x669CCC, (uint8_t)'\0');	// Remove text in empty save slots

	WriteProtectedMemory(0x669FA4, defaultCharNames);

	WriteProtectedMemory((0x64B390 + 164 * 0), charNameRyu);
	WriteProtectedMemory((0x64B390 + 164 * 1), charNameNina);
	WriteProtectedMemory((0x64B390 + 164 * 2), charNameGarr);
	WriteProtectedMemory((0x64B390 + 164 * 3), charNameTeepo);
	WriteProtectedMemory((0x64B390 + 164 * 4), charNameRei);
	WriteProtectedMemory((0x64B390 + 164 * 5), charNameMomo);

	WriteProtectedMemory(0x669D60, battleCommandText);
	WriteProtectedMemory(0x669DE0, battleCommandText2);

	// Item count format string fix (*%2d -> x%2d)
	WriteProtectedMemory(0x653EC0, 'x');

	const auto& txtMgr = TextManager::Get();

	// Item categories
	for (int i = 0; i < 5; ++i) {
		if (!txtMgr.HasCategoryText(i))
			break;

		const auto& text = txtMgr.GetCategoryText(i);

		WriteProtectedMemory(0x663970 + i * sizeof(char*), text.c_str());	// Inventory window
		WriteProtectedMemory(0x663994 + i * sizeof(char*), text.c_str());	// Equipment window
		WriteProtectedMemory(0x66B58C + i * sizeof(char*), text.c_str());	// In-battle inventory window
	}

	// Skill categories
	for (int i = 0; i < 4; ++i) {
		if (!txtMgr.HasCategoryText(5 + i))
			break;

		const auto& text = txtMgr.GetCategoryText(5 + i);

		WriteProtectedMemory(0x66B5A0 + i * sizeof(char*), text.c_str());
		WriteProtectedMemory(0x663984 + i * sizeof(char*), text.c_str());
	}

	// Master window category text
	WriteProtectedMemory(0x66A1F0, master);

	// Skill note window text
	WriteProtectedMemory(0x66B36C, skillNoteSortText);

	// Config controller action names
	WriteProtectedMemory(0x66A368, configActionText);

	// Button glyphs
	WriteProtectedMemory(0x66A2E4, EncodeUnicodeCharacter(u'△'));
	WriteProtectedMemory(0x66A2DC, EncodeUnicodeCharacter(u'◯'));
	WriteProtectedMemory(0x66A2E0, EncodeUnicodeCharacter(u'☓'));
	WriteProtectedMemory(0x66A2E8, EncodeUnicodeCharacter(u'⬜'));
	WriteProtectedMemory(0x66A2EC, EncodeUnicodeCharacter(u'⮪'));	// L1
	WriteProtectedMemory(0x66A2F0, EncodeUnicodeCharacter(u'⮫'));	// R1

	const char inkText[] = "Ink";
	WriteProtectedMemory(0x66A118, inkText);
}
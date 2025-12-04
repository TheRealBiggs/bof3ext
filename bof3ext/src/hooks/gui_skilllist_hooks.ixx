module;

#include <cstdint>

export module bof3ext.hooks:gui.skilllist;

import bof3ext.helpers;
import bof3ext.textManager;
import bof3.skill;


Func<0x57DC90, void, int16_t /* x */, int16_t /* y */, int /* paletteIdx */, uint8_t /* iconId */, const char* /* text */, uint8_t /* cost */, bool /* greyed */> DrawSkillInfo;
FuncHook<decltype(DrawSkillInfo)> DrawSkillInfoHook = [](auto x, auto y, auto paletteIdx, auto iconId, auto text, auto cost, auto greyed) {
	if ((uintptr_t)text >= 0x65C4C8 && (uintptr_t)text <= 0x65DA10) {	// Skill name
		auto id = ((uintptr_t)text - 0x65C4C8) / sizeof(SkillData);

		auto& txtMgr = TextManager::Get();

		if (txtMgr.HasSkillName(id))
			text = txtMgr.GetSkillName(id).c_str();
	}

	DrawSkillInfo.Original(x, y, paletteIdx, iconId, text, cost, greyed);
};


export void EnableGuiSkillListHooks() {
	EnableHook(DrawSkillInfo, DrawSkillInfoHook);
}
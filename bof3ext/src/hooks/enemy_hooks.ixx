module;

#include <cstdint>

export module bof3ext.hooks:enemy;

import bof3ext.helpers;
import bof3ext.textManager;
import bof3.enemy;

import std;


Func<0x4946C0, void, uint8_t /* slot */, uint8_t /* index */> LoadEnemyNameEtc;
auto LoadEnemyNameEtcHook(auto slot, auto index) {
	LoadEnemyNameEtc.Original(slot, index);

	auto battleSlot = (char*)0x93B9E0 + 296 * slot;	// Array of structs of size 296
	auto areaId = *(uint8_t*)0x904EFC;

	auto& txtMgr = TextManager::Get();

	if (!txtMgr.EnemyNamesFileIsLoaded(areaId))
		txtMgr.LoadEnemyNames(areaId);

	if (txtMgr.HasEnemyName(areaId, index)) {
		const auto& name = txtMgr.GetEnemyName(areaId, index);

		std::memset(battleSlot, 0, std::min(name.length() + 1, 12U));
		*(uint32_t*)battleSlot = 0x600DBEEF;
		*(const char**)&battleSlot[4] = name.c_str();
	}
}


export void EnableEnemyHooks() {
	EnableHook(LoadEnemyNameEtc, LoadEnemyNameEtcHook);
}
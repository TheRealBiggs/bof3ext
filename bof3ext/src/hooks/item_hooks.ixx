module;

#include <cstdint>

export module bof3ext.hooks:item;

import bof3ext.helpers;
import bof3ext.textManager;
import bof3.item;

import std;


auto GetItemNameHook(auto type, auto index) {
	auto& txtMgr = TextManager::Get();

	switch (type) {
		case 0:
			if (txtMgr.HasItemName(index))
				return txtMgr.GetItemName(index).c_str();

			break;

		case 1:
			if (txtMgr.HasWeaponName(index))
				return txtMgr.GetWeaponName(index).c_str();

			break;

		case 2:
			if (txtMgr.HasArmorName(index))
				return txtMgr.GetArmorName(index).c_str();

			break;

		case 3:
			if (txtMgr.HasAccessoryName(index))
				return txtMgr.GetAccessoryName(index).c_str();

			break;
	}

	// TODO: Add key item names

	return GetItemName.Original(type, index);
}

Func<0x4B58F0, void, uint8_t /* id */, uint8_t /* type */> LoadItemName;
auto LoadItemNameHook(auto index, auto type) {
	auto& txtMgr = TextManager::Get();
	auto dst = (char*)0x904CE0;

	switch (type) {
		case 0:
			if (txtMgr.HasItemName(index)) {
				const auto& name = txtMgr.GetItemName(index);
				name.copy(dst, name.length(), 0);
				dst[name.length()] = '\0';
			}

			break;

		case 1:
			if (txtMgr.HasWeaponName(index)) {
				const auto& name = txtMgr.GetWeaponName(index);
				name.copy(dst, name.length(), 0);
				dst[name.length()] = '\0';
			}

			break;

		case 2:
			if (txtMgr.HasArmorName(index)) {
				const auto& name = txtMgr.GetArmorName(index);
				name.copy(dst, name.length(), 0);
				dst[name.length()] = '\0';
			}

			break;

		case 3:
			if (txtMgr.HasAccessoryName(index)) {
				const auto& name = txtMgr.GetAccessoryName(index);
				name.copy(dst, name.length(), 0);
				dst[name.length()] = '\0';
			}

			break;
	}
}


export void EnableItemHooks() {
	EnableHook(GetItemName, GetItemNameHook);
	EnableHook(LoadItemName, LoadItemNameHook);
}
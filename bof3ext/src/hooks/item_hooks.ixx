module;

export module bof3ext.hooks:item;

import bof3ext.helpers;
import bof3ext.textManager;
import bof3.item;

import std;


FuncHook<decltype(GetItemName)> GetItemNameHook = [](auto type, auto index) {
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

	return GetItemName.Original(type, index);
};


export void EnableItemHooks() {
	EnableHook(GetItemName, GetItemNameHook);
}
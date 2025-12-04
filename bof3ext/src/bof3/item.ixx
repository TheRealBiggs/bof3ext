module;

#include <cstdint>

export module bof3.item;

import bof3ext.helpers;


struct ItemBase {
	char name[16];
};

struct EquipmentBase : public ItemBase {
	uint8_t equipFlags;
	uint8_t flags;
	uint8_t iconId;
};

struct ItemData : public ItemBase {
	uint8_t flags;
	uint8_t iconId;
	uint16_t descriptionId;
	uint16_t cost;
};

struct WeaponData : public EquipmentBase {
	uint8_t element;
	uint8_t weight;
	uint8_t gap2;
	uint8_t power;
	uint16_t descriptionId;
	uint16_t cost;
};

struct ArmorData : public EquipmentBase {
	uint8_t weight;
	uint8_t defense;
	uint16_t descriptionId;
	uint16_t cost;
};

struct AccessoryData : public EquipmentBase {
	uint8_t weight;
	uint16_t descriptionId;
	uint16_t price;
};


export Func<0x591720, int, uint8_t /* type */, uint8_t /* index */>			GetItemIconId;
export Func<0x591A80, uint8_t, uint8_t /* category */>						GetUniqueItemCount;
export Func<0x591680, const char*, uint8_t /* type */, uint8_t /* index */>	GetItemName;
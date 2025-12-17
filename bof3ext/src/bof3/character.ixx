module;

#include <cstdint>

export module bof3.character;

import bof3ext.helpers;


export struct Character {
	char name[8];
	uint8_t gap0;
	uint8_t id;
	uint8_t level;
	uint8_t gap1[7];
	uint8_t weaponId;
	uint8_t armorId1;
	uint8_t armorId2;
	uint8_t armorId3;
	uint8_t accessoryId1;
	uint8_t accessoryId2;
	uint16_t curHP;
	uint8_t gap22[5];
	uint8_t byte1F;
	uint8_t gap2[4];
	uint16_t power;
	uint16_t defence;
	uint16_t agility;
	uint16_t intelligence;
	uint8_t gap[120];
};


export ArrayAccessor<0x903A70, Character> g_Characters;
module;

#include <cstdint>

export module bof3.battle;

import bof3ext.helpers;


export struct EnemyBattleData {
	char name[12];
	uint8_t byteC;
	uint8_t byteD;
	uint8_t byteE;
	uint8_t gap0;
	uint16_t word10;
	uint8_t gap1[2];
	uint16_t word14;
	uint16_t word16;
	uint16_t word18;
	uint32_t dword1C;
	uint32_t dword20;
	uint8_t gap[260];
};


export ArrayAccessor<0x93B9E0, EnemyBattleData> g_EnemyBattleDatas;
module;

#include <cstdint>

export module bof3.battle;

import bof3ext.helpers;


export struct EnemyBattleData {
	char name[12];
	uint8_t byteC;
	uint8_t byteD;
	uint8_t byteE;
	uint8_t byteF;
	uint16_t word10;
	uint16_t word12;
	uint16_t word14;
	uint16_t word16;
	uint16_t word18;
	uint32_t dword1C;
	uint32_t dword20;
	uint16_t hp;
	uint8_t gap2[9];
	uint8_t gap22;
	uint16_t word30;
	uint8_t gap3[62];
	uint8_t enemyId;
	uint8_t gap33[20];
	uint8_t byte85;
	uint16_t word86;
	uint8_t gap4[3];
	uint8_t gap44;
	uint8_t byte8C;
	uint8_t gap444[2];
	uint8_t gap4444;
	uint32_t dword90;
	uint8_t gap5[20];
	uint8_t byteA8;
	uint8_t byteA9;
	uint8_t gap[126];
};


export typedef ArrayAccessor<0x93B9E0, EnemyBattleData> g_EnemyBattleDatas;


export typedef Func<0x598810, uint32_t, uint8_t /* charIdx */> GetXPToNextLevel;
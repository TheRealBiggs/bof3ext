module;

#include <cstdint>

export module bof3.gui;

import bof3ext.helpers;
import bof3.math;


export struct UnkStruct_D {
	char name[8];
	uint8_t gap0[1];
	uint8_t byte9;
	uint8_t level;
	uint8_t gap[321];
};

export struct UnkStruct_G {
	uint8_t gap0[2];
	bool isCharacterName;
	uint8_t gap1;
	char* text;
	uint8_t gap2;
	uint8_t gap3;
	uint8_t byteA;
	uint8_t gap;
};


export typedef Accessor<0x904B80, uint16_t> word_904B80;
export typedef ArrayAccessor<0x656A34, Vec2us> g_EnemyBattlePanelPositions;
export typedef ArrayAccessor<0x66383C, uint8_t> byte_66383C;
export typedef ArrayAccessor<0x66386C, uint8_t> g_EquipWindowItemIconIds;
export typedef ArrayAccessor<0x802DC0, UnkStruct_D> stru_802DC0;
export typedef ArrayAccessor<0x93B8E0, UnkStruct_G> stru_93B8E0;


export typedef Func<0x57CF60, void, 
	int16_t,	// x
	int16_t,	// y
	uint16_t,	// w
	uint16_t,	// h
	uint8_t,	// flags
	uint8_t		// paletteIdx
> DrawWindowBackground;

export typedef Func<0x57D910, void,
	int16_t,	// x
	int16_t,	// y
	char*,		// a3
	uint8_t		// a4
> DrawUIGroup;
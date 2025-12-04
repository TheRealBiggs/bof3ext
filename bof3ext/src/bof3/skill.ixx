module;

#include <cstdint>

export module bof3.skill;


export struct SkillData {
	char name[16];
	uint8_t targetFlags;
	uint8_t category : 4;
	uint8_t examineChance : 4;
	uint8_t cost;
	uint8_t power;
	uint8_t element;
	uint16_t descriptionId;
};
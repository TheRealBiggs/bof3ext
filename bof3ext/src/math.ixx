module;

#include <cstdint>

export module bof3ext.math;


export struct Vec2f {
	float x, y;
};

export struct Vec2i {
	int32_t x, y;
};

export struct Vec2s {
	uint16_t x, y;
};

export struct Vec2b {
	uint8_t x, y;
};

export struct Vec3f {
	float x, y, z;
};
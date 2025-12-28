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

export union Vec3f {
	struct {
		float x, y, z;
	};

	struct {
		float r, g, b;
	};
};

export union Vec3b {
	struct {
		uint8_t x, y, z;
	};

	struct {
		uint8_t r, g, b;
	};
};

export union Vec4f {
	struct {
		float x, y, z, w;
	};

	struct {
		float r, g, b, a;
	};
};

export union Vec4b {
	struct {
		uint8_t x, y, z, w;
	};

	struct {
		uint8_t r, g, b, a;
	};
};

export struct Rectf {
	float x, y, w, h;
};

export struct Mat4f {
	union {
		float m[4][4];
		struct {
			float
				_m00, _m01, _m02, _m03,
				_m10, _m11, _m12, _m13,
				_m20, _m21, _m22, _m23,
				_m30, _m31, _m32, _m33;
		};
	};
};

export struct PSX_RECT {
	int16_t x;
	int16_t y;
	int16_t w;
	int16_t h;
};

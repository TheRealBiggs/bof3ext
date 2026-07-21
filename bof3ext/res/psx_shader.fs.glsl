#version 450 core
#extension GL_NV_gpu_shader5 : enable

layout(std430, binding = 1) buffer TexturePages {
	uint8_t tpagebuffer[];
};

out vec4 FragColor;

in vec2 TexCoord;
in vec4 VertColor;

uniform uint tpage;
uniform uint palette;

void main() {
	uint tx = (tpage & 0xF) * 64;
	uint ty = ((tpage >> 4) & 0x1) * 256;

	ty += uint(floor(TexCoord.y));

	uint f = (tpage >> 7) & 3;
	uint t = ty * 2048 + tx * 2;

	uint cx = (palette & 0x3F) * 16;
	uint cy = palette >> 6;

	if (f == 0 || f == 1) {
		if (f == 0)
			t += uint(floor(TexCoord.x / 2.0));
		else
			t += uint(floor(TexCoord.x));

		uint tp = tpagebuffer[t];
						
		if (f == 0) {
			uint tr = uint(floor(TexCoord.x)) % 2;
			tp = (tp >> (tr * 4)) & 0xF;
		}

		uint pt = cy * 2048 + (cx + tp) * 2;
		uint pp = tpagebuffer[pt];
		pp |= uint(tpagebuffer[pt + 1]) << 8;
		uint r = pp & 0x1F;
		uint g = (pp >> 5) & 0x1F;
		uint b = (pp >> 10) & 0x1F;
		uint a = (pp >> 15) & 0x1;

		if (!(r == 0 && g == 0 && b == 0))
			a = 1;

		FragColor = vec4(r / 31.0, g / 31.0, b / 31.0, a) * vec4(VertColor.rgb * 2, VertColor.a);
	} else {
		FragColor = VertColor;
	}
}
#version 450 core

layout(std430, binding = 2) buffer TexturePages {
	uint tpagebuffer[];
};

out vec4 FragColor;

noperspective in vec2 TexCoord;
in vec4 VertColor;
flat in int TexPage;
flat in int Palette;

uint readByte(uint index) {
	uint word = tpagebuffer[index >> 2];
	uint shift = (index & 3u) * 8u;
	return (word >> shift) & 0xFFu;
}

void main() {
	uint tx = (TexPage & 0xF) * 64;
	uint ty = ((TexPage >> 4) & 0x1) * 256;

	uint x = uint(floor(TexCoord.x));
	uint y = uint(floor(TexCoord.y));

	ty += y;

	uint f = (TexPage >> 7) & 3;
	uint t = ty * 2048 + tx * 2;

	uint cx = (Palette & 0x3F) * 16;
	uint cy = Palette >> 6;

	if (TexPage != -1 && (f == 0 || f == 1)) {
		if (f == 0)
			t += uint(floor(x / 2.0));
		else
			t += x;

		uint tp = readByte(t);
						
		if (f == 0) {
			uint tr = x % 2;
			tp = (tp >> (tr * 4)) & 0xF;
		}

		uint pt = cy * 2048 + (cx + tp) * 2;
		uint pp = readByte(pt);
		pp |= uint(readByte(pt + 1)) << 8;
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
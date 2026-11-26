#version 430 core

layout(std430, binding = 2) buffer TexturePages {
	uint tpagebuffer[];
};

out vec4 FragColor;

noperspective in vec2 TexCoord;
in vec4 VertColor;
flat in int TexPage;
flat in int Palette;

uniform int Pass;
uniform int BlendMode;
uniform ivec4 TextureWindow;

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

	x = (x & ~(uint(TextureWindow.z) * 8u)) | ((uint(TextureWindow.x) & uint(TextureWindow.z)) * 8u);
	y = (y & ~(uint(TextureWindow.w) * 8u)) | ((uint(TextureWindow.y) & uint(TextureWindow.w)) * 8u);

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

		if (tp == 0)
			discard;
						
		if (f == 0) {
			uint tr = x % 2;
			tp = (tp >> (tr * 4)) & 0xF;
		}

		uint pt = cy * 2048 + (cx + tp) * 2;
		uint pp = readByte(pt);
		pp |= uint(readByte(pt + 1)) << 8;

		if (pp == 0)
			discard;

		uint r = pp & 0x1F;
		uint g = (pp >> 5) & 0x1F;
		uint b = (pp >> 10) & 0x1F;
		uint blend = (pp >> 15) & 0x1;

		if (Pass == 1 && blend != 0)
			discard;

		if (Pass == 2 && blend == 0)
			discard;

		FragColor = vec4(r / 31.0, g / 31.0, b / 31.0, 1.0) * vec4(VertColor.rgb * 2, VertColor.a);
	} else {
		if (Pass == 1)
			discard;

		FragColor = VertColor;
	}
}
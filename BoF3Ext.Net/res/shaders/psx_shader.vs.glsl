#version 430 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 uv;
layout (location = 3) in int tpage;
layout (location = 4) in int palette;

layout(std140, binding = 0) uniform ProjectionMatrix {
	mat4 proj;
};

layout(std140, binding = 1) uniform ScaleAndOffset {
	float scaleX;
	float scaleY;
	float offsetX;
};

out vec4 VertColor;
noperspective out vec2 TexCoord;
flat out int TexPage;
flat out int Palette;

void main() {
	gl_Position = proj * vec4((pos.x + offsetX) * scaleX, pos.y * scaleY, pos.z, 1.0);
	gl_Position.z = 0.0;
	VertColor = color;
	TexCoord = uv;
	TexPage = tpage;
	Palette = palette;
}
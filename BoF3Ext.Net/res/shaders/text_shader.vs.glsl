#version 430 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 uv;

layout(std140, binding = 0) uniform ProjectionMatrix {
	mat4 proj;
};

layout(std140, binding = 1) uniform Scale {
	float scaleX;
	float scaleY;
};

out vec4 VertColor;
out vec2 TexCoord;

void main() {
	gl_Position = proj * vec4(pos.x * scaleX, pos.y * scaleY, pos.z, 1.0);
	VertColor = color;
	TexCoord = vec2(uv.x, uv.y);
}
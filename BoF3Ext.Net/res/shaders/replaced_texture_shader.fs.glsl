#version 430 core

out vec4 FragColor;

in vec2 TexCoord;
in vec4 VertColor;

layout(binding = 0) uniform sampler2D tex;

void main() {
	vec4 pixel = texture(
		tex,
		TexCoord
	);

	// Alpha is not pre-multiplied
	pixel *= vec4(VertColor.rgb * 2, VertColor.a);
	pixel.rgb *= pixel.a;

	FragColor = pixel;
}
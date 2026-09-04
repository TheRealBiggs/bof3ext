#version 430 core

out vec4 FragColor;

in vec2 TexCoord;
in vec4 VertColor;

uniform sampler2D tex;

void main() {
	vec4 pixel = texture(
		tex,
		vec2(TexCoord.x / 256.0, TexCoord.y / 256.0)
	);

	// Alpha is not pre-multiplied
	pixel.rgb *= pixel.a;

	FragColor = pixel * vec4(VertColor.rgb * 2, VertColor.a);
}
#version 430 core

out vec4 FragColor;

in vec2 TexCoord;
in vec4 VertColor;

layout(binding = 0) uniform sampler2D tex;

const float SPREAD = 4.0;
const float OUTLINE_THICKNESS = 2.5;
const vec3 OUTLINE_COLOUR = vec3(0.1);

void main() {
	float dist = texture(tex, TexCoord).r;

	vec2 pos = TexCoord * vec2(textureSize(tex, 0));
	float dw = max(length(dFdx(pos)), length(dFdy(pos)));

	float spr = (2.0 * SPREAD) / max(dw, 0.00001);
	dist = (dist - 0.5) * spr;

	float aa = min(0.5, spr * 0.25);
	float odist = min(OUTLINE_THICKNESS, spr * 0.5 - aa);

	float fill = smoothstep(-aa, aa, dist);
	float outline = smoothstep(-odist - aa, -odist + aa, dist);

	vec3 color = VertColor.rgb * fill + OUTLINE_COLOUR * max(outline - fill, 0.0);

	float alpha = max(fill, outline);
	color /= max(alpha, 0.00001);

	FragColor = vec4(color, alpha);
}
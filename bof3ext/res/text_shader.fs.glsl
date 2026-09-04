#version 430 core

out vec4 FragColor;

in vec2 TexCoord;
in vec4 VertColor;

uniform sampler2D tex;

const float SPREAD = 4.0;
const float OUTLINE_THICKNESS = 2.5;
const vec3 OUTLINE_COLOUR = vec3(0.1);

void main() {
	float dist = texture(tex, TexCoord).r;
    float dw = fwidth(dist);

    float pr = 0.5 / SPREAD;
    float spr = pr / dw;

    float odist = OUTLINE_THICKNESS / spr * pr;

    float fill = smoothstep(0.5 - dw, 0.5 + dw, dist);
    float outline = smoothstep(0.5 - odist - dw, 0.5 - odist + dw, dist);

    vec3 color = mix(OUTLINE_COLOUR, VertColor.rgb * 2 * 0.87, fill);

    float alpha = max(fill, outline);

    FragColor = vec4(color, alpha);
}
#version 430 core

out vec4 FragColor;

in vec2 TexCoord;
in vec4 VertColor;

uniform sampler2D tex;

void main() {
    FragColor = texture(tex, TexCoord) * VertColor;
}
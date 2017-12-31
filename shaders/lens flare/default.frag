#version 330 core

uniform sampler2D renderTexture;
uniform vec2 resolution;

out vec4 outColour;
in vec2 vs_out_texcoord;

void main(void)
{
    outColour = texture(renderTexture, vs_out_texcoord);
}

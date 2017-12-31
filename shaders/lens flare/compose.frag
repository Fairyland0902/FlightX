#version 330 core

uniform sampler2D sampler1;
uniform sampler2D sampler2;
uniform vec2 resolution;

out vec4 outColour;
in vec2 vs_out_texcoord;

void main(void)
{
    vec4 sampler1 = texture(sampler1, vs_out_texcoord);
    vec4 sampler2 = texture(sampler2, vs_out_texcoord);

    outColour = vec4(sampler1.rgb + sampler2.rgb, 1.0);
    // outColour = vec4(sampler1.rgb, 1.0f);
    // outColour = vec4(sampler2.rgb, 1.0f);
}

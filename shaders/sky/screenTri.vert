#version 330 core

// input
layout(location = 0) in vec2 vs_in_position;

// output
out vec2 vs_out_texcoord;

void main()
{
    gl_Position.xy = vs_in_position;
    gl_Position.zw = vec2(0.0, 1.0);
    vs_out_texcoord = vs_in_position * 0.5 + 0.5;
}
#version 330 core

layout(location = 0) in vec4 vPosition;

uniform mat4 model;
uniform mat4 view;

void main()
{
    gl_Position = view * model * vPosition;
}
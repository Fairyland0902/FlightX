#version 330 core

layout(location = 0) in vec2 vertPos;

void main()
{
    gl_Position.xy = vertPos;
    gl_Position.z = 0;
}
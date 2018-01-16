#version 330 core
layout(location = 0) in vec3 vertex; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(vertex,1.0);
}

#version 330 core
out vec3 color;

in vec2 UV;

uniform sampler2D grassTexture;

void main()
{
//    color = vec3(1.0f, 0.5f, 0.2f);
    color = texture(grassTexture, UV).rgb;
}
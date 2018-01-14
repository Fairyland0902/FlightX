#version 330 core
out vec3 color;

in vec2 UV;
in float height;

uniform sampler2D lowTex;
uniform sampler2D highTex;

void main()
{
//    color = vec3(1.0f, 0.5f, 0.2f);
    if (height > -200) {
        color = texture(highTex, UV).rgb;
    }
    else {
        color = texture(lowTex, UV).rgb;
    }
}
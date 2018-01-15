#version 330 core
out vec3 color;

in vec2 UV;
in float height;

uniform sampler2D lowTex;
uniform sampler2D highTex;
uniform sampler2D middleTex;

void main()
{
//    color = vec3(1.0f, 0.5f, 0.2f);
    if (height > -190) {
        color = texture(highTex, UV).rgb;
    }
    else if (height >= -200){
        vec3 color1 = texture(middleTex, UV).rgb;
        vec3 color2 = texture(highTex, UV).rgb;
        color = mix(color1, color2, height / 10 + 20);
    } else if (height >= -210) {
        vec3 color3 = texture(lowTex, UV).rgb;
        vec3 color4 = texture(middleTex, UV).rgb;
        color = mix(color3, color4, height / 10 + 21);
    } else {
        color = texture(lowTex, UV).rgb;
    }
}
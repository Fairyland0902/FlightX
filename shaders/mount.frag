#version 330 core
out vec3 color;

in vec2 UV;
in float height;

uniform sampler2D lowTex;
uniform sampler2D highTex;
uniform sampler2D middleTex;
uniform sampler2D baseTex;

void main()
{
//    color = vec3(1.0f, 0.5f, 0.2f);
    vec3 rock = texture(baseTex, UV).rgb;
    if (height > -185) {
        color = texture(highTex, UV).rgb;
    }
    else if (height >= -195){
        vec3 color1 = texture(middleTex, UV).rgb;
        vec3 color2 = texture(highTex, UV).rgb;
        color = mix(color1, color2, (height + 195) / 10);
    } else if (height >= -205) {
        color = texture(middleTex, UV).rgb;
    }
    else if (height >= -215) {
        vec3 color3 = texture(lowTex, UV).rgb;
        vec3 color4 = texture(middleTex, UV).rgb;
        color = mix(color3, color4, (height + 215) / 10);
    } else {
        color = texture(lowTex, UV).rgb;
    }
    color = mix(color, rock, 0.3);
}
#version 330 core

in vec2 texCoord;
in vec4 eyePosition;

out vec4 fColour;

uniform sampler2D sprite;
uniform mat4 view;
uniform vec4 worldPosCentre;

void main()
{
    vec4 eyeSpaceCentre = view * worldPosCentre;
    vec3 toCentre = eyeSpaceCentre.xyz - eyePosition.xyz;
    float distToCentreSquared = pow(toCentre.x, 2) + pow(toCentre.y, 2) + pow(toCentre.z, 2);
    fColour = texture(sprite, texCoord);

    // fColour.b /= clamp(distToCentreSquared * 2, 0, 1);
    // fColour.g /= clamp(distToCentreSquared * 2, 0, 1);

    // float maxDist = 60.0f;
    // vec4 a = fColour;
    // vec4 b = vec4(0.2, 0.2, 0.2, 1);
    // float k = min((distToCentreSquared / maxDist), 1);
    // fColour.r = a.r + (b.r - a.r) * k;
    // fColour.g = a.g + (b.g - a.g) * k;
    // fColour.b = a.b + (b.b - a.b) * k;

    fColour.a *= 1 - (gl_FragCoord.y / 720);

    // fColour = vec4(1.0, 0.0, 0.0, 1.0);
}
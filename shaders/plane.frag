#version 330 core
out vec3 color;

in vec2 UV;
in vec3 FragPos;
in vec4 FragPosLightSpace;

uniform sampler2D planeTexture;
uniform sampler2D shadowMap;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // Perspective division.
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Change the range to [0, 1].
    projCoords = projCoords * 0.5 + 0.5;
    // Fetch the closest depth in the shadow map.
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // Fetch the current depth in light space.
    float currentDepth = projCoords.z;
    // Test whether the current fragment is in shadow.
    float shadow = currentDepth > closestDepth ? 1.0 : 0.0;

    return shadow;
}

void main()
{
    //    color = vec3(1.0f, 0.5f, 0.2f);
    color = texture(planeTexture, UV).rgb;
}
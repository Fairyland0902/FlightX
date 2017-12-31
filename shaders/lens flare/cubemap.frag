// #version 330 core

// // Uniforms
// layout(std140) uniform View
// {
//     mat4 ViewMatrix;
//     mat4 ViewProjection;
//     mat4 InverseViewProjection;
//     vec3 CameraPosition;
//     vec3 CameraRight;
//     vec3 CameraUp;
//     vec3 CameraDir;
// };

// uniform samplerCube cubeMap;

// out vec4 outColour;
// in vec2 vs_out_texcoord;

// void main(void)
// {
//     // Ray tracing, generate a ray from COP to each pixel.
//     vec2 deviceCoord = 2.0 * vs_out_texcoord - 1.0;
//     vec4 rayOrigin = InverseViewProjection * vec4(deviceCoord, -1, 1);
//     rayOrigin.xyz /= rayOrigin.w;
//     vec4 rayTarget = InverseViewProjection * vec4(deviceCoord, 0, 1);
//     rayTarget.xyz /= rayTarget.w;
//     vec3 rayDirection = normalize(rayTarget.xyz - rayOrigin.xyz);

//     vec4 color = texture(cubeMap, rayDirection);
//     outColour = color;
// }

#version 150

in vec3 pos;

uniform samplerCube cubeMap;

out vec4 outColour;

void main(void)
{
    vec4 color = texture(cubeMap, pos);
    outColour = color;
}

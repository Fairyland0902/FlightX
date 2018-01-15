#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 vertexUV;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

out vec2 UV;
out vec3 FragPos;
out vec4 FragPosLightSpace;

void main()
{
    FragPos = vec3(model * vec4(position, 1.0));
    gl_Position = projection * view * model * vec4(position, 1.0);
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
    UV = vertexUV;
}

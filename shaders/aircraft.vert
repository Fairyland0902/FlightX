#version 330 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 norm_Ws;
layout(location = 2) in vec2 uv_Ws;

uniform mat4 VP;
uniform mat4 Model;
uniform mat4 lightSpaceMatrix;

out vec4 pos;
out vec4 FragPosLightSpace;
out vec3 norm;
out vec2 uv;

void main()
{
    pos = Model * vec4(vertPos, 1.0);
    gl_Position = VP * pos;
    norm = normalize(transpose(inverse(mat3(Model))) * norm_Ws);
    uv = uv_Ws;
    FragPosLightSpace = lightSpaceMatrix * pos;
}
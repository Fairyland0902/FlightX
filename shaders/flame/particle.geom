#version 330 core

layout(points) in;
layout(triangle_strip) out;
layout(max_vertices = 4) out;

out vec2 texCoord;
out vec4 eyePosition;

uniform mat4 projection;

void main()
{
    vec3 pos = gl_in[0].gl_Position.xyz;
    vec3 toCamera = normalize(-pos);
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = cross(toCamera, up);

    pos -= (right * 0.015);
    gl_Position = projection * vec4(pos, 1.0);
    texCoord = vec2(0, 0);
    eyePosition = vec4(pos, 1.0);
    EmitVertex();

    pos.y += 0.03;
    gl_Position = projection * vec4(pos, 1.0);
    texCoord = vec2(0, 1.0);
    eyePosition = vec4(pos, 1.0);
    EmitVertex();

    pos.y -= 0.03;
    pos += (right * 0.03);
    gl_Position = projection * vec4(pos, 1.0);
    texCoord = vec2(1.0, 0);
    eyePosition = vec4(pos, 1.0);
    EmitVertex();

    pos.y += 0.03;
    gl_Position = projection * vec4(pos, 1.0);
    texCoord = vec2(1.0, 1.0);
    eyePosition = vec4(pos, 1.0);
    EmitVertex();

    EndPrimitive();
}
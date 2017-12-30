#version 330 core
in vec3 norm;
in vec4 pos;
in vec2 uv;
out vec3 color;
uniform mat4 VP;
uniform sampler2D texture_diffuse;
void main() {
	color = vec3(texture(texture_diffuse, uv));
//color=vec3(1.0,0,0);
//color.xyz=((VP*pos).xyz);
}
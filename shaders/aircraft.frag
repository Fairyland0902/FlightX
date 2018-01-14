#version 330 core

in vec3 norm;
in vec4 pos;
in vec2 uv;

out vec4 color;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform Material material;

uniform sampler2D texture_diffuse1;
uniform samplerCube skyBox;

void main()
{
    vec3 N = normalize(norm);
    vec3 V = normalize(vec3(pos) - viewPos);
    vec3 L = normalize(-dirLight.direction);
    vec3 H = normalize(V + L);
    vec3 R = reflect(L, N);

    float N_dot_L = max(0.0, dot(N, L));
    float N_dot_H = max(0.0, dot(N, L));
    float pf = (N_dot_H == 0.0) ? 0.0 : pow(N_dot_H, material.shininess);

    // Ambient
    vec3 ambient = dirLight.ambient * vec3(texture(texture_diffuse1, uv));
    // Diffuse
    vec3 diffuse = dirLight.diffuse * N_dot_L * vec3(texture(texture_diffuse1, uv));
    // Specular
    vec3 specular = dirLight.specular * pf * vec3(texture(skyBox, R));

    color = vec4(ambient + diffuse + specular, 1.0f);
    // color = texture(skyBox, R);

    // color = vec3(texture(texture_diffuse, uv));
    //color=vec3(1.0,0,0);
    //color.xyz=((VP*pos).xyz);
}
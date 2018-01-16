#version 330 core
out vec4 Fragcolor;

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

in vec3 FragPos;
in vec3 Normal;
in vec2 UV;
in float height;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform Material material;

uniform sampler2D lowTex;
uniform sampler2D highTex;
uniform sampler2D middleTex;
uniform sampler2D baseTex;

void main()
{
    // Generate the diffuse texture.
    vec4 diffuseTexture;

    vec4 rock = texture(baseTex, UV);
    if (height > -185) {
        diffuseTexture = texture(highTex, UV);
    } else if (height >= -195) {
        vec4 color1 = texture(middleTex, UV);
        vec4 color2 = texture(highTex, UV);
        diffuseTexture = mix(color1, color2, (height + 195) / 10);
    } else if (height >= -205) {
        diffuseTexture = texture(middleTex, UV);
    } else if (height >= -215) {
        vec4 color3 = texture(lowTex, UV);
        vec4 color4 = texture(middleTex, UV);
        diffuseTexture = mix(color3, color4, (height + 215) / 10);
    } else {
        diffuseTexture = texture(lowTex, UV);
    }
    diffuseTexture = mix(diffuseTexture, rock, 0.3);

    // Calculate light effect.
    vec3 N = normalize(Normal);
    vec3 L = normalize(-dirLight.direction);
    vec3 V = normalize(FragPos - viewPos);
    vec3 H = normalize(V + L);

    // Use Blinn-Phong shading model to calculate color.
    float N_dot_L = max(0.0, dot(N, L));
    float N_dot_H = max(0.0, dot(N, L));
    float pf = (N_dot_H == 0.0) ? 0.0 : pow(N_dot_H, material.shininess);

    // Ambient
    vec3 ambient = dirLight.ambient * vec3(diffuseTexture);
    // Diffuse
    vec3 diffuse = dirLight.diffuse * N_dot_L * vec3(diffuseTexture);
    // Specular
    vec3 specular = dirLight.specular * pf * material.specular;

    Fragcolor = vec4(ambient + diffuse + specular, 1.0);
}
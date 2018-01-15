#version 330 core

in vec3 norm;
in vec4 pos;
in vec4 FragPosLightSpace;
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
uniform sampler2D shadowMap;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    // Perspective division.
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Change the range to [0, 1].
    projCoords = projCoords * 0.5 + 0.5;
    // The fragment is farther than the fat plane of light.
    if (projCoords.z > 1.0)
        return 1.0;
    // Fetch the current depth in light space.
    float currentDepth = projCoords.z;
    // Test whether the current fragment is in shadow.
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.005);
    // PCF.
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    return shadow;
}

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
    // Shadow
    float shadow = ShadowCalculation(FragPosLightSpace, N, L);

    color = vec4(ambient + (1 - shadow) * (diffuse + specular), 1.0);
    // color = (shadow == 0) ? vec4(0.0, 0.0, 0.0, 1.0) : vec4(1.0, 1.0, 1.0, 1.0);

    // color = texture(skyBox, R);

    // color = vec3(texture(texture_diffuse, uv));
    //color=vec3(1.0,0,0);
    //color.xyz=((VP*pos).xyz);
}
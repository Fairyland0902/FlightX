#version 330 core
out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

// 对于太阳光的 shading 我觉得用平行光比较合适，所以我把点光源和探照灯都注释掉了。
struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// struct PointLight {
//     vec3 position;

//     float constant;
//     float linear;
//     float quadratic;

//     vec3 ambient;
//     vec3 diffuse;
//     vec3 specular;
// };

// struct SpotLight {
//     vec3 position;
//     vec3 direction;
//     float cutOff;
//     float outerCutOff;

//     float constant;
//     float linear;
//     float quadratic;

//     vec3 ambient;
//     vec3 diffuse;
//     vec3 specular;
// };

// #define NR_POINT_LIGHTS 4

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform DirLight dirLight;
// uniform PointLight pointLights[NR_POINT_LIGHTS];
// uniform SpotLight spotLight;
uniform Material material;

// 这是对 shader 中采样器名字的约定，这样才能在 model 的构造中被正确赋值。如果有多种纹理可以继续命名为 texture_diffuse2、texture_diffuse3 etc.
// 这里我遇到过一个坑，就是有的模型有 diffuse texture 和 specular texture，但有的模型只有 diffuse texture，如果统一用一个 shader 的话，在渲染只有 diffuse texture 的模型时会被错误渲染上其他模型的 specular texture，所以需要进行区分。
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

// Function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
// vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
// vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // == =====================================================
    // Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
    // For each phase, a calculate function is defined that calculates the corresponding color
    // per lamp. In the main() function we take all the calculated colors and sum them up for
    // this fragment's final color.
    // == =====================================================
    // phase 1: directional lighting
    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    // // phase 2: point lights
    // for (int i = 0; i < NR_POINT_LIGHTS; i++)
    //     result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    // // phase 3: spot light
    // result += CalcSpotLight(spotLight, norm, FragPos, viewDir);

    FragColor = vec4(result, 1.0);
}

// Calculates the color when using a directional light
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    vec3 halfDir = normalize(viewDir + lightDir);

    // 这里用了 half vector 来代替 reflect vector（纯粹是个人喜好 orz）
    float N_dot_L = max(0.0, dot(normal, lightDir));
    float N_dot_H = max(0.0, dot(normal, halfDir));
    float pf = (N_dot_H == 0.0) ? 0.0 : pow(N_dot_H, material.shininess);

    // 如果有多个纹理的话对多个纹理都要进行计算
    vec3 ambient, diffuse, specular;
    // Ambient
    ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));
    // Diffuse
    diffuse = light.diffuse * N_dot_L * vec3(texture(texture_diffuse1, TexCoords));
    // Specular
    specular = light.specular * pf * vec3(texture(texture_specular1, TexCoords));

    return (ambient + diffuse + specular);
}

// // Calculates the color when using a point light
// vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
// {
//     vec3 lightDir = normalize(light.position - fragPos);
//     vec3 halfDir = normalize(viewDir + lightDir);

//     float N_dot_L = max(0.0, dot(normal, lightDir));
//     float N_dot_H = max(0.0, dot(normal, halfDir));

//     float pf = (N_dot_H == 0.0) ? 0.0 : pow(N_dot_H, material.shininess);

//     vec3 ambient, diffuse, specular;
//     if (renderTexture) {
//         // Ambient
//         ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));
//         // Diffuse
//         diffuse = light.diffuse * N_dot_L * vec3(texture(texture_diffuse1, TexCoords));
//         // Specular
//         specular = light.specular * pf * material.specular;
//     } else {
//         // Ambient
//         ambient = light.ambient * material.ambient;
//         // Diffuse
//         diffuse = light.diffuse * material.diffuse * N_dot_L;
//         // Specular
//         specular = light.specular * material.specular * pf;
//     }

//     // Attenuation
//     float distance = length(light.position - fragPos);
//     float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
//     ambient *= attenuation;
//     diffuse *= attenuation;
//     specular *= attenuation;

//     return (ambient + diffuse + specular);
// }

// // Calculates the color when using a spot light
// vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
// {
//     vec3 lightDir = normalize(light.position - fragPos);
//     vec3 halfDir = normalize(viewDir + lightDir);

//     float N_dot_L = max(0.0, dot(normal, lightDir));
//     float N_dot_H = max(0.0, dot(normal, halfDir));

//     float pf = (N_dot_H == 0.0) ? 0.0 : pow(N_dot_H, material.shininess);

//     vec3 ambient, diffuse, specular;
//     if (renderTexture) {
//         // Ambient
//         ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));
//         // Diffuse
//         diffuse = light.diffuse * N_dot_L * vec3(texture(texture_diffuse1, TexCoords));
//         // Specular
//         specular = light.specular * pf * material.specular;
//     } else {
//         // Ambient
//         ambient = light.ambient * material.ambient;
//         // Diffuse
//         diffuse = light.diffuse * material.diffuse * N_dot_L;
//         // Specular
//         specular = light.specular * material.specular * pf;
//     }

//     // Attenuation
//     float distance = length(light.position - fragPos);
//     float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

//     // Spotlight intensity
//     float theta = dot(lightDir, normalize(-light.direction));
//     float epsilon = light.cutOff - light.outerCutOff;
//     float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

//     // Combine results
//     ambient *= attenuation * intensity;
//     diffuse *= attenuation * intensity;
//     specular *= attenuation * intensity;

//     return (ambient + diffuse + specular);
// }

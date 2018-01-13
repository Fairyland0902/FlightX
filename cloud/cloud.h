#ifndef _CLOUD_H
#define _CLOUD_H

#include <memory>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "screenAlignedTriangle.h"
#include <shader.h>

/* References:
 * http://dl.acm.org/citation.cfm?id=1730831
 */

class Cloud
{
public:
    Cloud(int screenResolutionX, int screenResolutionY, float farPlaneDistance, class ScreenAlignedTriangle &screenTri);

    ~Cloud();

    void Draw(const glm::mat4 &inverseViewProjection, const glm::mat4 &view,
              const glm::vec3 &cameraDirection, const glm::vec3 &cameraPosition, const glm::vec3 &lightDir);

private:
    void shader_init();

    void fbo_init();

    void sampler_init();

    void buffer_init();

    void noise_init();

    void moveClouds();

    void renderFOM(const glm::mat4 &inverseViewProjection, const glm::vec3 &viewDir, const glm::vec3 &cameraPos,
                   const glm::vec3 &lightDirection);

    void renderClouds(const glm::vec3 &lightDir, const glm::mat4 &viewMatrix);

    void particleSorting();

    void createLightMatrices(glm::mat4 &lightView, glm::mat4 &lightProjection, float &farPlaneDistance,
                             const glm::mat4 &viewProj,
                             const glm::vec3 &viewDir, const glm::vec3 &cameraPos,
                             const glm::vec3 &lightDir);

    // shader
    Shader moveShader;
    Shader fomShader;
    Shader fomFilterShader;
    Shader renderingShader;

    // screen infos
    unsigned int screenResolutionX, screenResolutionY;
    float farPlaneDistance;

    // light infos (updated every frame)
    float lightFarPlane;
    glm::mat4 lightProjection;
    glm::mat4 lightView;
    glm::mat4 lightViewProjection;
    float lightDistancePlane_Norm[4];

    // fom shader uniform indices
    GLuint fomShaderUniformIndex_cameraX;
    GLuint fomShaderUniformIndex_cameraY;
    GLuint fomShaderUniformIndex_cameraZ;
    GLuint fomShaderUniformIndex_lightViewProjection;
    GLuint fomShaderUniformIndex_LightDistancePlane_norm;
    // fom filter uniform indices
    GLuint fomFilterShaderUniformIndex_Offset;
    // rendering shader unifrom indices
    GLuint renderingShaderUniformIndex_lightViewProjection;
    GLuint renderingShaderUniformIndex_LightDistancePlane_norm;
    GLuint renderingShaderUniformIndex_LightDirection;

    // cloud particle data
    GLuint vbo_cloudParticleBuffer_Read[3];
    GLuint vbo_cloudParticleBuffer_Write[3];
    GLuint vao_cloudParticleBuffer_Read;
    GLuint vao_cloudParticleBuffer_Write;
    GLuint ibo_cloudParticleRendering;

    // buffer for cpu write/read operations
    std::unique_ptr<float[]> particleDepthBuffer;
    std::unique_ptr<unsigned short[]> particleIndexBuffer;
    unsigned int numParticlesRender;

    // FOM - two for filtering
    GLuint fourierOpacityMap_Textures[2][2];
    GLuint fourierOpacityMap_FBO[2];

    // noise
    GLuint noiseTexture;

    // sampler
    GLuint linearSampler_noMipMaps;
    GLuint linearSampler_MipMaps;

    // screen tri
    ScreenAlignedTriangle &screenTri;

    // mrt settings
    static const GLuint drawBuffers_One[1];
    static const GLuint drawBuffers_Two[2];


    static const char *transformFeedbackVaryings[];
    static const unsigned int maxNumCloudParticles;
    static const unsigned int fourierOpacityMapSize;
    static const unsigned int noiseTextureSize;
};

#endif

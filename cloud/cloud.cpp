#include <algorithm>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "cloud.h"
#include "../resource_manager.h"
#include "../stb_image.h"

const unsigned int Cloud::maxNumCloudParticles = 10000;// 16384;
const unsigned int Cloud::fourierOpacityMapSize = 1024;

const GLuint Cloud::drawBuffers_Two[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
const GLuint Cloud::drawBuffers_One[1] = {GL_COLOR_ATTACHMENT0};

template<class T>
void swap(T &a, T &b)
{
    T temp = b;
    b = a;
    a = temp;
}

glm::vec3 preMultiply(glm::vec3 v, glm::mat4 m)
{
    glm::vec3 r;

    float inv_w = 1.0f / (m[0][3] * v.x + m[1][3] * v.y + m[2][3] * v.z + m[3][3]);

    r.x = (m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0]) * inv_w;
    r.y = (m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1]) * inv_w;
    r.z = (m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2]) * inv_w;

    return r;
}

glm::vec3 transformNormal(const glm::vec3 &v, const glm::mat4 &m)
{
    glm::vec3 r;

    r.x = (m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z);
    r.y = (m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z);
    r.z = (m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z);

    r = glm::normalize(r);

    return r;
}

Cloud::Cloud(int screenResolutionX, int screenResolutionY, float farPlaneDistance, ScreenAlignedTriangle &screenTri) :
        screenResolutionX(screenResolutionX),
        screenResolutionY(screenResolutionY),
        farPlaneDistance(farPlaneDistance),
        particleDepthBuffer(new float[maxNumCloudParticles]),
        particleIndexBuffer(new unsigned short[maxNumCloudParticles]),
        screenTri(screenTri)
{
    moveShader = ResourceManager::GetShader("cloud move");
    fomShader = ResourceManager::GetShader("FOM");
    fomFilterShader = ResourceManager::GetShader("FOM filter");
    renderingShader = ResourceManager::GetShader("cloud render");

    shader_init();
    fbo_init();
    buffer_init();
    sampler_init();
    noise_init();
}

Cloud::~Cloud()
{
    glDeleteFramebuffers(1, &fourierOpacityMap_FBO[0]);
    glDeleteFramebuffers(1, &fourierOpacityMap_FBO[1]);
    glDeleteTextures(2, fourierOpacityMap_Textures[0]);
    glDeleteTextures(2, fourierOpacityMap_Textures[1]);
    glDeleteSamplers(1, &linearSampler_noMipMaps);
    glDeleteSamplers(1, &linearSampler_MipMaps);
    glDeleteVertexArrays(1, &vao_cloudParticleBuffer_Read);
    glDeleteVertexArrays(1, &vao_cloudParticleBuffer_Write);
    glDeleteBuffers(1, &vbo_cloudParticleBuffer_Read[0]);
    glDeleteBuffers(1, &vbo_cloudParticleBuffer_Write[0]);
    glDeleteBuffers(1, &vbo_cloudParticleBuffer_Read[1]);
    glDeleteBuffers(1, &vbo_cloudParticleBuffer_Write[1]);
    glDeleteBuffers(1, &vbo_cloudParticleBuffer_Read[2]);
    glDeleteBuffers(1, &vbo_cloudParticleBuffer_Write[2]);
    glDeleteBuffers(1, &ibo_cloudParticleRendering);
    glDeleteBuffers(1, &noiseTexture);
}

void Cloud::shader_init()
{
    // Set ubo bindings, get uniform locations.
    // move
    // View binding = 1
    GLuint blockIndex = glGetUniformBlockIndex(moveShader.ID, "View");
    glUniformBlockBinding(moveShader.ID, blockIndex, 1);
    // Timings binding = 2
    blockIndex = glGetUniformBlockIndex(moveShader.ID, "Timings");
    glUniformBlockBinding(moveShader.ID, blockIndex, 2);

    // FOM
    fomShader.Use();
    fomShaderUniformIndex_cameraX = glGetUniformLocation(fomShader.ID, "CameraRight");
    fomShaderUniformIndex_cameraY = glGetUniformLocation(fomShader.ID, "CameraUp");
    fomShaderUniformIndex_cameraZ = glGetUniformLocation(fomShader.ID, "CameraDir");
    fomShaderUniformIndex_LightDistancePlane_norm = glGetUniformLocation(fomShader.ID, "LightDistancePlane_norm");
    fomShaderUniformIndex_lightViewProjection = glGetUniformLocation(fomShader.ID, "LightViewProjection");
    glUniform1i(glGetUniformLocation(fomShader.ID, "NoiseTexture"), 0);

    // FOM filter
    fomFilterShader.Use();
    glUniform1i(glGetUniformLocation(fomFilterShader.ID, "FOMSampler0"), 1);
    glUniform1i(glGetUniformLocation(fomFilterShader.ID, "FOMSampler1"), 2);
    fomFilterShaderUniformIndex_Offset = glGetUniformLocation(fomFilterShader.ID, "Offset");

    // Rendering
    renderingShader.Use();
    // Screen binding = 0
    blockIndex = glGetUniformBlockIndex(renderingShader.ID, "Screen");
    glUniformBlockBinding(renderingShader.ID, blockIndex, 0);
    // View binding = 1
    blockIndex = glGetUniformBlockIndex(renderingShader.ID, "View");
    glUniformBlockBinding(renderingShader.ID, blockIndex, 1);
    renderingShaderUniformIndex_lightViewProjection = glGetUniformLocation(renderingShader.ID, "LightViewProjection");
    renderingShaderUniformIndex_LightDistancePlane_norm = glGetUniformLocation(renderingShader.ID,
                                                                               "LightDistancePlane_norm");
    renderingShaderUniformIndex_LightDirection = glGetUniformLocation(renderingShader.ID, "LightDirection_viewspace");

    glUniform1i(glGetUniformLocation(renderingShader.ID, "NoiseTexture"), 0);
    glUniform1i(glGetUniformLocation(renderingShader.ID, "FOMSampler0"), 1);
    glUniform1i(glGetUniformLocation(renderingShader.ID, "FOMSampler1"), 2);
}

void Cloud::fbo_init()
{
    // Data Stuff.
    // Textures
    for (int i = 0; i < 2; ++i)
    {
        glGenTextures(2, fourierOpacityMap_Textures[i]);

        glBindTexture(GL_TEXTURE_2D, fourierOpacityMap_Textures[i][0]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, fourierOpacityMapSize, fourierOpacityMapSize, 0, GL_RGBA, GL_FLOAT,
                     0);

        glBindTexture(GL_TEXTURE_2D, fourierOpacityMap_Textures[i][1]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, fourierOpacityMapSize, fourierOpacityMapSize, 0, GL_RGBA, GL_FLOAT,
                     0);

        // FBO
        glGenFramebuffers(1, &fourierOpacityMap_FBO[i]);
        glBindFramebuffer(GL_FRAMEBUFFER, fourierOpacityMap_FBO[i]);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, fourierOpacityMap_Textures[i][0], 0);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, fourierOpacityMap_Textures[i][1], 0);
        glDrawBuffers(2, drawBuffers_Two);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void Cloud::sampler_init()
{
    // Sampler.
    glGenSamplers(1, &linearSampler_noMipMaps);
    glSamplerParameteri(linearSampler_noMipMaps, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(linearSampler_noMipMaps, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(linearSampler_noMipMaps, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(linearSampler_noMipMaps, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(linearSampler_noMipMaps, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glGenSamplers(1, &linearSampler_MipMaps);
    glSamplerParameteri(linearSampler_MipMaps, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameteri(linearSampler_MipMaps, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(linearSampler_MipMaps, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(linearSampler_MipMaps, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(linearSampler_MipMaps, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Cloud::buffer_init()
{
    // Data objects.
    // Init with random lifetimes as seed.
    std::unique_ptr<float[]> startValues(new float[maxNumCloudParticles * 3]);
    memset(startValues.get(), 0, sizeof(float) * maxNumCloudParticles * 3);

    glGenBuffers(3, vbo_cloudParticleBuffer_Read);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_cloudParticleBuffer_Read[0]);
    glBufferData(GL_ARRAY_BUFFER, maxNumCloudParticles * sizeof(float) * 3, 0, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_cloudParticleBuffer_Read[1]);
    glBufferData(GL_ARRAY_BUFFER, maxNumCloudParticles * sizeof(float) * 3, startValues.get(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_cloudParticleBuffer_Read[2]);
    glBufferData(GL_ARRAY_BUFFER, maxNumCloudParticles * sizeof(float) * 1, 0, GL_STATIC_READ);    // static read!
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(3, vbo_cloudParticleBuffer_Write);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_cloudParticleBuffer_Write[0]);
    glBufferData(GL_ARRAY_BUFFER, maxNumCloudParticles * sizeof(float) * 3, 0, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_cloudParticleBuffer_Write[1]);
    glBufferData(GL_ARRAY_BUFFER, maxNumCloudParticles * sizeof(float) * 3, startValues.get(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_cloudParticleBuffer_Write[2]);
    glBufferData(GL_ARRAY_BUFFER, maxNumCloudParticles * sizeof(float) * 1, 0, GL_STATIC_READ);    // static read!
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // IBO
    for (unsigned short i = 0; i < maxNumCloudParticles; ++i)
        particleIndexBuffer[i] = i;
    glGenBuffers(1, &ibo_cloudParticleRendering);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cloudParticleRendering);
    // dynamic draw! often writen, often rendered, never read (http://wiki.delphigl.com/index.php/glBufferData)
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2 * maxNumCloudParticles, particleIndexBuffer.get(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


    // Generate vao for cloud particles (2 for ping/pong).
    glGenVertexArrays(1, &vao_cloudParticleBuffer_Read);
    glBindVertexArray(vao_cloudParticleBuffer_Read);
    // Position attribute.
    glBindBuffer(GL_ARRAY_BUFFER, vbo_cloudParticleBuffer_Read[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (GLvoid *) 0);
    glEnableVertexAttribArray(0);
    // Size/lifetime/rand attribute.
    glBindBuffer(GL_ARRAY_BUFFER, vbo_cloudParticleBuffer_Read[1]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (GLvoid *) 0);
    glEnableVertexAttribArray(1);
    // Depth attribute.
    glBindBuffer(GL_ARRAY_BUFFER, vbo_cloudParticleBuffer_Read[2]);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(float), (GLvoid *) 0);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glGenVertexArrays(1, &vao_cloudParticleBuffer_Write);
    glBindVertexArray(vao_cloudParticleBuffer_Write);
    // Position attribute.
    glBindBuffer(GL_ARRAY_BUFFER, vbo_cloudParticleBuffer_Write[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (GLvoid *) 0);
    glEnableVertexAttribArray(0);
    // Size/lifetime/rand attribute.
    glBindBuffer(GL_ARRAY_BUFFER, vbo_cloudParticleBuffer_Write[1]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (GLvoid *) 0);
    glEnableVertexAttribArray(1);
    // Depth attribute.
    glBindBuffer(GL_ARRAY_BUFFER, vbo_cloudParticleBuffer_Write[2]);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(float), (GLvoid *) 0);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Cloud::noise_init()
{
    /*glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    auto noise = PerlinNoiseGenerator::get().generate(noiseTextureSize,noiseTextureSize, 0.006f, 0.3f, 0.2f, 5, 0.3f);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, noiseTextureSize, noiseTextureSize, 0, GL_RED, GL_UNSIGNED_BYTE, noise.get());
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);*/

    int TexSizeX, TexSizeY;
    stbi_uc *TextureData = stbi_load("../textures/particle.png", &TexSizeX, &TexSizeY, NULL, 4);
    if (!TextureData)
        std::cout << "Error while loading texture `particle.png`" << std::endl;

    glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TexSizeX, TexSizeX, 0, GL_RGBA, GL_UNSIGNED_BYTE, TextureData);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(TextureData);
}

void Cloud::particleSorting()
{
    // Read depth vbo.
    glBindBuffer(GL_ARRAY_BUFFER, vbo_cloudParticleBuffer_Write[2]);
    glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * maxNumCloudParticles, particleDepthBuffer.get());
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Sort by depth.
    std::sort(particleIndexBuffer.get(), particleIndexBuffer.get() + maxNumCloudParticles,
              [&](size_t i, size_t j)
              { return particleDepthBuffer[i] > particleDepthBuffer[j]; });

    // Count num visible (the cloudMove.vert wrote a large depth value for all particles left/right/up/down/behind frustum).
    unsigned int numParticlesInvalid = 0;
    while (numParticlesInvalid < maxNumCloudParticles &&
           particleDepthBuffer[particleIndexBuffer[numParticlesInvalid]] > farPlaneDistance)
        ++numParticlesInvalid;
    numParticlesRender = maxNumCloudParticles - numParticlesInvalid;

    // Write ibo.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cloudParticleRendering);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(unsigned short) * numParticlesRender,
                    particleIndexBuffer.get() + numParticlesInvalid);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    std::cout << "Num Particles rendered      : " << numParticlesRender << " \r";
}

void Cloud::moveClouds()
{
    glBindVertexArray(vao_cloudParticleBuffer_Read);
    glEnable(GL_RASTERIZER_DISCARD);

    moveShader.Use();
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, vbo_cloudParticleBuffer_Write[0]);    // specify targets
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, vbo_cloudParticleBuffer_Write[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, vbo_cloudParticleBuffer_Write[2]);
    glBeginTransformFeedback(GL_POINTS);
    glDrawArrays(GL_POINTS, 0, maxNumCloudParticles);
    glEndTransformFeedback();

    glDisable(GL_RASTERIZER_DISCARD);
}

void Cloud::createLightMatrices(glm::mat4 &lightView, glm::mat4 &lightProjection, float &farPlaneDistance,
                                const glm::mat4 &viewProj, const glm::vec3 &viewDir, const glm::vec3 &cameraPos,
                                const glm::vec3 &lightDir)
{
    glm::vec3 lightUp(viewDir.x, -(lightDir.z * viewDir.z + lightDir.x * viewDir.x) / lightDir.y, viewDir.z);
    glm::mat4 matLightCamera = glm::lookAt(glm::vec3(0.0f), -lightDir, lightUp);
    glm::mat4 mViewProjInv_LightCamera = matLightCamera * glm::inverse(viewProj);

    glm::vec3 edges[8];
    edges[0] = preMultiply(glm::vec3(-1.0f, -1.0f, -1.0f), mViewProjInv_LightCamera);
    edges[1] = preMultiply(glm::vec3(1.0f, -1.0f, -1.0f), mViewProjInv_LightCamera);
    edges[2] = preMultiply(glm::vec3(-1.0f, 1.0f, -1.0f), mViewProjInv_LightCamera);
    edges[3] = preMultiply(glm::vec3(1.0f, 1.0f, -1.0f), mViewProjInv_LightCamera);
    edges[4] = preMultiply(glm::vec3(-1.0f, -1.0f, 1.0f), mViewProjInv_LightCamera);
    edges[5] = preMultiply(glm::vec3(1.0f, -1.0f, 1.0f), mViewProjInv_LightCamera);
    edges[6] = preMultiply(glm::vec3(-1.0f, 1.0f, 1.0f), mViewProjInv_LightCamera);
    edges[7] = preMultiply(glm::vec3(1.0f, 1.0f, 1.0f), mViewProjInv_LightCamera);

    // Create box.
    glm::vec3 min(999999, 999999, 999999);
    glm::vec3 max(-999999, -999999, -999999);
    for (int i = 0; i < 8; i++)
    {
        min.x = std::min(edges[i].x, min.x);
        min.y = std::min(edges[i].y, min.y);
        min.z = std::min(edges[i].z, min.z);
        max.x = std::max(edges[i].x, max.x);
        max.y = std::max(edges[i].y, max.y);
        max.z = std::max(edges[i].z, max.z);
    }

    // Find good camera position.
    farPlaneDistance = max.z - min.z;
    glm::vec3 lightPos = preMultiply(glm::vec3((min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f, min.z),
                                     glm::inverse(matLightCamera));
    lightView = glm::lookAt(lightPos, lightPos + lightDir, lightUp);
    // Create light projection matrix (orthogonal projection).
    lightProjection = glm::ortho(min.x, max.x, min.y, max.y, 0.0f, farPlaneDistance);
}

void
Cloud::renderFOM(const glm::mat4 &inverseViewProjection, const glm::vec3 &cameraDirection,
                 const glm::vec3 &cameraPosition, const glm::vec3 &lightDir)
{
    fomShader.Use();

    // Noise texture.
    // Noise is always on 0; the FOM textures are on 1 and 2.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glBindSampler(0, linearSampler_MipMaps);

    // Matrix setup.
    createLightMatrices(lightView, lightProjection, lightFarPlane, glm::inverse(inverseViewProjection), cameraDirection,
                        cameraPosition, lightDir);
    lightViewProjection = lightProjection * lightView;

    glUniform3fv(fomShaderUniformIndex_cameraX, 1,
                 glm::value_ptr(glm::vec3(lightView[0][0], lightView[1][0], lightView[2][0])));
    glUniform3fv(fomShaderUniformIndex_cameraY, 1,
                 glm::value_ptr(glm::vec3(lightView[0][1], lightView[1][1], lightView[2][1])));
    glUniform3fv(fomShaderUniformIndex_cameraZ, 1,
                 glm::value_ptr(-glm::vec3(lightView[0][2], lightView[1][2], lightView[2][2])));
    glUniformMatrix4fv(fomShaderUniformIndex_lightViewProjection, 1, GL_FALSE, glm::value_ptr(lightViewProjection));
    lightDistancePlane_Norm[0] = -lightView[0][2] / lightFarPlane;
    lightDistancePlane_Norm[1] = -lightView[1][2] / lightFarPlane;
    lightDistancePlane_Norm[2] = -lightView[2][2] / lightFarPlane;
    lightDistancePlane_Norm[3] = -lightView[3][2] / lightFarPlane;
    glUniform4fv(fomShaderUniformIndex_LightDistancePlane_norm, 1, lightDistancePlane_Norm);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glViewport(0, 0, fourierOpacityMapSize, fourierOpacityMapSize);

    glBindFramebuffer(GL_FRAMEBUFFER, fourierOpacityMap_FBO[0]);
    glDrawBuffers(2, drawBuffers_Two);

    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_POINTS, 0, maxNumCloudParticles);

    glDisable(GL_BLEND);

    // Sampler states for fom-textures.
    glBindSampler(1, linearSampler_noMipMaps);
    glBindSampler(2, linearSampler_noMipMaps);


    // Filter FOM.
    // shader
    fomFilterShader.Use();
    // horizontal
    glBindFramebuffer(GL_FRAMEBUFFER, fourierOpacityMap_FBO[1]);
    glDrawBuffers(2, drawBuffers_Two);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, fourierOpacityMap_Textures[0][0]);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, fourierOpacityMap_Textures[0][1]);
    glUniform4f(fomFilterShaderUniformIndex_Offset, 1.5f / fourierOpacityMapSize, 0, -1.5f / fourierOpacityMapSize, 0);
    screenTri.Draw();
    // vertical
    glBindFramebuffer(GL_FRAMEBUFFER, fourierOpacityMap_FBO[0]);
    glDrawBuffers(2, drawBuffers_Two);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, fourierOpacityMap_Textures[1][0]);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, fourierOpacityMap_Textures[1][1]);
    glUniform4f(fomFilterShaderUniformIndex_Offset, 0.0f, 1.5f / fourierOpacityMapSize, 0.0f,
                -1.5f / fourierOpacityMapSize);
    screenTri.Draw();

    // Reset to back buffer rendering.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawBuffers(1, drawBuffers_One);
    glViewport(0, 0, screenResolutionX, screenResolutionY);
}

void Cloud::renderClouds(const glm::vec3 &lightDir, const glm::mat4 &viewMatrix)
{
    // Render clouds.
    renderingShader.Use();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cloudParticleRendering);
    glBindVertexArray(vao_cloudParticleBuffer_Read);

    // Light settings.
    glUniformMatrix4fv(renderingShaderUniformIndex_lightViewProjection, 1, GL_FALSE,
                       glm::value_ptr(lightViewProjection));
    glUniform4fv(renderingShaderUniformIndex_LightDistancePlane_norm, 1, lightDistancePlane_Norm);
    glUniform3fv(renderingShaderUniformIndex_LightDirection, 1, glm::value_ptr(transformNormal(-lightDir, viewMatrix)));

    glBindSampler(1, linearSampler_noMipMaps);
    glBindSampler(2, linearSampler_noMipMaps);


    // Textures.
    // Noise is always on 0; the FOM textures are on 1 and 2.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glBindSampler(0, linearSampler_MipMaps);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, fourierOpacityMap_Textures[0][0]);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, fourierOpacityMap_Textures[0][1]);

    // Blending.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Buffers.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cloudParticleRendering);
    glBindVertexArray(vao_cloudParticleBuffer_Read);

    glDrawElements(GL_POINTS, numParticlesRender, GL_UNSIGNED_SHORT, 0);

    // Reset stuff.
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDisable(GL_BLEND);

    // Reset textures.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Cloud::Draw(const glm::mat4 &inverseViewProjection, const glm::mat4 &view, const glm::vec3 &cameraDirection,
                 const glm::vec3 &cameraPosition, const glm::vec3 &lightDir)
{
    glDisable(GL_DEPTH_TEST);

    /*
     * through all the following passes the "read" buffer will be read
     * in the "write" buffer comes new data, that will be sorted
     * i'm hoping to maximes parallism this way: first the data will be updated, but everyone uses the old
     * then (still rendering) the updated particles will be sorted - frame ended, buffer switch
     * (so new data is used with a delay!)
     */

    // Move clouds.
    moveClouds();
    // Render FOM.
    renderFOM(inverseViewProjection, cameraDirection, cameraPosition, lightDir);
    // Render clouds.
    renderClouds(lightDir, view);
    // Sort the new hopefully ready particles, while hopefully the screen itself is drawing.
    particleSorting();

    // Rotate read/write buffer.
    swap(vao_cloudParticleBuffer_Read, vao_cloudParticleBuffer_Write);
    swap(vbo_cloudParticleBuffer_Read[0], vbo_cloudParticleBuffer_Write[0]);
    swap(vbo_cloudParticleBuffer_Read[1], vbo_cloudParticleBuffer_Write[1]);
    swap(vbo_cloudParticleBuffer_Read[2], vbo_cloudParticleBuffer_Write[2]);
}
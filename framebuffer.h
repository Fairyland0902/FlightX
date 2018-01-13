#ifndef _FRAMEBUFFER_H
#define _FRAMEBUFFER_H

#include "texture.h"
#include "shader.h"

class Framebuffer
{
public:
    Framebuffer(int width, int height, bool depth = false);

    ~Framebuffer();

    void Start(float downSamplingRatio = 1.0);

    void End();

    void Bind(Shader *shader, glm::vec2 resolution, string textureName) const;

    void Init();

    void Clear() const;

    void AttachTexture(Texture2D *renderTexture_, string texturenName_);

    void Resize(int width, int height);

    void Upscale(float ratio);

    int Width() const;

    int Height() const;

    Texture2D *GetRenderTexture() const;

    GLuint id;
    GLuint depthId;

private:
    void ScaleRenderTarget(float ratio);

    int width, height;
    Texture2D *renderTexture;
    string textureName;
    bool depth;
    float currentSampleRatio;
};

#endif

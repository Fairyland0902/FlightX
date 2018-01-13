#include <iostream>
#include "framebuffer.h"

Framebuffer::Framebuffer(int width_, int height_, bool depth_)
{
    id = 0;
    depth = depth_;
    width = width_;
    height = height_;

    renderTexture = new Texture2D();
}

Framebuffer::~Framebuffer()
{
    glDeleteRenderbuffers(1, &depthId);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &id);
    delete renderTexture;
}

void Framebuffer::Resize(int width_, int height_)
{
    width = width_;
    height = height_;

    // TODO : only if depth
    glBindRenderbuffer(GL_RENDERBUFFER_EXT, depthId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    renderTexture->Resize(width, height);
}

void Framebuffer::ScaleRenderTarget(float ratio)
{
    renderTexture->Resize(width * ratio, height * ratio);
}

void Framebuffer::Start(float downSamplingRatio)
{
    assert(downSamplingRatio <= 1.0);
    // Down scale render target.
    ScaleRenderTarget(downSamplingRatio);
    glViewport(0, 0, width * downSamplingRatio, height * downSamplingRatio);
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    Clear();
}

void Framebuffer::End()
{
    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Bind(Shader *shader, glm::vec2 resolution, string textureName) const
{
    // TODO : move this to renderer
    shader->Use();
    renderTexture->Bind();
    shader->SetTexture(textureName, renderTexture);
    shader->SetVector2f("resolution", resolution);
}

void Framebuffer::Clear() const
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Framebuffer::AttachTexture(Texture2D *renderTexture_, string textureName_)
{
    renderTexture = renderTexture_;
    textureName = textureName_;
}

int Framebuffer::Width() const
{
    return width;
}

int Framebuffer::Height() const
{
    return height;
}

Texture2D *Framebuffer::GetRenderTexture() const
{
    return renderTexture;
}

void Framebuffer::Init()
{
    glGenFramebuffers(1, &id);
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture->ID, 0);

    // TODO : make a test if we want depth testing to take place to this fbo
    glGenRenderbuffers(1, &depthId);
    glBindRenderbuffer(GL_RENDERBUFFER_EXT, depthId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthId);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

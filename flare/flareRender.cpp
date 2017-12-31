#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "flareRender.h"
#include "../resource_manager.h"

FlareRender::FlareRender(int width_, int height_, Camera *camera_) :
        width(width_),
        height(height_),
        camera(camera_)
{}

FlareRender::~FlareRender()
{
    delete writeFBO;
    delete readFBO;
    delete mainFBO;
    delete skyFBO;
    for (int i = 0; i < MAX_DOWNSCALE_FBO; ++i)
    {
        delete downScaleFBO[i];
    }
    delete skyBox;
    delete hdrTextureCube;
    delete triangle;
}

void FlareRender::Init()
{
    // Initialize sky box.
    skyBox = new SkyBox();

    // Initialize hdr cube map.
    hdrTextureCube = new HDRTextureCube("../textures/uffizi_cross.hdr");

    // Initialize a full-screen triangle.
    triangle = new ScreenAlignedTriangle();

    Texture2D *renderTexture;

    writeFBO = new Framebuffer(width, height);
    readFBO = new Framebuffer(width, height);
    mainFBO = new Framebuffer(width, height);
    skyFBO = new Framebuffer(width, height);

    for (int i = 0; i < MAX_DOWNSCALE_FBO; ++i)
    {
        downScaleFBO[i] = new Framebuffer(width, height);
        renderTexture = new Texture2D(width, height, GL_RGB16F_ARB, GL_RGBA);
        renderTexture->Generate(width, height, nullptr);
        downScaleFBO[i]->AttachTexture(renderTexture, "renderTexture");
        downScaleFBO[i]->Init();
    }

    renderTexture = new Texture2D(width, height, GL_RGB16F_ARB, GL_RGBA);
    renderTexture->Generate(width, height, nullptr);
    writeFBO->AttachTexture(renderTexture, "renderTexture");
    renderTexture = new Texture2D(width, height, GL_RGB16F_ARB, GL_RGBA);
    renderTexture->Generate(width, height, nullptr);
    readFBO->AttachTexture(renderTexture, "renderTexture");
    renderTexture = new Texture2D(width, height, GL_RGB16F_ARB, GL_RGBA);
    renderTexture->Generate(width, height, nullptr);
    mainFBO->AttachTexture(renderTexture, "renderTexture");
    renderTexture = new Texture2D(width, height, GL_RGB16F_ARB, GL_RGBA);
    renderTexture->Generate(width, height, nullptr);
    skyFBO->AttachTexture(renderTexture, "renderTexture");

    writeFBO->Init();
    readFBO->Init();
    mainFBO->Init();
    skyFBO->Init();
}

void FlareRender::Draw()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    skyFBO->Start();
    skyBox->Draw();
    skyFBO->End();

    mainFBO->Start();
    skyLightPass();
    mainFBO->End();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    lensFlarePass();
    capture();

    Shader defaultShader = ResourceManager::GetShader("default");
    defaultShader.Use();
    mainFBO->Bind(&defaultShader, glm::vec2(width, height), "renderTexture");
    triangle->Draw();

    glDisable(GL_BLEND);
}

void FlareRender::lensFlarePass()
{
    float downScale = 8.0f;

    brightPass(3.0);

    writeFBO->Start(1 / downScale);
    Shader lensflare = ResourceManager::GetShader("lensflare");
    lensflare.Use();
    readFBO->Bind(&lensflare, glm::vec2(width / downScale, height / downScale), "renderTexture");
    triangle->Draw();
    writeFBO->End();

    swapBuffers();

    writeFBO->Start(1 / downScale);
    Shader blur = ResourceManager::GetShader("blur");
    blur.Use();
    readFBO->Bind(&blur, glm::vec2(width / downScale, height / downScale), "renderTexture");
    triangle->Draw();
    writeFBO->End();

    swapBuffers();

    writeFBO->Start();
    Shader rgbshift = ResourceManager::GetShader("rgbshift");
    rgbshift.Use();
    readFBO->Bind(&rgbshift, glm::vec2(width, height), "renderTexture");
    triangle->Draw();
    writeFBO->End();

    swapBuffers();

    writeFBO->Start();
    Shader radialnoise = ResourceManager::GetShader("radialnoise");
    radialnoise.Use();
    readFBO->Bind(&radialnoise, glm::vec2(width, height), "renderTexture");
    triangle->Draw();
    writeFBO->End();

    swapBuffers();

    writeFBO->Start();
    Shader compose = ResourceManager::GetShader("compose");
    compose.Use();
    skyFBO->Bind(&compose, glm::vec2(width, height), "sampler2");
    readFBO->GetRenderTexture()->SetTextureIndex(1);
    readFBO->Bind(&compose, glm::vec2(width, height), "sampler1");
    triangle->Draw();
    writeFBO->End();

    swapBuffers();
    readFBO->GetRenderTexture()->SetTextureIndex(0);
    glActiveTexture(GL_TEXTURE0);
}

void FlareRender::capture()
{
    mainFBO->Start();
    Shader defaultShader = ResourceManager::GetShader("default");
    defaultShader.Use();
    readFBO->Bind(&defaultShader, glm::vec2(width, height), "renderTexture");
    triangle->Draw();
    mainFBO->End();
}

void FlareRender::brightPass(float threshold)
{
    writeFBO->Start();
    Shader brightpass = ResourceManager::GetShader("brightpass");
    brightpass.Use();
    mainFBO->Bind(&brightpass, glm::vec2(width, height), "renderTexture");
    brightpass.SetFloat("threshold", threshold);
    triangle->Draw();
    writeFBO->End();

    swapBuffers();
}

void FlareRender::skyLightPass()
{
    glm::mat4 model;
    glm::mat4 view = camera->GetViewMatrix();
    glm::mat4 projection = glm::perspective(camera->Zoom, (float) width / (float) height,
                                            camera->NearClippingPlaneDistance, camera->FarClippingPlaneDistance);

    glm::mat4 mvp = projection * view * model;

    Shader shader = ResourceManager::GetShader("cubemap");
    shader.Use();
    shader.SetMatrix4("mvp", mvp);
    shader.SetVector3f("camPosition", camera->GetViewPosition());
    hdrTextureCube->Draw();
}

void FlareRender::swapBuffers()
{
    Framebuffer *tmp = readFBO;
    readFBO = writeFBO;
    writeFBO = tmp;
}

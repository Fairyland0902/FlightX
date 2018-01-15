#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include "game.h"
#include "resource_manager.h"

extern bool keys[1024];

// RenderQuad() Renders a 1x1 quad in NDC, best used for framebuffer color targets
// and post-processing effects.
GLuint quadVAO = 0;
GLuint quadVBO;

void RenderQuad()
{
    if (quadVAO == 0)
    {
        GLfloat quadVertices[] = {
                // Positions        // Texture Coords
                -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // Setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *) 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *) (3 * sizeof(GLfloat)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

Game::Game() :
        camera(glm::vec3(0.0f, -195.0f, 0.0f))
{
    currentcamera = &camera;
}

Game::~Game()
{
    delete cloudRender;
    delete flareRender;
}

void Game::Init(int width, int height)
{
    // Build and compile our shader program.
    loadShaders();

    // Initialize depth mapping frame buffer.
    initDepthMap();

    // Initialize cloud renderer.
    cloudRender = new CloudRender(width, height);

    // Initialize flare render.
    flareRender = new FlareRender(width, height);
    flareRender->Init();

    // Initialize ocean.
    ocean = new Ocean(width, height);
    ocean->Init();

    terrain = new Terrain(width, height);
    terrain->init();
    asphalt = new Asphalt(width, height);
    asphalt->init();

    aircraft.Init();
    aircraft.loadModel(_MODEL_PREFIX_"/f16/f16.obj");
    aircraft.setAirspeed(glm::vec3(3.0, 0, 0));
}

void Game::Render(int width, int height, float deltaTime)
{
    // 1. Render depth of scene to texture (from light's perspective)
    // - Get light projection/view matrix.
    glm::mat4 lightProjection, lightView;
    glm::mat4 lightSpaceMatrix;
    lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);
    lightView = glm::lookAt(aircraft.Position + glm::vec3(0.0f, 100.0f, 0.0f), aircraft.Position,
                            glm::vec3(0.0, 0.0, 1.0));
//    std::cout << glm::to_string(aircraft.Position) << std::endl;
    lightSpaceMatrix = lightProjection * lightView;

    // - Render scene from light's point of view.
    Shader shadow = ResourceManager::GetShader("shadow");
    shadow.Use();
    shadow.SetMatrix4("lightSpaceMatrix", lightSpaceMatrix);

    // Down sample.
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
//    glCullFace(GL_FRONT);
    aircraft.DrawDepth(shadow);
//    glCullFace(GL_BACK);
    terrain->DrawDepth(shadow);
    asphalt->DrawDepth(shadow);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Reset viewport.
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 2. Render scene as normal.
    flareRender->Draw();

    terrain->Draw(depthMap, lightSpaceMatrix);
    asphalt->Draw(depthMap, lightSpaceMatrix);

    //    ocean->Draw(deltaTime);
    cloudRender->Draw(deltaTime);

    aircraft.Draw(ResourceManager::GetShader("aircraft"), depthMap, lightSpaceMatrix);
    aircraft.Update(deltaTime);
    //For Test:
    aircraft.DrawHUD();

    // 3. DEBUG: visualize depth map by rendering it to plane
//    Shader debug = ResourceManager::GetShader("debug");
//    debug.Use();
//    debug.SetInteger("depthMap", 0);
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, depthMap);
//    RenderQuad();
}

void Game::loadShaders()
{
    const GLchar *transformFeedbackVaryings[] = {"vs_out_position", "vs_out_size_time_rand", "vs_out_depthclipspace"};
    std::cout << "Loading shaders ............. ";

    ResourceManager::LoadShader(_SHADER_PREFIX_"/sky/cloudMove.vert",
                                "",
                                "",
                                "cloud move", transformFeedbackVaryings, 3, false);
    ResourceManager::LoadShader(_SHADER_PREFIX_"/sky/cloudPassThrough.vert",
                                _SHADER_PREFIX_"/sky/cloudFOM.frag",
                                _SHADER_PREFIX_"/sky/cloudFOM.geom",
                                "FOM");
    ResourceManager::LoadShader(_SHADER_PREFIX_"/sky/screenTri.vert",
                                _SHADER_PREFIX_"/sky/cloudFOMBlur.frag",
                                "",
                                "FOM filter");
    ResourceManager::LoadShader(_SHADER_PREFIX_"/sky/cloudPassThrough.vert",
                                _SHADER_PREFIX_"/sky/cloudRendering.frag",
                                _SHADER_PREFIX_"/sky/cloudRendering.geom",
                                "cloud render");
    ResourceManager::LoadShader(_SHADER_PREFIX_"/sky/screenTri.vert",
                                _SHADER_PREFIX_"/sky/skybox.frag", "",
                                "skybox");
    ResourceManager::LoadShader(_SHADER_PREFIX_"/hudline.vert",
                                _SHADER_PREFIX_"/hudline.frag",
                                "",
                                "hudline");
    ResourceManager::LoadShader(_SHADER_PREFIX_"/sky/screenTri.vert",
                                _SHADER_PREFIX_"/lens flare/brightpass.frag",
                                "",
                                "brightpass");
    ResourceManager::LoadShader(_SHADER_PREFIX_"/sky/screenTri.vert",
                                _SHADER_PREFIX_"/lens flare/lensflare.frag",
                                "",
                                "lensflare");
    ResourceManager::LoadShader(_SHADER_PREFIX_"/sky/screenTri.vert",
                                _SHADER_PREFIX_"/lens flare/blur.frag",
                                "",
                                "blur");
    ResourceManager::LoadShader(_SHADER_PREFIX_"/sky/screenTri.vert",
                                _SHADER_PREFIX_"/lens flare/rgbshift.frag",
                                "",
                                "rgbshift");
    ResourceManager::LoadShader(_SHADER_PREFIX_"/sky/screenTri.vert",
                                _SHADER_PREFIX_"/lens flare/radialnoise.frag",
                                "",
                                "radialnoise");
    ResourceManager::LoadShader(_SHADER_PREFIX_"/sky/screenTri.vert",
                                _SHADER_PREFIX_"/lens flare/compose.frag",
                                "",
                                "compose");
    ResourceManager::LoadShader(_SHADER_PREFIX_"/sky/screenTri.vert",
                                _SHADER_PREFIX_"/lens flare/default.frag",
                                "",
                                "default");
    ResourceManager::LoadShader(_SHADER_PREFIX_"/lens flare/cubemap.vert",
                                _SHADER_PREFIX_"/lens flare/cubemap.frag",
                                "",
                                "cubemap");
    ResourceManager::LoadShader(_SHADER_PREFIX_"/ocean/ocean.vert",
                                _SHADER_PREFIX_"/ocean/ocean.frag",
                                "",
                                "ocean");
    ResourceManager::LoadShader(_SHADER_PREFIX_"/aircraft.vert",
                                _SHADER_PREFIX_"/aircraft.frag",
                                "",
                                "aircraft");
    ResourceManager::LoadShader(_SHADER_PREFIX_"/plane.vert",
                                _SHADER_PREFIX_"/plane.frag",
                                "",
                                "plane");
    ResourceManager::LoadShader(_SHADER_PREFIX_"/PBR.vert",
                                _SHADER_PREFIX_ "/PBR.frag",
                                "",
                                "PBR");
    ResourceManager::LoadShader(_SHADER_PREFIX_"/shadowMapping.vert",
                                _SHADER_PREFIX_"/shadowMapping.frag",
                                "",
                                "shadow");
    ResourceManager::LoadShader(_SHADER_PREFIX_"/depthMap.vert",
                                _SHADER_PREFIX_"/depthMap.frag",
                                "",
                                "debug");

    std::cout << "Done" << std::endl;
}

void Game::CameraControl()
{
    if (currentcamera != &aircraft && keys[GLFW_KEY_1])
    {
        currentcamera = &aircraft;
    }
    if (currentcamera != &camera && keys[GLFW_KEY_2])
    {
        currentcamera = &camera;
    }
    if (currentcamera != aircraft.AroundCam && keys[GLFW_KEY_3])
    {
        currentcamera = aircraft.AroundCam;
    }
}

void Game::loadTextures()
{
//    ResourceManager::LoadTexture3D("../noisegen/noise3.ex5", "cloud");
}

void Game::initDepthMap()
{
    // Configure depth map FBO.
    glGenFramebuffers(1, &depthMapFBO);
    // Create depth texture.
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                 NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = {1.0, 1.0, 1.0, 1.0};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

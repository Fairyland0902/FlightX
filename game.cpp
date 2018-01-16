#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include "game.h"
#include "resource_manager.h"

extern bool keys[1024];
extern GLFWwindow *window;
// RenderQuad() Renders a 1x1 quad in NDC, best used for framebuffer color targets
// and post-processing effects.
GLuint quadVAO = 0;
GLuint quadVBO;
<<<<<<< HEAD
int scene = 0;
void RenderQuad()
{
    if (quadVAO == 0) {
=======

void RenderQuad()
{
    if (quadVAO == 0)
    {
>>>>>>> fded35362a8a7c137ed9f0d86ff7bfa6971db4d9
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
<<<<<<< HEAD
        camera(glm::vec3(0.0f, -195.0f, 0.0f)), paused(0), crashed(0) {
    currentcamera = &aircraft;
=======
        camera(glm::vec3(0.0f, -195.0f, 0.0f)), paused(0), crashed(0)
{
    currentcamera = &camera;
>>>>>>> fded35362a8a7c137ed9f0d86ff7bfa6971db4d9
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
    mounts = new Mounts(width, height);

    aircraft.Init();
    aircraft.loadModel(_MODEL_PREFIX_"/f16/f16.obj");
    aircraft.setAirspeed(glm::vec3(3.0, 0, 0));
}

int drawbb = 0;


void Game::getSamplePoint(std::vector<glm::vec3> &points)
{
    int sample_num = 5;
    float worldX = aircraft.Position.x;
    float worldZ = aircraft.Position.z;
    for (int i = -sample_num; i <= sample_num; i++)
    {
        for (int j = -sample_num; j <= sample_num; j++)
        {
            float x = worldX + 0.01f * i;
            float z = worldZ + 0.01f * j;
            float height = mounts->getHeight(x, z);
            std::cout << worldX << " " << height << " " << aircraft.Position.y << " " << worldZ << std::endl;
            points.emplace_back(x, height, z);
        }
    }
}

void Game::Render(int width, int height, float deltaTime)
{
    std::vector<glm::vec3> points;
    getSamplePoint(points);
    if (aircraft.Position.y < mounts->getHeight(aircraft.Position.x, aircraft.Position.z))
    {
        crashed = 1;
        paused = 3;
    }
    for (auto &point: points)
    {
        if (aircraft.detechCrash(point))
        {
            crashed = 1;
            paused = 3;
        }
    }
    if (paused || crashed)deltaTime = 0;
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

if(scene==2){
   terrain->Draw(depthMap, lightSpaceMatrix);
   asphalt->Draw(depthMap, lightSpaceMatrix);
}else if(scene==0)
    mounts->Draw();
<<<<<<< HEAD
else if(scene==1) 
ocean->Draw(deltaTime);
=======

//    ocean->Draw(deltaTime);
>>>>>>> fded35362a8a7c137ed9f0d86ff7bfa6971db4d9

    cloudRender->Draw(deltaTime);
    aircraft.Draw(ResourceManager::GetShader("aircraft"), depthMap, lightSpaceMatrix);
    if (drawbb)
        aircraft.DrawBoundingBox();
    if (!paused && !crashed)
        aircraft.Update(deltaTime);
//    printf("%d\n", paused);
    if (paused > 0)
        aircraft.DrawHUD(paused);
    if (currentcamera == &aircraft)
        aircraft.DrawHUD(0);
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
    ResourceManager::LoadShader(_SHADER_PREFIX_"/boundingbox.vert",
                                _SHADER_PREFIX_"/boundingbox.frag",
                                "",
                                "boundingbox");
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
    ResourceManager::LoadShader(_SHADER_PREFIX_"/flame/particle.vert",
                                _SHADER_PREFIX_"/flame/particle.frag",
                                _SHADER_PREFIX_"/flame/particle.geom",
                                "flame");

    ResourceManager::LoadShader(_SHADER_PREFIX_"/mount.vert",
                                _SHADER_PREFIX_ "/mount.frag",
                                "",
                                "mount");
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
    static bool keyPpressed = false;
    if (keys[GLFW_KEY_P])
    {
        if (!keyPpressed)
        {
            keyPpressed = true;
            if (paused < 2)paused = !paused;
        }
    } else keyPpressed = false;
    static bool keyBpressed = false;
    if (keys[GLFW_KEY_B])
    {
        if (!keyBpressed)
        {
            keyBpressed = true;
            drawbb = !drawbb;
        }
    } else keyBpressed = false;
    if (paused == 2)
    {
        if (keys[GLFW_KEY_Y])
        {
            paused = 0;
            aircraft.Position = glm::vec3(0, 0, 0);
            aircraft.WorldUp = glm::vec3(0, 1, 0);
            aircraft.Front = glm::vec3(1, 0, 0);
            aircraft.Up = glm::vec3(0, 1, 0);
            aircraft.setAirspeed(glm::vec3(4, 0, 0));
        } else if (keys[GLFW_KEY_N])
        {
            if (crashed)paused = 3;
            else paused = 0;

        }
    }
    if (paused == 4)
    {
        if (keys[GLFW_KEY_Y])
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        } else if (keys[GLFW_KEY_N])
        {
            if (crashed)paused = 3;
            else paused = 0;
        }
    }
    if (keys[GLFW_KEY_R])paused = 2;
    if (keys[GLFW_KEY_ESCAPE])paused = 4;
    if(keys[GLFW_KEY_0])
        scene = 0;
        if(keys[GLFW_KEY_9])
        scene = 1;
        if(keys[GLFW_KEY_8])
        scene = 2;
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

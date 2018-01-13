#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "game.h"
#include "resource_manager.h"

extern bool keys[1024];

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

    aircraft.loadModel(_MODEL_PREFIX_"/f16/f16.obj");
    aircraft.setAirspeed(glm::vec3(1.0, 0, 0));
}

void Game::Render(int width, int height, float deltaTime)
{
    flareRender->Draw();
    cloudRender->Draw(deltaTime);

    aircraft.Draw(ResourceManager::GetShader("aircraft"));
    aircraft.Update(deltaTime);

//    ocean->Draw(deltaTime);

    terrain->Draw();
    asphalt->Draw();
    ocean->Draw(deltaTime);

    //For Test:
    aircraft.DrawHUD();
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
}

void Game::loadTextures()
{
//    ResourceManager::LoadTexture3D("../noisegen/noise3.ex5", "cloud");
}

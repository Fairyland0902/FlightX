#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "game.h"
#include "resource_manager.h"

Game::Game() :
        camera(glm::vec3(0.0f, 50.0f, 0.0f))
{}

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
    cloudRender = new CloudRender(width, height, camera);

    // Initialize flare render.
    flareRender = new FlareRender(width, height, &camera);
    flareRender->Init();
}

void Game::Render(int width, int height, float deltaTime)
{
    flareRender->Draw();
    cloudRender->Draw(deltaTime);
}

void Game::loadShaders()
{
    const GLchar *transformFeedbackVaryings[] = {"vs_out_position", "vs_out_size_time_rand", "vs_out_depthclipspace"};
    std::cout << "Loading shaders ............. ";

    ResourceManager::LoadShader("../shaders/sky/cloudMove.vert", "", "", "cloud move", transformFeedbackVaryings, 3,
                                false);
    ResourceManager::LoadShader("../shaders/sky/cloudPassThrough.vert", "../shaders/sky/cloudFOM.frag",
                                "../shaders/sky/cloudFOM.geom", "FOM");
    ResourceManager::LoadShader("../shaders/sky/screenTri.vert", "../shaders/sky/cloudFOMBlur.frag", "", "FOM filter");
    ResourceManager::LoadShader("../shaders/sky/cloudPassThrough.vert", "../shaders/sky/cloudRendering.frag",
                                "../shaders/sky/cloudRendering.geom", "cloud render");
    ResourceManager::LoadShader("../shaders/sky/screenTri.vert", "../shaders/sky/skybox.frag", "", "skybox");
    ResourceManager::LoadShader("../shaders/sky/screenTri.vert", "../shaders/lens flare/brightpass.frag", "",
                                "brightpass");
    ResourceManager::LoadShader("../shaders/sky/screenTri.vert", "../shaders/lens flare/lensflare.frag", "",
                                "lensflare");
    ResourceManager::LoadShader("../shaders/sky/screenTri.vert", "../shaders/lens flare/blur.frag", "", "blur");
    ResourceManager::LoadShader("../shaders/sky/screenTri.vert", "../shaders/lens flare/rgbshift.frag", "",
                                "rgbshift");
    ResourceManager::LoadShader("../shaders/sky/screenTri.vert", "../shaders/lens flare/radialnoise.frag", "",
                                "radialnoise");
    ResourceManager::LoadShader("../shaders/sky/screenTri.vert", "../shaders/lens flare/compose.frag", "",
                                "compose");
    ResourceManager::LoadShader("../shaders/sky/screenTri.vert", "../shaders/lens flare/default.frag", "",
                                "default");
    ResourceManager::LoadShader("../shaders/lens flare/cubemap.vert", "../shaders/lens flare/cubemap.frag", "",
                                "cubemap");

    std::cout << "Done" << std::endl;
}


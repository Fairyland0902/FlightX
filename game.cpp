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
}

void Game::Init(int width, int height)
{
    // Build and compile our shader program.
    loadShaders();

    // Load textures.
    loadTextures();

    // Initialize cloud renderer.
    cloudRender = new CloudRender(width, height, camera);

    // Initialize sky box.
    skyBox = new SkyBox();
}

void Game::Render(int width, int height, float deltaTime)
{
    skyBox->Draw();
    cloudRender->Draw(deltaTime);
}

void Game::loadShaders()
{
    const GLchar *transformFeedbackVaryings[] = {"vs_out_position", "vs_out_size_time_rand", "vs_out_depthclipspace"};
    std::cout << "Loading shaders ............. ";

    ResourceManager::LoadShader("../shaders/cloudMove.vert", "", "", "cloud move", transformFeedbackVaryings, 3, false);
    ResourceManager::LoadShader("../shaders/cloudPassThrough.vert", "../shaders/cloudFOM.frag",
                                "../shaders/cloudFOM.geom", "FOM");
    ResourceManager::LoadShader("../shaders/screenTri.vert", "../shaders/cloudFOMBlur.frag", "", "FOM filter");
    ResourceManager::LoadShader("../shaders/cloudPassThrough.vert", "../shaders/cloudRendering.frag",
                                "../shaders/cloudRendering.geom", "cloud render");
    ResourceManager::LoadShader("../shaders/screenTri.vert", "../shaders/skybox.frag", "", "skybox");

    std::cout << "Done" << std::endl;
}

void Game::loadTextures()
{
//    ResourceManager::LoadTexture3D("../noisegen/noise3.ex5", "cloud");
}

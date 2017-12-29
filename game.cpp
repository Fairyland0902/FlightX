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
    delete skyBox;
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
	aircraft.loadModel("model/f16/f16.obj");
}

void Game::Render(int width, int height, float deltaTime)
{
    skyBox->Draw();
    cloudRender->Draw(deltaTime);
	aircraft.Draw();
}

void Game::loadShaders()
{
    const GLchar *transformFeedbackVaryings[] = {"vs_out_position", "vs_out_size_time_rand", "vs_out_depthclipspace"};
    std::cout << "Loading shaders ............. ";

    ResourceManager::LoadShader(_SHADER_PREFIX_"/cloudMove.vert", "", "", "cloud move", transformFeedbackVaryings, 3, false);
    ResourceManager::LoadShader(_SHADER_PREFIX_"/cloudPassThrough.vert", _SHADER_PREFIX_"/cloudFOM.frag",
		_SHADER_PREFIX_"/cloudFOM.geom", "FOM");
    ResourceManager::LoadShader(_SHADER_PREFIX_"/screenTri.vert", _SHADER_PREFIX_"/cloudFOMBlur.frag", "", "FOM filter");
    ResourceManager::LoadShader(_SHADER_PREFIX_"/cloudPassThrough.vert", _SHADER_PREFIX_"/cloudRendering.frag",
		_SHADER_PREFIX_"/cloudRendering.geom", "cloud render");
    ResourceManager::LoadShader(_SHADER_PREFIX_"/screenTri.vert", _SHADER_PREFIX_"/skybox.frag", "", "skybox");

    std::cout << "Done" << std::endl;
}

void Game::loadTextures()
{
//    ResourceManager::LoadTexture3D("../noisegen/noise3.ex5", "cloud");
}

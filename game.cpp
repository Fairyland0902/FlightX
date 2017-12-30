#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "game.h"
#include "resource_manager.h"
extern bool keys[1024];
Game::Game() :
	camera(glm::vec3(0.0f, 0.0f, 0.0f)) {
	currentcamera = &camera;
}

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
	aircraft.setAirspeed(glm::vec3(1.0, 0, 0));
}

void Game::Render(int width, int height, float deltaTime)
{
    //skyBox->Draw();
    cloudRender->Draw(deltaTime);
	aircraft.Draw(ResourceManager::GetShader("aircraft"));
	aircraft.Update(deltaTime);
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
	ResourceManager::LoadShader(_SHADER_PREFIX_"/aircraft.vert", _SHADER_PREFIX_"/aircraft.frag", "", "aircraft");
    std::cout << "Done" << std::endl;
}

void Game::CameraControl() {
	if (currentcamera != &aircraft&&keys[GLFW_KEY_1]) {
		currentcamera = &aircraft;
		cloudRender->ChangeCamera(*currentcamera);
	}
	if (currentcamera != &camera&&keys[GLFW_KEY_2]) {
		currentcamera = &camera;
		cloudRender->ChangeCamera(*currentcamera);
	}
}

void Game::loadTextures()
{
//    ResourceManager::LoadTexture3D("../noisegen/noise3.ex5", "cloud");
}

#include "game.h"
#include "resource_manager.h"

Game::Game() :
        camera(glm::vec3(0.0f, 0.0f, 10.0f))
{}

void Game::Init()
{
    // Build and compile our shader program.
    loadShaders();

    // Initialize the components used for cloud rendering.
    cloudData = std::unique_ptr<CloudData>(new CloudData(128, 128, 128));
    cloudSim = std::unique_ptr<CloudSimulator>(new CloudSimulator(128, 128, 128));
    cloudRender = std::unique_ptr<CloudRender>(new CloudRender(128, 128, 128));
}

void Game::Render(int width, int height, float startTime)
{
    Shader cloudShader = ResourceManager::GetShader("cloud");
    cloudShader.Use();

    // Draw scene, create transformations.
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(camera.Zoom, (float) width / (float) height,
                                            camera.NearClippingPlaneDistance, camera.FarClippingPlaneDistance);
    cloudShader.SetMatrix4("view", view);
    cloudShader.SetMatrix4("viewInverse", glm::inverse(view));
    cloudShader.SetMatrix4("projection", projection);
    cloudShader.SetFloat("tanFOV", glm::radians(camera.Zoom / 2.0f));
    cloudShader.SetVector2f("screenSize", glm::vec2(width, height));
    cloudShader.SetVector3f("eyePosition", camera.GetViewPosition());
    cloudShader.SetFloat("near", camera.NearClippingPlaneDistance);
    cloudShader.SetFloat("far", camera.FarClippingPlaneDistance);

    cloudSim->Simulate(*cloudData.get(), startTime);
    cloudRender->Render(*cloudData.get(), cloudShader, startTime);
}

void Game::loadShaders()
{
    ResourceManager::LoadShader("../shaders/cloud.vert", "../shaders/cloud.frag", nullptr, "cloud");
}

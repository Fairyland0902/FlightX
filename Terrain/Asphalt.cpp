//
// Created by longmensparkle on 2018/1/14.
//

#include <resource_manager.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Asphalt.h"

Asphalt::Asphalt(int width, int height) : Terrain(width, height)
{
    chunk_width = 128;
    chunk_height = 128;
    absolute_height = -199.9;

    // Load PBR textures.
    Albedo = ResourceManager::LoadTexture2D(_TEXTURE_PREFIX_"/dry-dirt/dry-dirt1-albedo.png", true, "dirtAlbedo");
    Normal = ResourceManager::LoadTexture2D(_TEXTURE_PREFIX_"/dry-dirt/dry-dirt1-normal2.png", true, "dirtNormal");
    Metallic = ResourceManager::LoadTexture2D(_TEXTURE_PREFIX_"/dry-dirt/dry-dirt1-metalness.png", true,
                                              "dirtMetallic");
    Roughness = ResourceManager::LoadTexture2D(_TEXTURE_PREFIX_"/dry-dirt/dry-dirt1-roughness.png", true,
                                               "dirtRoughness");
    AO = ResourceManager::LoadTexture2D(_TEXTURE_PREFIX_"/dry-dirt/dry-dirt1-ao.png", true, "dirtAO");

    paint = new Paint(width, height);
    paint->init();
}

void Asphalt::Draw()
{
    setShader();
    glm::mat4 view = currentcamera->GetViewMatrix();
    glm::mat4 projection = glm::perspective(currentcamera->Zoom, (float) width / (float) height,
                                            currentcamera->NearClippingPlaneDistance,
                                            currentcamera->FarClippingPlaneDistance);
    shader.SetMatrix4("view", view);
    shader.SetMatrix4("projection", projection);
    // Uniform variables setting.
    shader.SetVector3f("camPos", currentcamera->GetViewPosition());
    shader.SetVector3f("lightDirection", 0.0f, 1.0f, 0.0f);
    shader.SetVector3f("lightColor", 1.0f, 1.0f, 1.0f);

    // Activate PBR textures.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Albedo.ID);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, Normal.ID);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, Metallic.ID);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, Roughness.ID);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, AO.ID);

    glm::mat4 trans;
    trans = glm::translate(trans, glm::vec3(0, 0, -256 - 64));
    for (int i = 0; i < 4; i++)
    {
        trans = glm::translate(trans, glm::vec3(0, 0, 128));
        shader.SetMatrix4("model", trans);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        paint->setMVP(trans, view, projection);
        paint->Draw();
    }

    // Reset activated texture to default.
    glActiveTexture(GL_TEXTURE0);
}

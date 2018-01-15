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
    cross = new Cross(width, height);
    cross->init();
}

void Asphalt::Draw(GLuint shadowMap, glm::mat4 &lightSpaceMatrix)
{
//    setShader();
    glm::mat4 view = currentcamera->GetViewMatrix();
    glm::mat4 projection = glm::perspective(currentcamera->Zoom, (float) width / (float) height,
                                            currentcamera->NearClippingPlaneDistance,
                                            currentcamera->FarClippingPlaneDistance);
//    shader.SetMatrix4("view", view);
//    shader.SetMatrix4("projection", projection);
//    // Uniform variables setting.
//    shader.SetVector3f("camPos", currentcamera->GetViewPosition());
//    shader.SetVector3f("lightDirection", 0.0f, 1.0f, 0.0f);
//    shader.SetVector3f("lightColor", 1.0f, 1.0f, 1.0f);

//    // Activate PBR textures.
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, Albedo.ID);
//    glActiveTexture(GL_TEXTURE1);
//    glBindTexture(GL_TEXTURE_2D, Normal.ID);
//    glActiveTexture(GL_TEXTURE2);
//    glBindTexture(GL_TEXTURE_2D, Metallic.ID);
//    glActiveTexture(GL_TEXTURE3);
//    glBindTexture(GL_TEXTURE_2D, Roughness.ID);
//    glActiveTexture(GL_TEXTURE4);
//    glBindTexture(GL_TEXTURE_2D, AO.ID);

    glm::mat4 trans;
    trans = glm::translate(trans, glm::vec3(0, 0, -256 - 64));
    for (int i = 0; i < 4; i++)
    {
        setShader();
        trans = glm::translate(trans, glm::vec3(0, 0, 128));
        shader.SetMatrix4("model", trans);
        shader.SetMatrix4("view", view);
        shader.SetMatrix4("projection", projection);
        shader.SetMatrix4("lightSpaceMatrix", lightSpaceMatrix);
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
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, shadowMap);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

//        paint->setShader();
        paint->setMVP(trans, view, projection);
        paint->Draw(shadowMap, lightSpaceMatrix);
//        cross->setShader();
        cross->setMVP(trans, view, projection);
        cross->Draw(shadowMap, lightSpaceMatrix);
    }

    // Reset activated texture to default.
    glActiveTexture(GL_TEXTURE0);
}

void Asphalt::DrawDepth(Shader &shader)
{
    glm::mat4 trans;
    trans = glm::translate(trans, glm::vec3(0, 0, -256 - 64));
    for (int i = 0; i < 4; i++)
    {
        trans = glm::translate(trans, glm::vec3(0, 0, 128));
        shader.SetMatrix4("model", trans);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        paint->DrawDepth(shader);
        cross->DrawDepth(shader);
    }
}

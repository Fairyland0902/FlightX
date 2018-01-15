//
// Created by qweer29 on 1/12/18.
//
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <resource_manager.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "Terrain.h"

Terrain::Terrain(int width, int height) : AbstractTerrain(width, height) {
    Albedo = ResourceManager::LoadTexture2D(_TEXTURE_PREFIX_"/grass/grass1-albedo3.png", true,
                                            "grassAlbedo");
    Normal = ResourceManager::LoadTexture2D(_TEXTURE_PREFIX_"/grass/grass1-albedo3.png", true,
                                            "grassAlbedo");
    Metallic = ResourceManager::LoadTexture2D(_TEXTURE_PREFIX_"/grass/grass1-height.png", true,
                                              "grassMetallic");
    Roughness = ResourceManager::LoadTexture2D(_TEXTURE_PREFIX_"/grass/grass1-rough.png", true,
                                               "grassRoughness");
    AO = ResourceManager::LoadTexture2D(_TEXTURE_PREFIX_"/grass/grass1-ao.png", true, "grassAO");

    chunk_width = 512;
    chunk_height = 512;
    absolute_height = -200;
}

void Terrain::Draw() {
    setShader();
    setMVP();

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

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Reset activated texture to default.
    glActiveTexture(GL_TEXTURE0);
}

void Terrain::setShader() {
    shader = ResourceManager::GetShader("PBR");
    shader.Use();

    shader.SetInteger("albedoMap", 0);
    shader.SetInteger("normalMap", 1);
    shader.SetInteger("metallicMap", 2);
    shader.SetInteger("roughnessMap", 3);
    shader.SetInteger("aoMap", 4);
}



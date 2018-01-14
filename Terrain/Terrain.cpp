//
// Created by qweer29 on 1/12/18.
//
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <resource_manager.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "Terrain.h"

void Terrain::generateCoord(std::vector<glm::vec3> &vertices, std::vector<glm::vec2> &uvs, std::vector<GLuint> &indices)
{

    const float slice = 1.0 / (float) (n - 1);

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
        {
            float height = absolute_height;
            std::cout << i << ' ' << j << std::endl;
            vertices.emplace_back(i * slice * (chunk_width) - chunk_width / 2, height,
                                  j * slice * (chunk_height) - chunk_height / 2);
            uvs.emplace_back(float(i) / n * 40, float(j) / n * 40);
            if (i < n - 1 && j < n - 1)
            {
                indices.emplace_back(i * n + j);
                indices.emplace_back(i * n + n + j);
                indices.emplace_back(i * n + n + j + 1);

                indices.emplace_back(i * n + j);
                indices.emplace_back(i * n + 1 + j);
                indices.emplace_back(i * n + n + j + 1);
            }
        }
}

Terrain::Terrain(int width, int height) : width(width), height(height)
{
//    std::cout << heightMap.channels() << std::endl;
    // Load PBR material textures.
    Albedo = ResourceManager::LoadTexture2D(_TEXTURE_PREFIX_"/grass/grass1-albedo3.png", true, "grassAlbedo");
    Normal = ResourceManager::LoadTexture2D(_TEXTURE_PREFIX_"/grass/grass1-normal2.png", true, "grassNormal");
    Metallic = ResourceManager::LoadTexture2D(_TEXTURE_PREFIX_"/grass/grass1-height.png", true, "grassMetallic");
    Roughness = ResourceManager::LoadTexture2D(_TEXTURE_PREFIX_"/grass/grass1-rough.png", true, "grassRoughness");
    AO = ResourceManager::LoadTexture2D(_TEXTURE_PREFIX_"/grass/grass1-ao.png", true, "grassAO");
}

void Terrain::setMVP()
{
    glm::mat4 trans;
    glm::mat4 view = currentcamera->GetViewMatrix();
	glm::mat4 projection = currentcamera->GetProjectionMatrix();
    shader.SetMatrix4("model", trans);
    shader.SetMatrix4("view", view);
    shader.SetMatrix4("projection", projection);
}


void Terrain::Draw()
{
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

void Terrain::setShader()
{
    shader = ResourceManager::GetShader("PBR");
    shader.Use();

    shader.SetInteger("albedoMap", 0);
    shader.SetInteger("normalMap", 1);
    shader.SetInteger("metallicMap", 2);
    shader.SetInteger("roughnessMap", 3);
    shader.SetInteger("aoMap", 4);
}

void Terrain::init()
{
    generateCoord(vertices, uvs, indices);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Set the objects we need in the rendering process (namely, VAO, VBO and EBO).
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &UV);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const GLvoid *) 0);
    glEnableVertexAttribArray(0);
    // TexCoords attribute.
    glBindBuffer(GL_ARRAY_BUFFER, UV);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * uvs.size(), &uvs[0], GL_STATIC_DRAW);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (const GLvoid *) 0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}


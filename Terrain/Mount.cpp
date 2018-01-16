//
// Created by longmensparkle on 2018/1/14.
//

#include <resource_manager.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "Mount.h"

const int Mount::n = NUM_N;
const int Mount::chunk_width = 512;
const int Mount::chunk_height = 512;
const float Mount::absolute_height = -200;
const float Mount::mesh_width = float(chunk_width) / n;
const float Mount::mesh_height = float(chunk_height) / n;


void Mount::generateCoord(std::vector<glm::vec3> &vertices, std::vector<glm::vec2> &uvs, std::vector<GLuint> &indices)
{
    const float slice = 1.0 / (float) (n - 1);

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
        {
            int i_offset = (x_offset) / (chunk_width - 1) * (n - 1);
            int j_offset = (z_offset) / (chunk_height - 1) * (n - 1);

            float height = absolute_height + generator->generateHeight(i_offset + i, j_offset + j);
            heights[i][j] = height;
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

void Mount::setShader()
{
    shader = ResourceManager::GetShader("mount");
    shader.Use();
    shader.SetInteger("lowTex", 0);
    shader.SetInteger("highTex", 3);
    shader.SetInteger("middleTex", 4);
    shader.SetInteger("baseTex", 5);

    // Light setting.
    shader.SetVector3f("dirLight.direction", glm::vec3(0.0f, -1.0f, 0.0f));
    shader.SetVector3f("dirLight.ambient", glm::vec3(1.0f));
    shader.SetVector3f("dirLight.diffuse", glm::vec3(1.0f));
    shader.SetVector3f("dirLight.specular", glm::vec3(1.0f));

    // Material setting.
    shader.SetVector3f("magterial.ambient", glm::vec3(0.2f, 0.6f, 0.8f));
    shader.SetVector3f("material.diffuse", glm::vec3(0.2f, 0.6f, 0.8f));
    shader.SetVector3f("material.specular", glm::vec3(0.0f));
    shader.SetFloat("material.shininess", 16.0f);
}

void Mount::Draw()
{
    setShader();
    setMVP();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, lowTexture.ID);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, highTexture.ID);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, middleTexture.ID);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, baseTexture.ID);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Reset activated texture to default.
    glActiveTexture(GL_TEXTURE0);

//    tree->Draw();

}

Mount::Mount(int width, int height, int id, int x_offset, int z_offset, HeightGenerator *generator) :
        AbstractTerrain(width, height),
        id(id),
        x_offset(x_offset),
        z_offset(z_offset),
        generator(generator)
{
    highTexture = ResourceManager::LoadTexture2D(_TEXTURE_PREFIX_"/snow.jpg", true, "mountSnow");
    lowTexture = ResourceManager::LoadTexture2D(_TEXTURE_PREFIX_"/sand.tga", true, "mountDirt");
    middleTexture = ResourceManager::LoadTexture2D(_TEXTURE_PREFIX_"/grass/grass1-albedo3.png", true, "mountGrass");
    baseTexture = ResourceManager::LoadTexture2D(_TEXTURE_PREFIX_"/rock.tga", true, "mountRock");

}

void Mount::setMVP()
{
    glm::mat4 trans;
    trans = glm::translate(trans, glm::vec3(x_offset, 0, z_offset));
    glm::mat4 view = currentcamera->GetViewMatrix();
    glm::mat4 projection = currentcamera->GetProjectionMatrix();
    shader.SetMatrix4("model", trans);
    shader.SetMatrix4("view", view);
    shader.SetMatrix4("projection", projection);
    shader.SetVector3f("viewPos", currentcamera->GetViewPosition());
}





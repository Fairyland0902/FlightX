//
// Created by longmensparkle on 2018/1/14.
//

#include <resource_manager.h>
#include <iostream>
#include "Mount.h"

Mount::Mount(int width, int height) : Terrain(width, height) {
    chunk_width = 512;
    chunk_height = 512;
    absolute_height = -200;
    highTexture = ResourceManager::LoadTexture2D(_TEXTURE_PREFIX_"/asphalt2.jpg", true, "mountAsphalt2");
    lowTexture = ResourceManager::LoadTexture2D(_TEXTURE_PREFIX_"/grass2.jpg", true, "mountGrass2");
}

void Mount::generateCoord(std::vector<glm::vec3> &vertices, std::vector<glm::vec2> &uvs, std::vector<GLuint> &indices) {
    const float slice = 1.0 / (float) (n - 1);

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
        {
            float height = absolute_height + generator.generateHeight(i, j);
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

void Mount::setShader() {
    shader = ResourceManager::GetShader("mount");
    shader.Use();

    shader.SetInteger("lowTex", 0);
    shader.SetInteger("highTex", 1);
}

void Mount::Draw() {
    setShader();
    setMVP();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, lowTexture.ID);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, highTexture.ID);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Reset activated texture to default.
    glActiveTexture(GL_TEXTURE0);

}




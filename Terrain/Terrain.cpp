//
// Created by qweer29 on 1/12/18.
//
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <resource_manager.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "Terrain.h"

void Terrain::generateCoord(std::vector<glm::vec3> &vertices, std::vector<glm::vec2> &uvs, std::vector<GLuint> &indices) {

    const float slice = 1.0 / (float) (n - 1);

    for (int  i = 0; i < n; i++)
        for (int j = 0; j < n; j++){
            vertices.emplace_back(i * slice * (2000) - 1000, -200, j * slice * (2000) - 1000);
            uvs.emplace_back(i % 2,j % 2);
            if (i < n - 1 && j < n - 1) {
                indices.emplace_back(i * n + j);
                indices.emplace_back(i * n + n + j);
                indices.emplace_back(i * n + n + j + 1);

                indices.emplace_back(i * n + j);
                indices.emplace_back(i * n + 1 + j);
                indices.emplace_back(i * n + n + j + 1);
            }
        }
}

Terrain::Terrain(int width, int height) : width(width), height(height) {
    texture = ResourceManager::LoadTexture2D(_TEXTURE_PREFIX_"/grass.jpg", 0, "grass");

    generateCoord(vertices, uvs, indices);
//
//    for (glm::vec3 i : vertices) {
//        std::cout << i.x << ' ' << i.y << ' ' << i.z << std::endl;
//    }
//
//    for (glm::vec2 i : uvs) {
//        std::cout << i.x << ' ' << i.y  << std::endl;
//    }
//    for (GLushort i : indices) {
//        std::cout << i << std::endl;
//    }
}

void Terrain::setMVP() {
    glm::mat4 trans;
    glm::mat4 view = currentcamera->GetViewMatrix();
    glm::mat4 projection = glm::perspective(currentcamera->Zoom, (float) width / (float) height,
                                            currentcamera->NearClippingPlaneDistance,
                                            currentcamera->FarClippingPlaneDistance);
    shader.SetMatrix4("model", trans);
    shader.SetMatrix4("view", view);
    shader.SetMatrix4("projection", projection);
}



void Terrain::Draw() {
    setShader();
    setMVP();

    // Set the objects we need in the rendering process (namely, VAO, VBO and EBO).
    GLuint UV, VBO, EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &UV);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//    std::cout << sizeof(indices) << " " << sizeof(unsigned short) * indices2.size();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindTexture(GL_TEXTURE_2D, texture.ID);
    glUniform1i(glGetUniformLocation(shader.ID, "grassTexture"), 0);


    // Position attribute.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const GLvoid *) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, UV);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * uvs.size(), &uvs[0], GL_STATIC_DRAW);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (const GLvoid *) 0);
    glEnableVertexAttribArray(1);


    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void Terrain::setShader() {
    shader = ResourceManager::GetShader("plane");
    shader.Use();
}



//
// Created by qweer29 on 1/12/18.
//
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <resource_manager.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Terrain.h"

Terrain::Terrain(int width, int height): width(width), height(height) {}

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
    GLfloat vertices[] = {
            1000.0f, -500.0f, 1000.0f,
            1000.0f, -500.0f, -1000.0f,
            -1000.0f, -500.0f, -1000.0f,
            -1000.0f, -500.0f, 1000.0f
    };

    GLuint indices[] = {
            0, 1, 3,
            1, 2, 3
    };

    // Set the objects we need in the rendering process (namely, VAO, VBO and EBO).
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const GLvoid *) 0);
    glEnableVertexAttribArray(0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Unbind VAO.
    glBindVertexArray(0);
}

void Terrain::setShader() {
    shader = ResourceManager::GetShader("plane");
    shader.Use();
}

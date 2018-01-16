//
// Created by qweer29 on 1/16/18.
//

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "resource_manager.h"
#include "Trees.h"


void Trees::Draw() {
    setShader();
    setMVP();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, normal_tree.ID);

    glBindVertexArray(VAO);
//    std::cout << "fuck" << " " << normal_tree.ID << std::endl;
    angle = 0;
    shader.SetFloat("angle", angle);
    glDrawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_INT, 0);
//
//    angle = 90;
//    shader.SetFloat("angle", angle);
//    glDrawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Reset activated texture to default.
    glActiveTexture(GL_TEXTURE0);


}

Trees::Trees(int width, int height) : AbstractTerrain(width, height) {
    reed_tree = ResourceManager::LoadTexture2D(_TEXTURE_PREFIX_"/reed_texture.tga", true,
            "reed");
    normal_tree = ResourceManager::LoadTexture2D(_TEXTURE_PREFIX_"/tree_texture.tga", true,
                                                 "tree_texture");
    snowy_tree = ResourceManager::LoadTexture2D(_TEXTURE_PREFIX_"/snowy_tree_texture.tga", true,
                                                 "snowy_tree");


}

void Trees::setShader() {
    shader = ResourceManager::GetShader("tree");
    shader.Use();

    shader.SetFloat("angle", angle);
    shader.SetFloat("clipping", clipping);
    shader.SetFloat("height", height);
    shader.SetFloat("tree_height", tree_height);
    shader.SetInteger("tree_tex", 0);
}

void Trees::generateCoord(std::vector<glm::vec2> &vertices, std::vector<glm::vec2> &uvs, std::vector<GLuint> &indices) {
    vertices.emplace_back(-height, -height);
    vertices.emplace_back(-height, height);
    vertices.emplace_back(height, -height);
    vertices.emplace_back(height, height);

    indices.emplace_back(0);
    indices.emplace_back(1);
    indices.emplace_back(2);
    indices.emplace_back(3);

}

void Trees::init() {
    generateCoord(vertices, uvs, indices);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Set the objects we need in the rendering process (namely, VAO, VBO and EBO).
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute.
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (const GLvoid *) 0);
    glEnableVertexAttribArray(0);

//    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (const GLvoid *) 0);
//    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Trees::setMVP() {
    glm::mat4 trans = glm::scale(glm::mat4(1), glm::vec3(100, 100, 100));
    glm::mat4 view = currentcamera->GetViewMatrix();
    glm::mat4 projection = currentcamera->GetProjectionMatrix();
    shader.SetMatrix4("model", trans);
    shader.SetMatrix4("view", view);
    shader.SetMatrix4("projection", projection);
}

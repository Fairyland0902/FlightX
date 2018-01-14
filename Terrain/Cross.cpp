//
// Created by longmensparkle on 2018/1/14.
//

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "resource_manager.h"
#include "Cross.h"

Cross::Cross(int width, int height) : Paint(width, height) {
    chunk_width = 8;
    chunk_height = 36;
    absolute_height = -199.8;
    texture = ResourceManager::LoadTexture2D(_TEXTURE_PREFIX_"/asphalt3.jpg", true, "asphalt3");
}

void Cross::Draw() {
    trans = glm::translate(trans, glm::vec3(-56, 0, -40));
    for (int i = 0; i < 8; i++) {
        setShader();
        shader.SetMatrix4("model", trans);
        shader.SetMatrix4("view", view);
        shader.SetMatrix4("projection", projection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture.ID);
        glUniform1i(glGetUniformLocation(shader.ID, "planeTexture"), 0);
//        std::cout <<  texture.ID << std::endl;

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        trans = glm::translate(trans, glm::vec3(16, 0, 0));
    }
}

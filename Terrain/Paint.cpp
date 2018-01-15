//
// Created by longmensparkle on 2018/1/14.
//

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "resource_manager.h"
#include "Paint.h"


Paint::Paint(int width, int height) : AbstractTerrain(width, height)
{
    chunk_width = 4;
    chunk_height = 32;
    absolute_height = -199.8;
    texture = ResourceManager::LoadTexture2D(_TEXTURE_PREFIX_"/asphalt2.jpg", true, "asphalt2");
}

void Paint::setMVP(glm::mat4 trans, glm::mat4 view, glm::mat4 projection)
{
    this->trans = trans;
    this->view = view;
    this->projection = projection;
}

void Paint::Draw()
{
    glm::translate(trans, glm::vec3(0, 0, -16));
    for (int i = 0; i < 2; i++)
    {
        setShader();
        shader.SetMatrix4("model", trans);
        shader.SetMatrix4("view", view);
        shader.SetMatrix4("projection", projection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture.ID);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        trans = glm::translate(trans, glm::vec3(0, 0, 48));
    }
}

void Paint::setShader() {
    shader = ResourceManager::GetShader("plane");
    shader.Use();

    shader.SetInteger("planeTexture", 0);
}

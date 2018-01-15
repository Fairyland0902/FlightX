//
// Created by longmensparkle on 2018/1/14.
//

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "resource_manager.h"
#include "Cross.h"

Cross::Cross(int width, int height) : Paint(width, height)
{
    chunk_width = 8;
    chunk_height = 36;
    absolute_height = -199.8;
    texture = ResourceManager::LoadTexture2D(_TEXTURE_PREFIX_"/asphalt3.jpg", true, "asphalt3");
}

void Cross::Draw(GLuint shadowMap, glm::mat4 &lightSpaceMatrix)
{
    trans = glm::translate(trans, glm::vec3(-56, 0, -40));
    for (int i = 0; i < 8; i++)
    {
        setShader();
        shader.SetMatrix4("model", trans);
        shader.SetMatrix4("view", view);
        shader.SetMatrix4("projection", projection);
        shader.SetMatrix4("lightSpaceMatrix", lightSpaceMatrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture.ID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, shadowMap);
//        std::cout <<  texture.ID << std::endl;

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        trans = glm::translate(trans, glm::vec3(16, 0, 0));
    }
    glActiveTexture(GL_TEXTURE0);
}

void Cross::DrawDepth(Shader &shader)
{
    trans = glm::translate(trans, glm::vec3(-56, 0, -40));
    for (int i = 0; i < 8; i++)
    {
        shader.SetMatrix4("model", trans);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        trans = glm::translate(trans, glm::vec3(16, 0, 0));
    }
}

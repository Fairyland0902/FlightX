//
// Created by longmensparkle on 2018/1/14.
//

#include <resource_manager.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Asphalt.h"

Asphalt::Asphalt(int width, int height) : Terrain(width, height) {
    chunk_width = 128;
    chunk_height = 128;
    absolute_height = -199.9;
    texture = ResourceManager::LoadTexture2D(_TEXTURE_PREFIX_"/asphalt.jpg", 0, "asphalt");
    paint = new Paint(width, height);
    paint->init();
}

void Asphalt::Draw() {
    setShader();
    glm::mat4 trans;
    trans = glm::translate(trans, glm::vec3(0, 0, -256-64));
    for (int i = 0; i < 4; i++) {
        trans = glm::translate(trans, glm::vec3(0, 0, 128));
        glm::mat4 view = currentcamera->GetViewMatrix();
        glm::mat4 projection = glm::perspective(currentcamera->Zoom, (float) width / (float) height,
                                                currentcamera->NearClippingPlaneDistance,
                                                currentcamera->FarClippingPlaneDistance);
        shader.SetMatrix4("model", trans);
        shader.SetMatrix4("view", view);
        shader.SetMatrix4("projection", projection);
        glBindTexture(GL_TEXTURE_2D, texture.ID);
        glUniform1i(glGetUniformLocation(shader.ID, "planeTexture"), 0);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        paint->setMVP(trans, view, projection);
        paint->Draw();
    }
}

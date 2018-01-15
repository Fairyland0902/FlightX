//
// Created by qweer29 on 1/15/18.
//

#ifndef FLIGHTX_ABSTRACTTERRAIN_H
#define FLIGHTX_ABSTRACTTERRAIN_H


#include <glm/vec2.hpp>
#include <GL/glew.h>
#include <vector>
#include <shader.h>
#include <camera.h>

extern Camera *currentcamera;

class AbstractTerrain {
public:
    AbstractTerrain(int width, int height);

    virtual void init();

protected:
    virtual void setMVP();

    virtual void setShader() = 0;

    virtual void
    generateCoord(std::vector<glm::vec3> &vertices, std::vector<glm::vec2> &uvs, std::vector<GLuint> &indices);

    GLuint VAO;
    GLuint UV, VBO, EBO;
    int width;
    int height;
    int chunk_width = 512;
    int chunk_height = 512;
    float absolute_height = -200;
    Shader shader;
    std::vector<glm::vec3> vertices;
    std::vector<GLuint> indices;
    std::vector<glm::vec2> uvs;
    int n = 128;

    float MAX_HEIGHT = 20;
};


#endif //FLIGHTX_ABSTRACTTERRAIN_H

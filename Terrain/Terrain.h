//
// Created by qweer29 on 1/12/18.
//

#ifndef FLIGHTX_TERRAIN_H
#define FLIGHTX_TERRAIN_H

#include <camera.h>
#include <shader.h>

extern Camera *currentcamera;

class Terrain
{
public:
    Terrain(int width, int height);

    void Draw();

private:
    void setMVP();

    void setShader();

    void generateCoord(std::vector<glm::vec3> &vertices, std::vector<glm::vec2> &uvs, std::vector<GLuint> &indices);

    Texture2D texture;
	GLuint VAO;
    int width;
    int height;
    Shader shader;
    std::vector<glm::vec3> vertices;
    std::vector<GLuint> indices;
    std::vector<glm::vec2> uvs;
    int n = 100;
};


#endif //FLIGHTX_TERRAIN_H

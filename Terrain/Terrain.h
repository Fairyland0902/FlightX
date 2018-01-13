//
// Created by qweer29 on 1/12/18.
//

#ifndef FLIGHTX_TERRAIN_H
#define FLIGHTX_TERRAIN_H

#include "camera.h"
#include <shader.h>
#include <vector>

extern Camera *currentcamera;

class Terrain {
public:
    Terrain(int width, int height);

	virtual void Draw();

	virtual void init();

protected:
	virtual void setMVP();

	virtual void setShader();

	virtual void generateCoord(std::vector<glm::vec3> &vertices, std::vector<glm::vec2> &uvs, std::vector<GLuint> &indices);


    Texture2D texture;

    int width;
    int height;
    int chunk_width = 512;
    int chunk_height = 512;
	float absolute_height = -200;
    Shader shader;
    std::vector<glm::vec3> vertices;
    std::vector<GLuint> indices;
    std::vector<glm::vec2> uvs;
    int n = 10;
};


#endif //FLIGHTX_TERRAIN_H

//
// Created by longmensparkle on 2018/1/14.
//

#ifndef FLIGHTX_MOUNT_H
#define FLIGHTX_MOUNT_H


#include "HeightGenerator.h"
#include "Paint.h"

class Mount: public Terrain {
public:
    Mount(int width, int height);
    Texture2D highTexture, lowTexture;
    HeightGenerator generator;

    void Draw() override ;
protected:
    void generateCoord(std::vector<glm::vec3> &vertices, std::vector<glm::vec2> &uvs, std::vector<GLuint> &indices) override;

    void setShader() override ;
};


#endif //FLIGHTX_GRASS_H

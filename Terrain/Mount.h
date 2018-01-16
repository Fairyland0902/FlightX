//
// Created by longmensparkle on 2018/1/14.
//

#ifndef FLIGHTX_MOUNT_H
#define FLIGHTX_MOUNT_H


#include "HeightGenerator.h"
#include "Paint.h"

#define NUM_N 128

class Mount final : public AbstractTerrain
{
public:
    Mount(int width, int height, int id, int x_offset, int z_offset, HeightGenerator *generator);

    Texture2D highTexture, lowTexture, middleTexture, baseTexture;

    HeightGenerator *generator;

    void Draw();

    static const int n;
    static const int chunk_width;
    static const int chunk_height;
    static const float absolute_height;
    static const float mesh_width;
    static const float mesh_height;

    int id;

    int x_offset, z_offset;

    float heights[NUM_N][NUM_N];

protected:
    void
    generateCoord(std::vector<glm::vec3> &vertices, std::vector<glm::vec2> &uvs, std::vector<GLuint> &indices) override;

    void setShader() override;

    void setMVP() override;

};


#endif //FLIGHTX_GRASS_H

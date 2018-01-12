//
// Created by qweer29 on 1/12/18.
//

#ifndef FLIGHTX_TERRAIN_H
#define FLIGHTX_TERRAIN_H

#include "camera.h"
#include <shader.h>

extern Camera *currentcamera;

class Terrain {
public:
    Terrain(int width, int height);

    void Draw();

private:
    void setMVP();

    void setShader();

    int width;
    int height;
    Shader shader;
};


#endif //FLIGHTX_TERRAIN_H

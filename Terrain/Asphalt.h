//
// Created by longmensparkle on 2018/1/14.
//

#ifndef FLIGHTX_ASPHALT_H
#define FLIGHTX_ASPHALT_H

#include "Terrain.h"
#include "Paint.h"
#include "Cross.h"

class Asphalt : public Terrain
{
public:
    Asphalt(int width, int height);

    void Draw(GLuint shadowMap, glm::mat4 &lightSpaceMatrix) override;

    void DrawDepth(Shader &shader) override;

private:
    Paint *paint;
    Cross *cross;
};


#endif //FLIGHTX_ASPHALT_H

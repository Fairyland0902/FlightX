//
// Created by longmensparkle on 2018/1/14.
//

#ifndef FLIGHTX_PAINT_H
#define FLIGHTX_PAINT_H


#include "Terrain.h"

class Paint: public Terrain{
public:
    Paint(int width, int height);
    void Draw() override ;
    void setMVP(glm::mat4 trans, glm::mat4 view, glm::mat4 projection);

protected:
    glm::mat4 trans, view, projection;
};


#endif //FLIGHTX_PAINT_H

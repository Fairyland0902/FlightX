//
// Created by longmensparkle on 2018/1/14.
//

#ifndef FLIGHTX_ASPHALT_H
#define FLIGHTX_ASPHALT_H


#include "Terrain.h"
#include "Paint.h"

class Asphalt: public Terrain {
public:
    Asphalt(int width, int height);
    void Draw() override;

private:
    Paint *paint;
};


#endif //FLIGHTX_ASPHALT_H

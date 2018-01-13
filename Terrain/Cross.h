//
// Created by longmensparkle on 2018/1/14.
//

#ifndef FLIGHTX_CROSS_H
#define FLIGHTX_CROSS_H


#include "Paint.h"

class Cross: public Paint {
public:
    Cross(int width, int height);
    void Draw() override;
};


#endif //FLIGHTX_CROSS_H

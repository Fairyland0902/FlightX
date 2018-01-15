//
// Created by longmensparkle on 2018/1/14.
//

#ifndef FLIGHTX_CROSS_H
#define FLIGHTX_CROSS_H


#include "Paint.h"

class Cross final : public AbstractTerrain {
public:
    Cross(int width, int height);

    void Draw() override;

    void setMVP(glm::mat4 trans, glm::mat4 view, glm::mat4 projection);

    void setShader() override;

    Texture2D texture;

protected:
    glm::mat4 trans, view, projection;
};


#endif //FLIGHTX_CROSS_H

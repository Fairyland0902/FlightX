//
// Created by longmensparkle on 2018/1/14.
//

#ifndef FLIGHTX_ASPHALT_H
#define FLIGHTX_ASPHALT_H

#include "Terrain.h"
#include "Paint.h"
#include "Cross.h"

class Asphalt final : public AbstractTerrain {
public:
    Asphalt(int width, int height);

    void Draw(GLuint shadowMap, glm::mat4 &lightSpaceMatrix) override;

    void DrawDepth(Shader &shader) override;

private:
    void setShader() override;

    Texture2D Albedo;
    Texture2D Normal;
    Texture2D Metallic;
    Texture2D Roughness;
    Texture2D AO;

    Paint *paint;
    Cross *cross;
};


#endif //FLIGHTX_ASPHALT_H

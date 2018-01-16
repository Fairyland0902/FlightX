//
// Created by qweer29 on 1/12/18.
//

#ifndef FLIGHTX_TERRAIN_H
#define FLIGHTX_TERRAIN_H

#include <camera.h>
#include <shader.h>
#include <vector>
#include "HeightGenerator.h"
#include "AbstractTerrain.h"


class Terrain final : public AbstractTerrain {
public:
    Terrain(int width, int height);

    virtual void Draw(GLuint shadowMap, glm::mat4 &lightSpaceMatrix);

    virtual void DrawDepth(Shader &shader);


protected:

    virtual void setShader() override;

    Texture2D Albedo;
    Texture2D Normal;
    Texture2D Metallic;
    Texture2D Roughness;
    Texture2D AO;

};


#endif //FLIGHTX_TERRAIN_H

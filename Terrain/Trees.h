//
// Created by qweer29 on 1/16/18.
//

#ifndef FLIGHTX_TREES_H
#define FLIGHTX_TREES_H


#include <GL/glew.h>
#include <shader.h>
#include <vector>
#include "AbstractTerrain.h"

class Trees: public AbstractTerrain {
public:
    Trees(int width, int height);

    void Draw();
    float tree_height = -200.f;
    float height = 1.f;

    void init() override ;

protected:

    void setMVP() override ;

    void setShader() override;

    float angle;
    float clipping = 0.f;

    Texture2D snowy_tree;
    Texture2D normal_tree;
    Texture2D reed_tree;

    std::vector<glm::vec2> vertices;


    void generateCoord(std::vector<glm::vec2> &vertices, std::vector<glm::vec2> &uvs,
                        std::vector<GLuint> &indices);
};


#endif //FLIGHTX_TREES_H

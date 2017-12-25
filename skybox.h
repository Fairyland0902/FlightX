#ifndef _SKYBOX_H
#define _SKYBOX_H

#include <GL/glew.h>
#include "cloud/screenAlignedTriangle.h"
#include "shader.h"

class SkyBox
{
public:
    SkyBox();

    ~SkyBox();

    void Draw();

private:
    ScreenAlignedTriangle screenAlignedTriangle;
    Shader skyShader;

    GLuint cubeMap;
};

#endif

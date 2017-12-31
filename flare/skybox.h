#ifndef _SKYBOX_H
#define _SKYBOX_H

#include <vector>
#include <GL/glew.h>
#include "../shader.h"
#include "../cloud/screenAlignedTriangle.h"
#include "hdrTextureCube.h"

class SkyBox
{
public:
    SkyBox();

    ~SkyBox();

    void Draw();

private:
    GLuint loadCubemap(std::vector<std::string> faces);

    Shader skyboxShader;

    ScreenAlignedTriangle screenAlignedTriangle;

    GLuint cubemapTexture;
};


#endif

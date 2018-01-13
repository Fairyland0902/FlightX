#ifndef _FLARERENDER_H
#define _FLARERENDER_H

#include <camera.h>
#include <cloud/screenAlignedTriangle.h>
#include "hdrTextureCube.h"
#include <framebuffer.h>
#include "skybox.h"

#define MAX_DOWNSCALE_FBO 4

extern Camera *currentcamera;

class FlareRender
{
public:
    FlareRender(int width_, int height_);

    ~FlareRender();

    void Init();

    void Draw();

private:
    void skyLightPass();

    void lensFlarePass();

    void capture();

    void brightPass(float threshold);

    void swapBuffers();

    int width, height;

    SkyBox *skyBox;
    HDRTextureCube *hdrTextureCube;
    // A full-screen triangle used for frame buffer rendering.
    ScreenAlignedTriangle *triangle;

    Framebuffer *mainFBO;
    Framebuffer *skyFBO;
    Framebuffer *readFBO;
    Framebuffer *writeFBO;
    Framebuffer *downScaleFBO[MAX_DOWNSCALE_FBO];
};

#endif

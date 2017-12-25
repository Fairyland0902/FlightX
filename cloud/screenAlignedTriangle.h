#ifndef _SCREEN_ALIGNED_TRIANGLES
#define _SCREEN_ALIGNED_TRIANGLES

#include <GL/glew.h>

class ScreenAlignedTriangle
{
public:
    ScreenAlignedTriangle();

    ~ScreenAlignedTriangle();

    void Draw();

private:
    GLuint VAO, VBO;
};

#endif

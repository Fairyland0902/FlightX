#ifndef _OCEAN_H
#define _OCEAN_H
#include <string>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "wave.h"
#include <camera.h>
#include <cloud/screenAlignedTriangle.h>
#include <shader.h>

#define MESH_RESOLUTION 320
#define MAX_FBO_ATTACHMENTS 5

extern Camera *currentcamera;

struct frame_buffer
{
    glm::vec2 Size;
    GLuint NumAttachments;
    GLuint FBO;
    GLuint DepthBufferID;
    GLuint BufferIDs[MAX_FBO_ATTACHMENTS];
};

class Ocean
{
public:
    Ocean(int width, int height);

    ~Ocean();

    void Init();

    void Draw(float deltaTime);

private:
    Wave *wave_model;
    // Mesh resolution
    int N;
    int M;
    float L_x;
    float L_z;

    float A;
    // Wind speed
    float V;
    // Wind direction
    glm::vec2 omega;

    float heightMax;
    float heightMin;

    GLuint VAO, VBO, EBO;
    int indexSize;

    int width;
    int height;
    float lastTime;

    // PBR
    GLuint GGXLUT;
    GLuint GlossyEnvmap;
    ScreenAlignedTriangle *triangle;
    Shader ocean;

    void init_buffer_objects();

    // _WAVE_
    // Build the mesh using the height provided by the algorithm.
    void buildTessendorfWaveMesh(float time);

    // Compute the GGX texture used for PBR.
    GLuint precomputeGGXLUT(GLuint width);

    // Load the glossy cube map for IBL
    GLuint loadCubemap(std::vector<std::string> faces);
};

#endif

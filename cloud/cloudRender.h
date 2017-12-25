#ifndef _CLOUD_RENDER
#define _CLOUD_RENDER

#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "../camera.h"
#include "screenAlignedTriangle.h"
#include "cloud.h"

class CloudRender
{
public:
    CloudRender(int width, int height, Camera &camera);

    ~CloudRender();

    void Draw(float deltaTime);

private:
    void ubo_init();

    int backBufferResolutionX;
    int backBufferResolutionY;


    // Global UBO.
    GLuint screenUBO;
    GLuint viewUBO;
    GLuint timingsUBO;

    // Camera matrix.
    Camera &camera;

    glm::mat4 projectionMatrix;

    // Clouds.
    std::unique_ptr<Cloud> clouds;
    // Screen aligned triangles.
    std::unique_ptr<ScreenAlignedTriangle> screenAlignedTriangle;
};

#endif

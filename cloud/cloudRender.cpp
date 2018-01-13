#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "cloudRender.h"

CloudRender::CloudRender(int width, int height)
{
    backBufferResolutionX = width;
    backBufferResolutionY = height;

    // Projection matrix.
    projectionMatrix = glm::perspective(currentcamera->Zoom, (float) width / (float) height,
                                        currentcamera->NearClippingPlaneDistance,
                                        currentcamera->FarClippingPlaneDistance);

    // Uniform buffers.
    ubo_init();
    // Init screen aligned triangles.
    screenAlignedTriangle.reset(new ScreenAlignedTriangle());
    // Init cloud rendering.
    clouds.reset(new Cloud(backBufferResolutionX, backBufferResolutionY, currentcamera->FarClippingPlaneDistance,
                           *screenAlignedTriangle.get()));
}

CloudRender::~CloudRender()
{
    glDeleteBuffers(1, &screenUBO);
    glDeleteBuffers(1, &viewUBO);
    glDeleteBuffers(1, &timingsUBO);
}

void CloudRender::ubo_init()
{
    // screen ubo
    glGenBuffers(1, &screenUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, screenUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(float) * 4 * 5, NULL, GL_STREAM_DRAW);
    // Write projection.
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float) * 4 * 4, glm::value_ptr(projectionMatrix));
    float inverseScreenResolution[] = {1.0f / backBufferResolutionX, 1.0f / backBufferResolutionY};
    // Write inverse screen.
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 4 * 4, sizeof(float) * 2, inverseScreenResolution);
    // Bind to index 0 - its assumed that ubo-index0-binding will never change.
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, screenUBO);

    // view ubo
    glGenBuffers(1, &viewUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, viewUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(float) * 4 * 16, NULL, GL_STREAM_DRAW);
    // Bind to index 1 - its assumed that ubo-index1-binding will never change.
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, viewUBO);

    // timings ubo
    glGenBuffers(1, &timingsUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, timingsUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(float) * 2, NULL, GL_STREAM_DRAW);
    // Bind to index 2 - its assumed that ubo-index2-binding will never change.
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, timingsUBO);
}

void CloudRender::Draw(float deltaTime)
{
    // Update global matrices.
    int offset = 0;
    glm::mat4 viewMatrix = currentcamera->GetViewMatrix();
    glm::mat4 viewProjectionMatrix = projectionMatrix * viewMatrix;
    glm::mat4 inverseViewProjection = glm::inverse(viewProjectionMatrix);

    glBindBuffer(GL_UNIFORM_BUFFER, viewUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(float) * 4 * 4, glm::value_ptr(viewMatrix));
    offset += sizeof(float) * 4 * 4;
    glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(float) * 4 * 4, glm::value_ptr(viewProjectionMatrix));
    offset += sizeof(float) * 4 * 4;
    glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(float) * 4 * 4, glm::value_ptr(inverseViewProjection));
    offset += sizeof(float) * 4 * 4;
    glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(float) * 3, glm::value_ptr(currentcamera->Position));
    offset += sizeof(float) * 4;
    glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(float) * 3,
                    glm::value_ptr(glm::vec3(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0])));
    offset += sizeof(float) * 4;
    glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(float) * 3,
                    glm::value_ptr(glm::vec3(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1])));
    offset += sizeof(float) * 4;
    glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(float) * 3,
                    glm::value_ptr(-glm::vec3(viewMatrix[0][2], viewMatrix[1][2], viewMatrix[2][2])));
    offset += sizeof(float) * 4;


    // Update timings.
    glBindBuffer(GL_UNIFORM_BUFFER, timingsUBO);
    float timings[2] = {static_cast<float>(glfwGetTime()), deltaTime};
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float) * 2, timings);

    glm::vec3 lightDirection(1.0f, -1.0f, 0.0f);
    lightDirection = glm::normalize(lightDirection);

    clouds->Draw(inverseViewProjection, viewMatrix, currentcamera->Position, currentcamera->Front, lightDirection);
}
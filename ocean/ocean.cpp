#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ocean.h"
#include "../resource_manager.h"

Ocean::Ocean(int width, int height) :
        width(width),
        height(height)
{
    /*
     * Constants initialization.
     */

    // Mesh resolution
    N = MESH_RESOLUTION;
    M = MESH_RESOLUTION;
    L_x = 1000;
    L_z = 1000;

    A = 3e-7f;
    // Wind speed
    V = 10;
    // Wind direction
    omega = glm::vec2(0.0f, 1.0f);

    heightMax = 0;
    heightMin = 0;

    lastTime = 0.0f;
}

Ocean::~Ocean()
{
    delete wave_model;
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Ocean::Init()
{
    // Wave model initialization.
    wave_model = new Wave(N, M, L_x, L_z, omega, V, A, 1.0f);

    // Buffer object initialization.
    init_buffer_objects();

//    // Full-screen aligned triangle initialization.
//    triangle = new ScreenAlignedTriangle();
//
//    // Generate the GGX texture.
//    GGXLUT = precomputeGGXLUT(width);
//
//    // Generate glossy environemnt map.
//    std::vector<std::string> faces
//            {
//                    _TEXTURE_PREFIX_"/skybox/TropicalSunnyDayRight_blur.png",
//                    _TEXTURE_PREFIX_"/skybox/TropicalSunnyDayLeft_blur.png",
//                    _TEXTURE_PREFIX_"/skybox/TropicalSunnyDayUp_blur.png",
//                    _TEXTURE_PREFIX_"/skybox/TropicalSunnyDayDown_blur.png",
//                    _TEXTURE_PREFIX_"/skybox/TropicalSunnyDayBack_blur.png",
//                    _TEXTURE_PREFIX_"/skybox/TropicalSunnyDayFront_blur.png"
//            };
//    GlossyEnvmap = loadCubemap(faces);

    // Shader configuration.
    ocean = ResourceManager::GetShader("ocean");
//    ocean.Use();
//    ocean.SetInteger("GGXLUT", 0);
//    ocean.SetInteger("Skybox", 1);
}

void Ocean::Draw(float deltaTime)
{
    // Update wave mesh.
    float currentTime = lastTime + deltaTime;
    buildTessendorfWaveMesh(currentTime);
    lastTime = currentTime;

    ocean.Use();

    ocean.SetVector3f("light.direction", glm::vec3(0.0f, -1.0f, 3.0f));
    ocean.SetVector3f("viewPos", currentcamera->GetViewPosition());
    ocean.SetFloat("heightMin", heightMin);
    ocean.SetFloat("heightMax", heightMax);
    // Set lights properties
    ocean.SetVector3f("light.ambient", 1.0f, 1.0f, 1.0f);
    ocean.SetVector3f("light.diffuse", 1.0f, 1.0f, 1.0f);
    ocean.SetVector3f("light.specular", 1.0f, 0.9f, 0.7f);

    // Create camera transformations
    glm::mat4 view = currentcamera->GetViewMatrix();
    glm::mat4 projection = glm::perspective(currentcamera->Zoom, (GLfloat) width / (GLfloat) height,
                                            currentcamera->NearClippingPlaneDistance,
                                            currentcamera->FarClippingPlaneDistance);
    ocean.SetMatrix4("view", view);
    ocean.SetMatrix4("projection", projection);

//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, GGXLUT);
//    glActiveTexture(GL_TEXTURE1);
//    glBindTexture(GL_TEXTURE_CUBE_MAP, GlossyEnvmap);

    glm::mat4 model = glm::mat4();
    model = glm::translate(model, glm::vec3(0.0f, -1000.0f, 0.0f));
    ocean.SetMatrix4("model", model);

    // Draw mesh.
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexSize, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Ocean::init_buffer_objects()
{
    indexSize = (N - 1) * (M - 1) * 6;
    GLuint *indices = new GLuint[indexSize];
    int p = 0;

    for (int j = 0; j < N - 1; j++)
    {
        for (int i = 0; i < M - 1; i++)
        {
            indices[p++] = i + j * N;
            indices[p++] = (i + 1) + j * N;
            indices[p++] = i + (j + 1) * N;

            indices[p++] = (i + 1) + j * N;
            indices[p++] = (i + 1) + (j + 1) * N;
            indices[p++] = i + (j + 1) * N;
        }
    }

    // Element buffer object.
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize * sizeof(GLuint), indices, GL_STATIC_DRAW);
    glBindVertexArray(0);

    delete[] indices;
}

void Ocean::buildTessendorfWaveMesh(float time)
{
    int vertices = N * M;

    wave_model->buildField(time);
    glm::vec3 *heightField = wave_model->heightField;
    glm::vec3 *normalField = wave_model->normalField;

    int p = 0;

    for (int i = 0; i < N; i++)
        for (int j = 0; j < M; j++)
        {
            int index = j * N + i;

            if (heightField[index].y > heightMax)
                heightMax = heightField[index].y;
            else if (heightField[index].y < heightMin)
                heightMin = heightField[index].y;
        }


    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    int fieldArraySize = sizeof(glm::vec3) * vertices;
    glBufferData(GL_ARRAY_BUFFER, fieldArraySize * 2, NULL, GL_STATIC_DRAW);

    // Copy height to buffer
    glBufferSubData(GL_ARRAY_BUFFER, 0, fieldArraySize, heightField);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *) 0);
    glEnableVertexAttribArray(0);

    // Copy normal to buffer
    glBufferSubData(GL_ARRAY_BUFFER, fieldArraySize, fieldArraySize, normalField);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *) fieldArraySize);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

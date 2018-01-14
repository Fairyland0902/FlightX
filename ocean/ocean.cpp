#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ocean.h"
#include <resource_manager.h>
#include <stb_image.h>

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
    omega = glm::vec2(0.01f, 0.0f);

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

    // Full-screen aligned triangle initialization.
    triangle = new ScreenAlignedTriangle();

    // Generate the GGX texture.
    GGXLUT = precomputeGGXLUT(512);
//
    // Generate glossy environemnt map.
    std::vector<std::string> faces
            {
                    _TEXTURE_PREFIX_"/skybox/TropicalSunnyDayRight_blur.png",
                    _TEXTURE_PREFIX_"/skybox/TropicalSunnyDayLeft_blur.png",
                    _TEXTURE_PREFIX_"/skybox/TropicalSunnyDayUp_blur.png",
                    _TEXTURE_PREFIX_"/skybox/TropicalSunnyDayDown_blur.png",
                    _TEXTURE_PREFIX_"/skybox/TropicalSunnyDayBack_blur.png",
                    _TEXTURE_PREFIX_"/skybox/TropicalSunnyDayFront_blur.png"
            };
    GlossyEnvmap = loadCubemap(faces);

    // Shader configuration.
    ocean = ResourceManager::GetShader("ocean");
    ocean.Use();
    ocean.SetInteger("GGXLUT", 0);
    ocean.SetInteger("Skybox", 1);
}

void Ocean::Draw(float deltaTime)
{
    // Update wave mesh.
    float currentTime = lastTime + deltaTime;
    buildTessendorfWaveMesh(currentTime);
    lastTime = currentTime;

    ocean.Use();

    ocean.SetFloat("Time", currentTime);

//    ocean.SetVector3f("light.direction", glm::vec3(0.0f, -1.0f, 3.0f));
//    ocean.SetVector3f("viewPos", currentcamera->GetViewPosition());
//    ocean.SetFloat("heightMin", heightMin);
//    ocean.SetFloat("heightMax", heightMax);
//    // Set lights properties
//    ocean.SetVector3f("light.ambient", 1.0f, 1.0f, 1.0f);
//    ocean.SetVector3f("light.diffuse", 1.0f, 1.0f, 1.0f);
//    ocean.SetVector3f("light.specular", 1.0f, 0.9f, 0.7f);

    // Create camera transformations
    glm::mat4 view = currentcamera->GetViewMatrix();
	glm::mat4 projection = currentcamera->GetProjectionMatrix();
    ocean.SetMatrix4("ViewMatrix", view);
    ocean.SetMatrix4("ProjMatrix", projection);
    ocean.SetVector3f("CameraPos", currentcamera->GetViewPosition());

    // Set light properties.
    ocean.SetVector3f("SunDirection", glm::vec3(0.0f, 1.0f, 0.0f));
    ocean.SetVector3f("LightColor", 1.0f, 1.0f, 1.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, GGXLUT);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, GlossyEnvmap);

    glm::mat4 model = glm::mat4();
    model = glm::translate(model, glm::vec3(0.0f, -1000.0f, 0.0f));
    ocean.SetMatrix4("ModelMatrix", model);

    // Draw mesh.
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexSize, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
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

// Some frame buffer operations.

static GLuint FBOAttachments[MAX_FBO_ATTACHMENTS] =
        {
                GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
                GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4
        };

void DestroyFramebuffer(frame_buffer *FB)
{
    if (FB->FBO > 0)
    {
        glDeleteTextures(FB->NumAttachments, FB->BufferIDs);
        glDeleteRenderbuffers(1, &FB->DepthBufferID);
        FB->DepthBufferID = 0;
        glDeleteFramebuffers(1, &FB->FBO);
        FB->FBO = 0;
        FB->Size = glm::vec2(0);
        FB->NumAttachments = 0;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

frame_buffer MakeFramebuffer(GLuint NumAttachments, glm::vec2 Size, bool AddDepthBuffer)
{
    assert(NumAttachments < MAX_FBO_ATTACHMENTS);

    frame_buffer FB = {};
    FB.Size = Size;
    FB.NumAttachments = NumAttachments;

    glGenFramebuffers(1, &FB.FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FB.FBO);
    glDrawBuffers((GLsizei) NumAttachments, FBOAttachments);

    if (AddDepthBuffer)
    {
        glGenRenderbuffers(1, &FB.DepthBufferID);
        glBindRenderbuffer(GL_RENDERBUFFER, FB.DepthBufferID);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, Size.x, Size.y);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, FB.DepthBufferID);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cout << "Framebuffer creation error : not complete." << std::endl;
            DestroyFramebuffer(&FB);
        }
    }

    return FB;
}

// Some texture operations.

void FormatFromChannels(GLuint Channels, bool IsFloat, bool FloatHalfPrecision, GLint *BaseFormat, GLint *Format)
{
    if (IsFloat)
    {
        switch (Channels)
        {
            case 1:
                *BaseFormat = FloatHalfPrecision ? GL_R16F : GL_R32F;
                *Format = GL_RED;
                break;
            case 2:
                *BaseFormat = FloatHalfPrecision ? GL_RG16F : GL_RG32F;
                *Format = GL_RG;
                break;
            case 3:
                *BaseFormat = FloatHalfPrecision ? GL_RGB16F : GL_RGB32F;
                *Format = GL_RGB;
                break;
            case 4:
                *BaseFormat = FloatHalfPrecision ? GL_RGBA16F : GL_RGBA32F;
                *Format = GL_RGBA;
                break;
        }
    } else
    {
        switch (Channels)
        {
            case 1:
                *BaseFormat = *Format = GL_RED;
                break;
            case 2:
                *BaseFormat = *Format = GL_RG;
                break;
            case 3:
                *BaseFormat = *Format = GL_RGB;
                break;
            case 4:
                *BaseFormat = *Format = GL_RGBA;
                break;
        }
    }
}

GLuint
Make2DTexture(void *ImageBuffer, GLuint Width, GLuint Height, GLuint Channels, bool IsFloat, bool FloatHalfPrecision,
              GLfloat AnisotropicLevel, int MagFilter, int MinFilter, int WrapS, int WrapT)
{
    GLuint Texture;
    glGenTextures(1, &Texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture);

    GLint CurrentAlignment;
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &CurrentAlignment);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, MinFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, MagFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, WrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, WrapT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, AnisotropicLevel);

    GLint BaseFormat, Format;
    FormatFromChannels(Channels, IsFloat, FloatHalfPrecision, &BaseFormat, &Format);
    GLenum Type = IsFloat ? GL_FLOAT : GL_UNSIGNED_BYTE;

    glTexImage2D(GL_TEXTURE_2D, 0, BaseFormat, Width, Height, 0, Format, Type, ImageBuffer);

    if (MinFilter >= GL_NEAREST_MIPMAP_NEAREST && MinFilter <= GL_LINEAR_MIPMAP_LINEAR)
    { // NOTE - Generate mipmaps if mag filter ask it
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, CurrentAlignment);

    glBindTexture(GL_TEXTURE_2D, 0);

    return Texture;
}

GLuint Ocean::precomputeGGXLUT(GLuint width)
{
    glActiveTexture(GL_TEXTURE0);
    GLuint texture = Make2DTexture(NULL, width, width, 2, true, true, 1, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE,
                                   GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, texture);

    frame_buffer FBO = MakeFramebuffer(1, glm::vec2(width, width), true);

    glBindFramebuffer(GL_FRAMEBUFFER, FBO.FBO);
    glBindRenderbuffer(GL_RENDERBUFFER, FBO.DepthBufferID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, width);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    glViewport(0, 0, width, width);
    Shader LUT = ResourceManager::GetShader("GGX");
    LUT.Use();
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    triangle->Draw();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    DestroyFramebuffer(&FBO);

    return texture;
}

GLuint Ocean::loadCubemap(std::vector<std::string> faces)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        } else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return textureID;
}

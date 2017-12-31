#include <iostream>
#include "skybox.h"
#include "../stb_image.h"
#include "../resource_manager.h"

SkyBox::SkyBox()
{
    // Shader configuration.
    skyboxShader = ResourceManager::GetShader("skybox");
    skyboxShader.Use();
    skyboxShader.SetInteger("skybox", 0);

    // View binding = 1
    GLuint blockIndex = glGetUniformBlockIndex(skyboxShader.ID, "View");
    glUniformBlockBinding(skyboxShader.ID, blockIndex, 1);

    // Load textures.
    std::vector<std::string> faces
            {
                    _TEXTURE_PREFIX_"/skybox/TropicalSunnyDayRight.png",
                    _TEXTURE_PREFIX_"/skybox/TropicalSunnyDayLeft.png",
                    _TEXTURE_PREFIX_"/skybox/TropicalSunnyDayUp.png",
                    _TEXTURE_PREFIX_"/skybox/TropicalSunnyDayDown.png",
                    _TEXTURE_PREFIX_"/skybox/TropicalSunnyDayBack.png",
                    _TEXTURE_PREFIX_"/skybox/TropicalSunnyDayFront.png"
            };
    cubemapTexture = loadCubemap(faces);
}

SkyBox::~SkyBox()
{
    glDeleteTextures(1, &cubemapTexture);
}

void SkyBox::Draw()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    skyboxShader.Use();
    screenAlignedTriangle.Draw();
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

GLuint SkyBox::loadCubemap(std::vector<std::string> faces)
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

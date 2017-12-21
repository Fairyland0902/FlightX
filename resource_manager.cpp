/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "resource_manager.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include "stb_image.h"

// Instantiate static variables.
std::map<std::string, Shader>    ResourceManager::Shaders;
std::map<std::string, Texture2D> ResourceManager::Textures;
std::map<std::string, Model *>   ResourceManager::Models;
std::map<std::string, GLuint>    ResourceManager::VAOmap;
std::map<std::string, int>       ResourceManager::VAOSizeMap;
std::map<std::string, glm::vec3> ResourceManager::modelSizeMap;

Shader ResourceManager::LoadShader(const string& vShaderFile, const string& fShaderFile, const string& gShaderFile,
                                   std::string name)
{
    Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
    return Shaders[name];
}

Shader ResourceManager::GetShader(std::string name)
{
    return Shaders[name];
}

void ResourceManager::Clear()
{
    // (Properly) delete all shaders.
    for (auto iter : Shaders)
        glDeleteProgram(iter.second.ID);
}

Shader
ResourceManager::loadShaderFromFile(const string& vShaderFile, const string& fShaderFile, const string& gShaderFile)
{
    // 1. Retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    try
    {
        // Open files
        std::ifstream vertexShaderFile(vShaderFile);
        std::ifstream fragmentShaderFile(fShaderFile);
        std::stringstream vShaderStream, fShaderStream;
        // Read file's buffer contents into streams
        vShaderStream << vertexShaderFile.rdbuf();
        fShaderStream << fragmentShaderFile.rdbuf();
        // close file handlers
        vertexShaderFile.close();
        fragmentShaderFile.close();
        // Convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        // If geometry shader path is present, also load a geometry shader
        if (gShaderFile.empty())
        {
            std::ifstream geometryShaderFile(gShaderFile);
            std::stringstream gShaderStream;
            gShaderStream << geometryShaderFile.rdbuf();
            geometryShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    }
    catch (std::exception e)
    {
        std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
    }
    const string& vShaderCode = vertexCode.c_str();
    const string& fShaderCode = fragmentCode.c_str();
    const string& gShaderCode = geometryCode.c_str();
    // 2. Now create shader object from source code
    Shader shader;
    shader.Compile(vShaderCode.c_str(), fShaderCode.c_str(), gShaderFile.empty() ? gShaderCode.c_str() : nullptr);
    return shader;
}

Texture2D ResourceManager::LoadTexture(const string& file, GLboolean alpha, std::string name)
{
    Textures[name] = loadTextureFromFile(file, alpha);
    return Textures[name];
}

Texture2D ResourceManager::GetTexture(std::string name)
{
    return Textures[name];
}

// Private constructor, that is we do not want any actual resource manager objects. Its members and functions should be publicly available (static).
ResourceManager::ResourceManager()
{}

Texture2D ResourceManager::loadTextureFromFile(const string& file, GLboolean alpha)
{
    // Create Texture object
    Texture2D texture;
    if (alpha)
    {
        texture.Internal_Format = GL_RGBA;
        texture.Image_Format = GL_RGBA;
    }
    // Load image
    int width, height;
    unsigned char *image = stbi_load(file.c_str(), &width, &height, 0,
                                           texture.Image_Format == GL_RGBA ? STBI_rgb_alpha : STBI_rgb);
    // Now generate texture
    texture.Generate(width, height, image);
    // And finally free image data
    stbi_image_free(image);
    return texture;
}

void ResourceManager::LoadModel(const std::string objModelFile, std::string name)
{
<<<<<<< HEAD
=======
//    std::string str = "models/" + name + ".obj";
//    if (name == "cube") str = "models/stone.3ds";
//    if (name == "player") str = "models/rectangle.obj";
//    if (name == "rocket") str = "models/rocket3.obj";
//    if (name == "cannon") str = "models/Naval/cannon.obj";
//    if (name == "sphere") str = "models/sphere.obj";
//    if (name == "gun") str = "models/portal/portalgun1.obj";
//    if (name == "building") str = "models/eastern ancient casttle/eastern ancient casttle.obj";
//    string& path = new char[str.length()];
//    memcpy(path, str.c_str(), str.length() + 1);
//    puts("adding");
//    if (!LoadedModels.count(name))
//    {
//        // std::cout << path << std::endl;
//        Model *m = new Model(path);
//        // std::cout << path << std::endl;
//        LoadedModels[name] = m;
////        modelSizeMap[name] = m->size;
//        puts("done");
//    }
>>>>>>> 11a224ee057affa6d268103a139d7a75e1c44961
    if (!Models.count(name))
    {
        Model *m = new Model(objModelFile);
        Models[name] = m;
    }
}

Model *ResourceManager::GetModel(std::string name)
{
    return Models[name];
}

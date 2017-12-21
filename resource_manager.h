#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <map>
#include <string>

#include <GL/glew.h>
#include "texture.h"
#include "shader.h"
#include "model.h"
using std::string;
// A static singleton ResourceManager class that hosts several
// functions to load Textures and Shaders. Each loaded texture
// and/or shader is also stored for future reference by string
// handles. All functions and resources are static and no
// public constructor is defined.
class ResourceManager
{
public:
    // Resource storage
    static std::map<string, Shader> Shaders;
    static std::map<string, Texture2D> Textures;
    static std::map<string, Model *> Models;
    static std::map<string, GLuint> VAOmap;
    static std::map<string, int> VAOSizeMap;
    static std::map<string, glm::vec3> modelSizeMap;

    // Loads (and generates) a shader program from file loading vertex, fragment (and geometry) shader's source code. If gShaderFile is not nullptr, it also loads a geometry shader.
    static Shader
    LoadShader(const string& vShaderFile, const string& fShaderFile, const string& gShaderFile, string name);

    // Retrieves a stored shader.
    static Shader GetShader(string name);

    // Properly de-allocates all loaded resources.
    static void Clear();

    static Texture2D LoadTexture(const string& file, GLboolean alpha, string name);

    // Retrieves a stored texture.
    static Texture2D GetTexture(string name);

    // Load a model from a .obj file.
    static void LoadModel(string objModelFile, string name);

    // Retrieves a stored model.
    static Model *GetModel(string name);

private:
    // Private constructor, that is we do not want any actual resource manager objects. Its members and functions should be publicly available (static).
    ResourceManager();

    // Loads and generates a shader from file
    static Shader
    loadShaderFromFile(const string& vShaderFile, const string& fShaderFile, const string& gShaderFile = nullptr);

    static Texture2D loadTextureFromFile(const string& file, GLboolean alpha);
};

#endif
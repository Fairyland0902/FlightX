/* Adapted from the online tuturials by Joey de Vries found at 'http://learnopengl.com/'
 * Loads meshes (associated VAOs/VBOs etc and renders them)
 */

#ifndef _MESH_H
#define _MESH_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

using namespace std;

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/types.h>

#include "shader.h"


struct Vertex
{
    // Position
    glm::vec3 Position;
    // Normal
    glm::vec3 Normal;
    // TexCoords
    glm::vec2 TexCoords;
};

struct Texture
{
    GLuint id;
    string type;
    aiString path;
};

class Mesh
{
public:
    /*  Mesh Data  */
    vector<Vertex> vertices;
    vector<GLuint> indices;
    vector<Texture> textures;

    /*  Functions  */
    // Default constructor
    Mesh();
    // Constructor
    Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures);

    // Render the mesh.
    void Draw(Shader shader);

private:
    /*  Render data  */
    GLuint VAO, VBO, EBO;

    /*  Functions    */
    // Initializes all the buffer objects/arrays.
    void setupMesh();
};

#endif

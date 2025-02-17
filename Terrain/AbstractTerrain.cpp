//
// Created by qweer29 on 1/15/18.
//

#include "AbstractTerrain.h"

AbstractTerrain::AbstractTerrain(int width, int height) : width(width), height(height)
{}

void AbstractTerrain::init()
{
    generateCoord(vertices, uvs, indices);
    generateNormal(normals);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Set the objects we need in the rendering process (namely, VAO, VBO and EBO).
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &UV);
    glGenBuffers(1, &Normal);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const GLvoid *) 0);
    glEnableVertexAttribArray(0);
    // TexCoords attribute.
    glBindBuffer(GL_ARRAY_BUFFER, UV);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * uvs.size(), &uvs[0], GL_STATIC_DRAW);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (const GLvoid *) 0);
    glEnableVertexAttribArray(1);
    // Normal attribute.
    glBindBuffer(GL_ARRAY_BUFFER, Normal);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), &normals[0], GL_STATIC_DRAW);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const GLvoid *) 0);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void AbstractTerrain::setMVP()
{
    glm::mat4 trans;
    glm::mat4 view = currentcamera->GetViewMatrix();
    glm::mat4 projection = currentcamera->GetProjectionMatrix();
    shader.SetMatrix4("model", trans);
    shader.SetMatrix4("view", view);
    shader.SetMatrix4("projection", projection);
}

void AbstractTerrain::generateCoord(std::vector<glm::vec3> &vertices, std::vector<glm::vec2> &uvs,
                                    std::vector<GLuint> &indices)
{
    const float slice = 1.0 / (float) (n - 1);

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
        {
            float height = absolute_height;
//            std::cout << i << ' ' << j << std::endl;
            vertices.emplace_back(i * slice * (chunk_width) - chunk_width / 2, height,
                                  j * slice * (chunk_height) - chunk_height / 2);
            uvs.emplace_back(float(i) / n * 40, float(j) / n * 40);
            if (i < n - 1 && j < n - 1)
            {
                indices.emplace_back(i * n + j);
                indices.emplace_back(i * n + n + j);
                indices.emplace_back(i * n + n + j + 1);

                indices.emplace_back(i * n + j);
                indices.emplace_back(i * n + 1 + j);
                indices.emplace_back(i * n + n + j + 1);
            }
        }
}

void AbstractTerrain::generateNormal(std::vector<glm::vec3> &normals)
{
    // Initialize normal vector.
    for (size_t i = 0; i < vertices.size(); i++)
    {
        normals.push_back(glm::vec3(0.0f));
    }

    for (size_t i = 0; i < indices.size(); i += 3)
    {
        // Get the vertices.
        glm::vec3 v0 = vertices[indices[i + 0]];
        glm::vec3 v1 = vertices[indices[i + 1]];
        glm::vec3 v2 = vertices[indices[i + 2]];

        // Calculate normal defined by this triangle.
        glm::vec3 n = glm::cross(v1 - v0, v2 - v0);
        // Flip if necessary.
        if (n.y < 0)
            n = -n;

        // Apply normal to corresponding vertices.
        normals[indices[i + 0]] = glm::normalize(normals[indices[i + 0]] + n);
        normals[indices[i + 1]] = glm::normalize(normals[indices[i + 1]] + n);
        normals[indices[i + 2]] = glm::normalize(normals[indices[i + 2]] + n);
    }
}


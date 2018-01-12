#include <GL/glew.h>
#include "plane.h"

Plane::Plane()
{}

void Plane::Draw()
{
    GLfloat vertices[] = {
            100.0f, -100.0f, 100.0f,
            100.0f, -100.0f, -100.0f,
            -100.0f, -100.0f, -100.0f,
            -100.0f, -100.0f, 100.0f
    };

    GLuint indices[] = {
            0, 1, 3,
            1, 2, 3
    };

    // Set the objects we need in the rendering process (namely, VAO, VBO and EBO).
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const GLvoid *) 0);
    glEnableVertexAttribArray(0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Unbind VAO.
    glBindVertexArray(0);
}

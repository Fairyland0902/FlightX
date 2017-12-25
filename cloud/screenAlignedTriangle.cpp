#include "screenAlignedTriangle.h"

ScreenAlignedTriangle::ScreenAlignedTriangle()
{
    GLfloat vertices[] = {
            -1.0f, -1.0f,
            3.0f, -1.0f,
            -1.0f, 3.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

ScreenAlignedTriangle::~ScreenAlignedTriangle()
{
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

void ScreenAlignedTriangle::Draw()
{
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}


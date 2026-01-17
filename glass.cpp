#include "glass.h"
#include <iostream>

// Constructor
GlassWindow::GlassWindow(const glm::vec3& pos, const glm::vec3& windowSize)
    : position(pos), size(windowSize),
    transparency(0.85f), reflectivity(0.15f),
    refractionIndex(1.52f), tintColor(1.0f, 1.0f, 1.0f),
    VAO(0), VBO(0), EBO(0) {
}

void GlassWindow::generateVertices() {
    vertices.clear();
    indices.clear();

    float halfWidth = size.x / 2.0f;
    float halfHeight = size.y / 2.0f;
    float halfDepth = size.z / 2.0f;

    // Create a thin quad with normals and texture coordinates
    // Position (3), Normal (3), Color (3), TexCoord (2) = 11 floats per vertex

    // Front face vertices
    vertices = {
        // Position              Normal            Color (white base)    TexCoord
        -halfWidth, -halfHeight,  halfDepth,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
         halfWidth, -halfHeight,  halfDepth,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
         halfWidth,  halfHeight,  halfDepth,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f,
        -halfWidth,  halfHeight,  halfDepth,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f,

        // Back face vertices
        -halfWidth, -halfHeight, -halfDepth,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
         halfWidth, -halfHeight, -halfDepth,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
         halfWidth,  halfHeight, -halfDepth,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f,
        -halfWidth,  halfHeight, -halfDepth,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
    };

    // Indices for both front and back faces (2 quads = 4 triangles)
    indices = {
        // Front face
        0, 1, 2,
        2, 3, 0,

        // Back face
        4, 5, 6,
        6, 7, 4,

        // Edges (optional, for thickness)
        0, 4, 7,
        7, 3, 0,

        1, 5, 6,
        6, 2, 1,

        0, 1, 5,
        5, 4, 0,

        3, 2, 6,
        6, 7, 3
    };
}

void GlassWindow::setupBuffers() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Position attribute (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Color attribute (location = 2)
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Texture coordinate attribute (location = 3)
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
}

void GlassWindow::setup() {
    generateVertices();
    setupBuffers();
}

void GlassWindow::draw(unsigned int shaderProgram) {
    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Disable back face culling for glass (optional)
    // glDisable(GL_CULL_FACE);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Re-enable face culling
    // glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
}

void GlassWindow::cleanup() {
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        VAO = VBO = EBO = 0;
    }
}

void GlassWindow::updateShader(unsigned int shaderProgram, const glm::vec3& lightPos,
    const glm::vec3& viewPos, const glm::vec3& lightColor,
    float ambientStrength) {
    glUseProgram(shaderProgram);

    // Set glass properties (transparency is already here)
    glUniform1f(glGetUniformLocation(shaderProgram, "transparency"), transparency);
    glUniform1f(glGetUniformLocation(shaderProgram, "reflectivity"), reflectivity);
    glUniform3fv(glGetUniformLocation(shaderProgram, "tintColor"), 1, &tintColor[0]);

    // Set lighting properties
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, &lightPos[0]);
    glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, &viewPos[0]);
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, &lightColor[0]);
    glUniform1f(glGetUniformLocation(shaderProgram, "ambientStrength"), ambientStrength);
}
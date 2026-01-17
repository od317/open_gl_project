#include "light_source.h"
#include <iostream>

// Constructor
LightSource::LightSource(const glm::vec3& pos, const glm::vec3& color)
    : position(pos), lightColor(color),
    ambientStrength(0.2f), specularStrength(0.5f),
    constant(1.0f), linear(0.09f), quadratic(0.032f),
    VAO(0), VBO(0), EBO(0) {
}

void LightSource::generateCubeVertices() {
    // Create a small cube to represent the light source
    float size = 0.2f; // Small cube size
    float halfSize = size / 2.0f;

    // Cube vertices: position (3), color (3), normal (3) = 9 floats per vertex
    vertices = {
        // Front face
        -halfSize, -halfSize,  halfSize,  lightColor.r, lightColor.g, lightColor.b,  0.0f, 0.0f, 1.0f,
         halfSize, -halfSize,  halfSize,  lightColor.r, lightColor.g, lightColor.b,  0.0f, 0.0f, 1.0f,
         halfSize,  halfSize,  halfSize,  lightColor.r, lightColor.g, lightColor.b,  0.0f, 0.0f, 1.0f,
        -halfSize,  halfSize,  halfSize,  lightColor.r, lightColor.g, lightColor.b,  0.0f, 0.0f, 1.0f,

        // Back face
        -halfSize, -halfSize, -halfSize,  lightColor.r, lightColor.g, lightColor.b,  0.0f, 0.0f, -1.0f,
         halfSize, -halfSize, -halfSize,  lightColor.r, lightColor.g, lightColor.b,  0.0f, 0.0f, -1.0f,
         halfSize,  halfSize, -halfSize,  lightColor.r, lightColor.g, lightColor.b,  0.0f, 0.0f, -1.0f,
        -halfSize,  halfSize, -halfSize,  lightColor.r, lightColor.g, lightColor.b,  0.0f, 0.0f, -1.0f,

        // Left face
        -halfSize, -halfSize, -halfSize,  lightColor.r, lightColor.g, lightColor.b,  -1.0f, 0.0f, 0.0f,
        -halfSize, -halfSize,  halfSize,  lightColor.r, lightColor.g, lightColor.b,  -1.0f, 0.0f, 0.0f,
        -halfSize,  halfSize,  halfSize,  lightColor.r, lightColor.g, lightColor.b,  -1.0f, 0.0f, 0.0f,
        -halfSize,  halfSize, -halfSize,  lightColor.r, lightColor.g, lightColor.b,  -1.0f, 0.0f, 0.0f,

        // Right face
         halfSize, -halfSize, -halfSize,  lightColor.r, lightColor.g, lightColor.b,  1.0f, 0.0f, 0.0f,
         halfSize, -halfSize,  halfSize,  lightColor.r, lightColor.g, lightColor.b,  1.0f, 0.0f, 0.0f,
         halfSize,  halfSize,  halfSize,  lightColor.r, lightColor.g, lightColor.b,  1.0f, 0.0f, 0.0f,
         halfSize,  halfSize, -halfSize,  lightColor.r, lightColor.g, lightColor.b,  1.0f, 0.0f, 0.0f,

         // Top face
         -halfSize,  halfSize, -halfSize,  lightColor.r, lightColor.g, lightColor.b,  0.0f, 1.0f, 0.0f,
          halfSize,  halfSize, -halfSize,  lightColor.r, lightColor.g, lightColor.b,  0.0f, 1.0f, 0.0f,
          halfSize,  halfSize,  halfSize,  lightColor.r, lightColor.g, lightColor.b,  0.0f, 1.0f, 0.0f,
         -halfSize,  halfSize,  halfSize,  lightColor.r, lightColor.g, lightColor.b,  0.0f, 1.0f, 0.0f,

         // Bottom face
         -halfSize, -halfSize, -halfSize,  lightColor.r, lightColor.g, lightColor.b,  0.0f, -1.0f, 0.0f,
          halfSize, -halfSize, -halfSize,  lightColor.r, lightColor.g, lightColor.b,  0.0f, -1.0f, 0.0f,
          halfSize, -halfSize,  halfSize,  lightColor.r, lightColor.g, lightColor.b,  0.0f, -1.0f, 0.0f,
         -halfSize, -halfSize,  halfSize,  lightColor.r, lightColor.g, lightColor.b,  0.0f, -1.0f, 0.0f
    };

    // Cube indices
    indices.clear();
    for (int face = 0; face < 6; face++) {
        unsigned int baseIndex = face * 4;
        indices.push_back(baseIndex);
        indices.push_back(baseIndex + 1);
        indices.push_back(baseIndex + 2);
        indices.push_back(baseIndex);
        indices.push_back(baseIndex + 2);
        indices.push_back(baseIndex + 3);
    }
}

void LightSource::setupBuffers() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Normal attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void LightSource::setup() {
    generateCubeVertices();
    setupBuffers();
}

void LightSource::draw(unsigned int shaderProgram) {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void LightSource::cleanup() {
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        VAO = VBO = EBO = 0;
    }
}

void LightSource::updateShader(unsigned int shaderProgram, const glm::vec3& viewPos) {
    glUseProgram(shaderProgram);

    // Set light properties
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, &position[0]);
    glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, &viewPos[0]);
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, &lightColor[0]);
    glUniform1f(glGetUniformLocation(shaderProgram, "ambientStrength"), ambientStrength);
    glUniform1f(glGetUniformLocation(shaderProgram, "specularStrength"), specularStrength);

    // Set attenuation (point light properties)
    glUniform1f(glGetUniformLocation(shaderProgram, "constant"), constant);
    glUniform1f(glGetUniformLocation(shaderProgram, "linear"), linear);
    glUniform1f(glGetUniformLocation(shaderProgram, "quadratic"), quadratic);
}
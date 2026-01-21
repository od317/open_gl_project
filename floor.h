#pragma once
#ifndef FLOOR_H
#define FLOOR_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class Floor {
private:
    glm::vec3 position;
    glm::vec3 size;
    glm::vec3 color;
    float shininess;

    unsigned int floorVAO;
    unsigned int floorVBO;
    unsigned int floorTexture;
    bool useTexture;

    // Static vertex data
    static const float floorVertices[44];
    static const unsigned int floorIndices[6];

    unsigned char* generateCheckerboardPattern(int width, int height, int cellSize);
    bool generateDefaultTexture();

public:
    Floor(const glm::vec3& position = glm::vec3(0.0f),
        const glm::vec3& size = glm::vec3(1.0f),
        const glm::vec3& color = glm::vec3(0.8f, 0.8f, 0.8f));
    ~Floor();

    void setup();
    bool loadTexture(const std::string& path);
    void setTextureRepeat(float repeatX, float repeatY);

    void draw(unsigned int shaderProgram);
    void cleanup();

    // Simplified version for your current shaders
    void draw();

    // Setters
    void setColor(const glm::vec3& newColor);
    void setShininess(float newShininess);
    void setUseTexture(bool useTex);

    // Getters
    glm::vec3 getPosition() const;
    glm::vec3 getSize() const;
    glm::vec3 getColor() const;
    bool hasTexture() const;
};

#endif
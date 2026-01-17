#pragma once
#ifndef LIGHT_SOURCE_H
#define LIGHT_SOURCE_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

class LightSource {
private:
    // Light properties
    glm::vec3 position;
    glm::vec3 lightColor;
    float ambientStrength;
    float specularStrength;
    float constant;
    float linear;
    float quadratic;

    // Visual representation (a small glowing cube)
    unsigned int VAO, VBO, EBO;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    void generateCubeVertices();
    void setupBuffers();

public:
    // Constructor
    LightSource(const glm::vec3& pos = glm::vec3(0.0f, 2.0f, 0.0f),
        const glm::vec3& color = glm::vec3(1.0f, 1.0f, 1.0f));

    // Setup and rendering
    void setup();
    void draw(unsigned int shaderProgram);
    void cleanup();

    // Getters and setters
    glm::vec3 getPosition() const { return position; }
    glm::vec3 getColor() const { return lightColor; }
    float getAmbientStrength() const { return ambientStrength; }
    float getSpecularStrength() const { return specularStrength; }
    float getConstant() const { return constant; }
    float getLinear() const { return linear; }
    float getQuadratic() const { return quadratic; }

    void setPosition(const glm::vec3& pos) { position = pos; }
    void setColor(const glm::vec3& color) { lightColor = color; }
    void setAmbientStrength(float strength) { ambientStrength = strength; }
    void setSpecularStrength(float strength) { specularStrength = strength; }
    void setAttenuation(float c, float l, float q) {
        constant = c; linear = l; quadratic = q;
    }

    // Update light in shader (for lighting calculations on other objects)
    void updateShader(unsigned int shaderProgram, const glm::vec3& viewPos);
};

#endif
#pragma once
#ifndef GLASS_H
#define GLASS_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

class GlassWindow {
private:
    // Window properties
    glm::vec3 position;
    glm::vec3 size;
    float transparency;
    float reflectivity;
    float refractionIndex;
    glm::vec3 tintColor;

    // OpenGL objects
    unsigned int VAO, VBO, EBO;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    void generateVertices();
    void setupBuffers();

public:
    // Constructor
    GlassWindow(const glm::vec3& pos = glm::vec3(0.0f, 1.5f, 4.9f),
        const glm::vec3& windowSize = glm::vec3(3.0f, 2.0f, 0.01f));

    // Transparency controls
    void setTransparency(float transparency) {
        this->transparency = glm::clamp(transparency, 0.0f, 1.0f);
    }


    void increaseTransparency(float amount = 0.1f) {
        transparency = glm::clamp(transparency + amount, 0.0f, 1.0f);
    }

    void decreaseTransparency(float amount = 0.1f) {
        transparency = glm::clamp(transparency - amount, 0.0f, 1.0f);
    }

    // Quick presets
    void setFullyTransparent() { transparency = 0.9f; }      // 90% transparent
    void setSemiTransparent() { transparency = 0.5f; }       // 50% transparent
    void setSlightlyTransparent() { transparency = 0.2f; }   // 20% transparent
    void setOpaque() { transparency = 0.0f; }               // 0% transparent (solid)

    // Setup and rendering
    void setup();
    void draw(unsigned int shaderProgram);
    void cleanup();

    // Getters and setters
    glm::vec3 getPosition() const { return position; }
    glm::vec3 getSize() const { return size; }
    float getTransparency() const { return transparency; }
    float getReflectivity() const { return reflectivity; }
    float getRefractionIndex() const { return refractionIndex; }
    glm::vec3 getTintColor() const { return tintColor; }

    void setPosition(const glm::vec3& pos) { position = pos; }
    void setSize(const glm::vec3& windowSize) { size = windowSize; }
    void setReflectivity(float reflect) { reflectivity = glm::clamp(reflect, 0.0f, 1.0f); }
    void setRefractionIndex(float index) { refractionIndex = index; }
    void setTintColor(const glm::vec3& color) { tintColor = color; }

    // Advanced glass properties
    void setAsClearGlass() {
        transparency = 0.9f;
        reflectivity = 0.1f;
        refractionIndex = 1.52f;
        tintColor = glm::vec3(1.0f, 1.0f, 1.0f);
    }

    void setAsTintedGlass() {
        transparency = 0.7f;
        reflectivity = 0.3f;
        refractionIndex = 1.52f;
        tintColor = glm::vec3(0.8f, 0.9f, 1.0f); // Light blue tint
    }

    // Send glass properties to shader
    void updateShader(unsigned int shaderProgram, const glm::vec3& lightPos,
        const glm::vec3& viewPos, const glm::vec3& lightColor,
        float ambientStrength = 0.2f);
};

#endif
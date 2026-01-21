#pragma once
#ifndef SKYBOX_H
#define SKYBOX_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>

class Skybox {
private:
    unsigned int skyboxVAO, skyboxVBO;
    unsigned int cubemapTexture;
    bool loadCrossFormat3x4(unsigned char* data, int width, int height, int nrChannels);
    bool loadCrossFormat4x3(unsigned char* data, int width, int height, int nrChannels);
    unsigned char* extractFace(unsigned char* data, int width, int height, int channels,
        int faceSize, int gridX, int gridY);

    // Skybox vertices
    float skyboxVertices[108] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

public:
    Skybox();
    ~Skybox();

    // Load cubemap texture
    bool loadCubemap(const std::vector<std::string>& faces);
    bool loadDefaultCubemap(); // Load a simple gradient skybox
    bool loadCrossFormatCubemap(const std::string& imagePath);

    // Setup and rendering
    void setup();
    void draw(unsigned int shaderProgram, const glm::mat4& view, const glm::mat4& projection);
    void cleanup();

    // Get texture ID for reflections
    unsigned int getTextureID() const { return cubemapTexture; }
};

#endif
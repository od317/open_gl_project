#include "texture.h"
#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>

TextureManager::TextureManager() : floorTexture(0), wallTexture(0), ceilingTexture(0) {
}

TextureManager::~TextureManager() {
    cleanup();
}

void TextureManager::initialize() {
    // Create default textures
    floorTexture = createCheckeredTexture();
    wallTexture = createDefaultTexture(glm::vec3(0.8f, 0.8f, 0.8f));
    ceilingTexture = createDefaultTexture(glm::vec3(0.9f, 0.9f, 0.9f));

    std::cout << "Texture manager initialized with default textures" << std::endl;
}

void TextureManager::cleanup() {
    if (floorTexture != 0) glDeleteTextures(1, &floorTexture);
    if (wallTexture != 0) glDeleteTextures(1, &wallTexture);
    if (ceilingTexture != 0) glDeleteTextures(1, &ceilingTexture);

    floorTexture = wallTexture = ceilingTexture = 0;
}

unsigned int TextureManager::loadTexture(const std::string& path) {
    // Simple placeholder - in real implementation, you'd load from file
    std::cout << "Loading texture from: " << path << std::endl;
    return createDefaultTexture(glm::vec3(0.5f, 0.5f, 0.5f));
}

unsigned int TextureManager::createDefaultTexture(const glm::vec3& color) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    unsigned char data[] = {
        static_cast<unsigned char>(color.r * 255),
        static_cast<unsigned char>(color.g * 255),
        static_cast<unsigned char>(color.b * 255)
    };

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return textureID;
}

unsigned int TextureManager::createCheckeredTexture() {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    const int size = 64;
    unsigned char data[size * size * 3];

    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            int index = (y * size + x) * 3;
            bool isDark = ((x / 8) + (y / 8)) % 2 == 0;

            if (isDark) {
                data[index] = 100;
                data[index + 1] = 100;
                data[index + 2] = 100;
            }
            else {
                data[index] = 200;
                data[index + 1] = 200;
                data[index + 2] = 200;
            }
        }
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return textureID;
}
#include "floor.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"

// Define the floor vertices and indices
const float Floor::floorVertices[] = {
    // Positions          // Colors           // Normals         // Texture Coords
    -0.5f, 0.0f, -0.5f,   0.8f, 0.8f, 0.8f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
     0.5f, 0.0f, -0.5f,   0.8f, 0.8f, 0.8f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
     0.5f, 0.0f,  0.5f,   0.8f, 0.8f, 0.8f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
    -0.5f, 0.0f,  0.5f,   0.8f, 0.8f, 0.8f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f
};

const unsigned int Floor::floorIndices[] = {
    0, 1, 2,
    0, 2, 3
};

Floor::Floor(const glm::vec3& position, const glm::vec3& size, const glm::vec3& color)
    : position(position), size(size), color(color), shininess(32.0f),
    floorVAO(0), floorVBO(0), floorTexture(0), useTexture(true) {
}

Floor::~Floor() {
    cleanup();
}

void Floor::setup() {
    // Generate buffers
    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &floorVBO);
    unsigned int floorEBO;
    glGenBuffers(1, &floorEBO);

    // Bind VAO
    glBindVertexArray(floorVAO);

    // Bind VBO and copy vertex data
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);

    // Bind EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(floorIndices), floorIndices, GL_STATIC_DRAW);

    // Position attribute (location 0) - 3 floats
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color attribute (location 1) - 3 floats
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Normal attribute (location 2) - 3 floats
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Texture coordinate attribute (location 3) - 2 floats
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
    glEnableVertexAttribArray(3);

    // Unbind
    glBindVertexArray(0);
    glDeleteBuffers(1, &floorEBO);

    // Generate default texture
    generateDefaultTexture();
}

unsigned char* Floor::generateCheckerboardPattern(int width, int height, int cellSize) {
    unsigned char* data = new unsigned char[width * height * 3];

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int cellX = x / cellSize;
            int cellY = y / cellSize;

            // Determine checkerboard color
            bool isDark = (cellX + cellY) % 2 == 0;

            int index = (y * width + x) * 3;

            if (isDark) {
                // Dark gray - good contrast
                data[index] = 40;     // R
                data[index + 1] = 40; // G
                data[index + 2] = 45; // B
            }
            else {
                // Light gray
                data[index] = 70;     // R
                data[index + 1] = 70; // G
                data[index + 2] = 75; // B
            }
        }
    }

    return data;
}

bool Floor::generateDefaultTexture() {
    // Generate checkerboard pattern
    int width = 512;
    int height = 512;
    int cellSize = 32; // Each checker square is 32x32 pixels

    unsigned char* data = generateCheckerboardPattern(width, height, cellSize);

    // Generate texture
    glGenTextures(1, &floorTexture);
    glBindTexture(GL_TEXTURE_2D, floorTexture);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Clean up
    delete[] data;
    glBindTexture(GL_TEXTURE_2D, 0);

    std::cout << "Generated default checkerboard floor texture" << std::endl;
    return true;
}

bool Floor::loadTexture(const std::string& path) {
    // Delete existing texture
    if (floorTexture != 0) {
        glDeleteTextures(1, &floorTexture);
        floorTexture = 0;
    }

    // Load texture using stb_image
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    if (data) {
        glGenTextures(1, &floorTexture);
        glBindTexture(GL_TEXTURE_2D, floorTexture);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Determine format
        GLenum format = GL_RGB;
        if (nrChannels == 4)
            format = GL_RGBA;
        else if (nrChannels == 1)
            format = GL_RED;

        // Upload texture data
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
        glBindTexture(GL_TEXTURE_2D, 0);

        std::cout << "Loaded floor texture: " << path << " (" << width << "x" << height
            << ", channels: " << nrChannels << ")" << std::endl;
        useTexture = true;
        return true;
    }
    else {
        std::cout << "Failed to load floor texture: " << path << std::endl;
        std::cout << "Generating default checkerboard pattern..." << std::endl;
        return generateDefaultTexture();
    }
}

void Floor::setTextureRepeat(float repeatX, float repeatY) {
    glBindTexture(GL_TEXTURE_2D, floorTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void Floor::draw(unsigned int shaderProgram) {
    if (floorVAO == 0) {
        std::cerr << "Floor not initialized! Call setup() first." << std::endl;
        return;
    }

    glUseProgram(shaderProgram);

    // Set model matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, size);

    // Set model uniform
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    if (modelLoc != -1) {
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    }

    // Set color uniform
    GLint colorLoc = glGetUniformLocation(shaderProgram, "objectColor");
    if (colorLoc != -1) {
        glUniform3f(colorLoc, color.r, color.g, color.b);
    }

    // Bind texture if available
    if (useTexture && floorTexture != 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        GLint textureLoc = glGetUniformLocation(shaderProgram, "texture_diffuse1");
        if (textureLoc != -1) {
            glUniform1i(textureLoc, 0);
        }
    }

    // Draw floor
    glBindVertexArray(floorVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

// Simple draw function for basic usage
void Floor::draw() {
    if (floorVAO == 0) {
        std::cerr << "Floor not initialized! Call setup() first." << std::endl;
        return;
    }

    // Bind texture if available
    if (useTexture && floorTexture != 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
    }

    // Draw floor
    glBindVertexArray(floorVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Floor::cleanup() {
    if (floorVAO != 0) {
        glDeleteVertexArrays(1, &floorVAO);
        floorVAO = 0;
    }
    if (floorVBO != 0) {
        glDeleteBuffers(1, &floorVBO);
        floorVBO = 0;
    }
    if (floorTexture != 0) {
        glDeleteTextures(1, &floorTexture);
        floorTexture = 0;
    }
}

void Floor::setColor(const glm::vec3& newColor) {
    color = newColor;
}

void Floor::setShininess(float newShininess) {
    shininess = newShininess;
}

void Floor::setUseTexture(bool useTex) {
    useTexture = useTex;
}

glm::vec3 Floor::getPosition() const {
    return position;
}

glm::vec3 Floor::getSize() const {
    return size;
}

glm::vec3 Floor::getColor() const {
    return color;
}

bool Floor::hasTexture() const {
    return floorTexture != 0;
}
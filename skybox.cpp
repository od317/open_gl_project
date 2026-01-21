#include "skybox.h"
#include <iostream>
#include <vector>

#include "stb_image.h" 

Skybox::Skybox() : skyboxVAO(0), skyboxVBO(0), cubemapTexture(0) {
}

Skybox::~Skybox() {
    cleanup();
}

void Skybox::setup() {
    // Setup skybox VAO/VBO
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);

    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);

    // Load default cubemap
    loadDefaultCubemap();
}

bool Skybox::loadCubemap(const std::vector<std::string>& faces) {
    glGenTextures(1, &cubemapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format = GL_RGB;
            if (nrChannels == 4)
                format = GL_RGBA;
            else if (nrChannels == 1)
                format = GL_RED;

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
            std::cout << "Loaded skybox face: " << faces[i] << std::endl;
        }
        else {
            std::cout << "Failed to load skybox face: " << faces[i] << std::endl;
            stbi_image_free(data);
            return false;
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    return true;
}

bool Skybox::loadDefaultCubemap() {
    // Create a simple gradient skybox programmatically
    glGenTextures(1, &cubemapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

    // Sky colors (top to bottom gradient)
    unsigned char skyColors[6][4] = {
        {180, 220, 255, 255},  // Right - light blue
        {200, 230, 255, 255},  // Left - slightly lighter blue
        {220, 240, 255, 255},  // Top - lightest blue
        {150, 200, 255, 255},  // Bottom - darker blue
        {190, 225, 255, 255},  // Front - medium blue
        {170, 210, 255, 255}   // Back - medium-dark blue
    };

    // Create simple 1x1 textures for each face
    for (unsigned int i = 0; i < 6; i++) {
        unsigned char data[4];
        data[0] = skyColors[i][0];
        data[1] = skyColors[i][1];
        data[2] = skyColors[i][2];
        data[3] = skyColors[i][3];

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    std::cout << "Created default gradient skybox" << std::endl;
    return true;
}

void Skybox::draw(unsigned int shaderProgram, const glm::mat4& view, const glm::mat4& projection) {
    // Draw skybox last (with depth test trick)
    glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal

    glUseProgram(shaderProgram);

    // Remove translation from the view matrix for skybox
    glm::mat4 viewNoTranslation = glm::mat4(glm::mat3(view));

    // Set uniforms
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &viewNoTranslation[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);

    // Skybox cube
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    // Reset depth function
    glDepthFunc(GL_LESS);
}

bool Skybox::loadCrossFormat3x4(unsigned char* data, int width, int height, int nrChannels) {
    // 3x4 layout
    // +---+---+---+
    // |   | +Y |   |  row 0
    // +---+---+---+
    // | -X| +Z | +X|  row 1  
    // +---+---+---+
    // |   | -Y |   |  row 2
    // +---+---+---+
    // |   | -Z |   |  row 3
    // +---+---+---+

    int faceSize = width / 3;

    if (faceSize * 4 != height) {
        std::cout << "Invalid 3x4 layout dimensions" << std::endl;
        stbi_image_free(data);
        return false;
    }

    glGenTextures(1, &cubemapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

    // Extract each face
    std::vector<unsigned char*> faces = {
        extractFace(data, width, height, nrChannels, faceSize, 0, 1),  // right (+X)
        extractFace(data, width, height, nrChannels, faceSize, 2, 1),  // left (-X)
        extractFace(data, width, height, nrChannels, faceSize, 1, 0),  // top (+Y)
        extractFace(data, width, height, nrChannels, faceSize, 1, 2),  // bottom (-Y)
        extractFace(data, width, height, nrChannels, faceSize, 1, 1),  // front (+Z)
        extractFace(data, width, height, nrChannels, faceSize, 1, 3)   // back (-Z)
    };

    // Load each face
    for (unsigned int i = 0; i < 6; i++) {
        GLenum format = GL_RGB;
        if (nrChannels == 4)
            format = GL_RGBA;
        else if (nrChannels == 1)
            format = GL_RED;

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0, format, faceSize, faceSize, 0, format, GL_UNSIGNED_BYTE, faces[i]);

        delete[] faces[i];
    }

    stbi_image_free(data);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // Generate mipmaps
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    std::cout << "Loaded 3x4 cross-format skybox with face size: " << faceSize << std::endl;
    return true;
}

bool Skybox::loadCrossFormat4x3(unsigned char* data, int width, int height, int nrChannels) {
    // 4x3 layout (horizontal cross)
    // +---+---+---+---+
    // |   | +Y |   |   |  row 0
    // +---+---+---+---+
    // | -X| +Z | +X| -Z|  row 1
    // +---+---+---+---+
    // |   | -Y |   |   |  row 2
    // +---+---+---+---+

    int faceSize = height / 3;

    if (faceSize * 4 != width) {
        std::cout << "Invalid 4x3 layout dimensions" << std::endl;
        stbi_image_free(data);
        return false;
    }

    glGenTextures(1, &cubemapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

    // Extract each face
    std::vector<unsigned char*> faces = {
        extractFace(data, width, height, nrChannels, faceSize, 2, 1),  // right (+X)
        extractFace(data, width, height, nrChannels, faceSize, 0, 1),  // left (-X)
        extractFace(data, width, height, nrChannels, faceSize, 1, 0),  // top (+Y)
        extractFace(data, width, height, nrChannels, faceSize, 1, 2),  // bottom (-Y)
        extractFace(data, width, height, nrChannels, faceSize, 1, 1),  // front (+Z)
        extractFace(data, width, height, nrChannels, faceSize, 3, 1)   // back (-Z)
    };

    // Load each face
    for (unsigned int i = 0; i < 6; i++) {
        GLenum format = GL_RGB;
        if (nrChannels == 4)
            format = GL_RGBA;
        else if (nrChannels == 1)
            format = GL_RED;

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0, format, faceSize, faceSize, 0, format, GL_UNSIGNED_BYTE, faces[i]);

        delete[] faces[i];
    }

    stbi_image_free(data);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // Generate mipmaps
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    std::cout << "Loaded 4x3 cross-format skybox with face size: " << faceSize << std::endl;
    return true;
}

unsigned char* Skybox::extractFace(unsigned char* data, int width, int height, int channels,
    int faceSize, int gridX, int gridY) {
    unsigned char* faceData = new unsigned char[faceSize * faceSize * channels];

    for (int y = 0; y < faceSize; y++) {
        for (int x = 0; x < faceSize; x++) {
            int srcX = gridX * faceSize + x;
            int srcY = gridY * faceSize + y;
            int srcIdx = (srcY * width + srcX) * channels;
            int dstIdx = (y * faceSize + x) * channels;

            for (int c = 0; c < channels; c++) {
                faceData[dstIdx + c] = data[srcIdx + c];
            }
        }
    }

    return faceData;
}

bool Skybox::loadCrossFormatCubemap(const std::string& imagePath) {
    int width, height, nrChannels;
    unsigned char* data = stbi_load(imagePath.c_str(), &width, &height, &nrChannels, 0);

    if (!data) {
        std::cout << "Failed to load cross-format skybox: " << imagePath << std::endl;
        return false;
    }

    std::cout << "Loaded cross-format skybox: " << imagePath
        << " (" << width << "x" << height << ", " << nrChannels << " channels)" << std::endl;

    // Cross format layout: (assuming 3x2 layout)
    // +-------+-------+-------+
    // |       |  up   |       |
    // +-------+-------+-------+
    // | left  | front | right |
    // +-------+-------+-------+
    // |       | down  |       |
    // +-------+-------+-------+
    // |       | back  |       |
    // +-------+-------+-------+

    // Each face should be (width/3) x (height/4) in 3x4 layout
    // OR (width/4) x (height/3) in 4x3 layout

    // Determine layout based on aspect ratio
    float aspect = (float)width / height;

    if (fabs(aspect - (3.0f / 4.0f)) < 0.1f) {
        // 3x4 layout (common for cross format)
        return loadCrossFormat3x4(data, width, height, nrChannels);
    }
    else if (fabs(aspect - (4.0f / 3.0f)) < 0.1f) {
        // 4x3 layout
        return loadCrossFormat4x3(data, width, height, nrChannels);
    }
    else {
        std::cout << "Unknown cubemap layout. Using default skybox." << std::endl;
        stbi_image_free(data);
        return loadDefaultCubemap();
    }
}

void Skybox::cleanup() {
    if (skyboxVAO != 0) {
        glDeleteVertexArrays(1, &skyboxVAO);
        skyboxVAO = 0;
    }
    if (skyboxVBO != 0) {
        glDeleteBuffers(1, &skyboxVBO);
        skyboxVBO = 0;
    }
    if (cubemapTexture != 0) {
        glDeleteTextures(1, &cubemapTexture);
        cubemapTexture = 0;
    }
}
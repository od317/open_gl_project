#pragma once
#pragma once
#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <string>
#include <glm/glm.hpp>

class TextureManager {
private:
    unsigned int floorTexture;
    unsigned int wallTexture;
    unsigned int ceilingTexture;

public:
    TextureManager();
    ~TextureManager();

    void initialize();
    void cleanup();

    unsigned int getFloorTexture() const { return 1; }
    unsigned int getWallTexture() const { return wallTexture; }
    unsigned int getCeilingTexture() const { return ceilingTexture; }

    unsigned int loadTexture(const std::string& path);

private:
    unsigned int createDefaultTexture(const glm::vec3& color);
    unsigned int createCheckeredTexture();
};

#endif
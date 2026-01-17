#ifndef ROOM_H
#define ROOM_H

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Room {
private:
    // Room dimensions
    float roomWidth;
    float roomHeight;
    float roomDepth;

    // Colors for different surfaces
    glm::vec3 floorColor;
    glm::vec3 ceilingColor;
    glm::vec3 wallColors[4];  // 0:back, 1:front, 2:left, 3:right

    // Window wall properties
    bool hasInteriorWall = false;
    glm::vec3 interiorWallPos;
    glm::vec3 interiorWallSize;
    glm::vec3 windowPosRelative;  // Position relative to interior wall
    glm::vec3 interiorWindowSize; // Renamed to avoid conflict
    glm::vec3 interiorWallColor;
    glm::vec3 interiorWallNormal;

    bool needsUpdate;

    // OpenGL objects
    unsigned int VAO, VBO, EBO;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    void generateVertices();
    void updateBuffers();
    void addQuad(const glm::vec3& p1, const glm::vec3& p2,
        const glm::vec3& p3, const glm::vec3& p4,
        const glm::vec3& color, const glm::vec3& normal);
    void createWallWithWindow(const glm::vec3& position, const glm::vec3& size,
        const glm::vec3& windowPos, const glm::vec3& windowSize,
        const glm::vec3& color, const glm::vec3& normal);

public:
    Room(float width = 10.0f, float height = 4.0f, float depth = 10.0f);

    void setup();
    void draw();
    void cleanup();

    // Getters for collision detection
    float getWidth() const { return roomWidth; }
    float getHeight() const { return roomHeight; }
    float getDepth() const { return roomDepth; }

    // Set room color schemes
    void setFloorColor(const glm::vec3& color) { floorColor = color; needsUpdate = true; }
    void setCeilingColor(const glm::vec3& color) { ceilingColor = color; needsUpdate = true; }
    void setWallColor(int wallIndex, const glm::vec3& color);

    // Interior wall with window
    void addInteriorWallWithWindow(const glm::vec3& position, const glm::vec3& size,
        const glm::vec3& windowRelativePos, const glm::vec3& windowSize);

    // Get window position for placing glass
    glm::vec3 getWindowPosition() const;
    glm::vec3 getWindowSize() const { return interiorWindowSize; }
};

#endif
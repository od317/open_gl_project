#ifndef ROOM_H
#define ROOM_H

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Room {
private:
    // Exhibition hall dimensions - MUCH LARGER
    float roomWidth;
    float roomHeight;
    float roomDepth;

    // Modern showroom color scheme (all in light gray)
    glm::vec3 wallColor;           // Single color for all walls
    glm::vec3 floorColor;
    glm::vec3 ceilingColor;
    glm::vec3 accentColor;         // For architectural details

    // Exhibition features
    bool hasMainWindow = false;
    glm::vec3 windowPos;
    glm::vec3 windowSize;
    glm::vec3 windowWallNormal;

    // Exhibition details
    bool hasColumns = true;
    bool hasDisplayPlatforms = true;

    // OpenGL objects
    unsigned int VAO, VBO, EBO;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    bool needsUpdate;

    // Helper methods
    void generateVertices();
    void updateBuffers();
    void addQuad(const glm::vec3& p1, const glm::vec3& p2,
        const glm::vec3& p3, const glm::vec3& p4,
        const glm::vec3& color, const glm::vec3& normal);
    void addTriangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3,
        const glm::vec3& color, const glm::vec3& normal);
    void createMainWallWithLargeWindow();
    void createColumns();
    void createDisplayPlatforms();
    void createEntranceArch();

public:
    // Constructor for large exhibition hall
    Room(float width = 40.0f, float height = 15.0f, float depth = 30.0f);

    void setup();
    void draw();
    void cleanup();

    // Getters
    float getWidth() const { return roomWidth; }
    float getHeight() const { return roomHeight; }
    float getDepth() const { return roomDepth; }

    // Get window position for placing glass
    glm::vec3 getWindowPosition() const { return windowPos; }
    glm::vec3 getWindowSize() const { return windowSize; }

    // Color settings
    void setWallColor(const glm::vec3& color) { wallColor = color; needsUpdate = true; }
    void setFloorColor(const glm::vec3& color) { floorColor = color; needsUpdate = true; }
    void setCeilingColor(const glm::vec3& color) { ceilingColor = color; needsUpdate = true; }

    // Exhibition features
    void addMainWindow(const glm::vec3& position, const glm::vec3& size);
};

#endif
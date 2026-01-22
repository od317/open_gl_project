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

    // Modern showroom color scheme
    glm::vec3 wallColor;           // Single color for all walls
    glm::vec3 floorColor;
    glm::vec3 ceilingColor;
    glm::vec3 accentColor;         // For architectural details

    // Exhibition features
    bool hasMainWindow = false;
    bool hasSideWindows = false;
    bool hasFrontWindows = false;
    glm::vec3 windowPos;
    glm::vec3 windowSize;
    glm::vec3 windowWallNormal;

    // Window properties for sides and front
    float windowSpacing = 5.0f;
    float windowHeight = 4.0f;
    float windowWidth = 3.0f;

    // Door properties
    bool hasDoors = true;
    float doorAngle = 0.0f;        // For animation
    bool doorsOpen = false;

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
    void createSideWindows();
    void createFrontWindows();
    void createColumns();
    void createDisplayPlatforms();
    void createEntranceArch();
    void createDoorFrames();  // New method
    void createDoors();       // New method

    // Helper for creating windows in walls
    void addWindowToWall(float wallZ, bool isFrontWall = false);
    void createWallWithWindows(float wallZ, const glm::vec3& normal, bool isFrontWall = false);

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

    // Window settings
    void enableSideWindows(bool enable) { hasSideWindows = enable; needsUpdate = true; }
    void enableFrontWindows(bool enable) { hasFrontWindows = enable; needsUpdate = true; }
    void setWindowSpacing(float spacing) { windowSpacing = spacing; needsUpdate = true; }
    void setWindowSize(float width, float height) { 
        windowWidth = width; 
        windowHeight = height; 
        needsUpdate = true; 
    }

    // Exhibition features
    void addMainWindow(const glm::vec3& position, const glm::vec3& size);

    // Door controls
    void toggleDoors();
    void updateDoors(float deltaTime);
    void setDoorsOpen(bool open);
    bool areDoorsOpen() const { return doorsOpen; }
};

#endif
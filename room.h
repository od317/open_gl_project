#ifndef ROOM_H
#define ROOM_H

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Room {
private:
    // Member variables
    float roomWidth;
    float roomHeight;
    float roomDepth;
    glm::vec3 floorColor;
    glm::vec3 ceilingColor;
    glm::vec3 wallColors[4];
    bool needsUpdate;

    unsigned int VAO, VBO, EBO;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    void generateVertices();
    void updateBuffers();

public:
    // Constructor declaration
    Room(float width = 10.0f, float height = 4.0f, float depth = 10.0f);

    // Public method declarations
    void setup();
    void draw();
    void cleanup();

    // Getters for collision detection
    float getWidth() const { return roomWidth; }
    float getHeight() const { return roomHeight; }
    float getDepth() const { return roomDepth; }

    // Setters for colors
    void setFloorColor(const glm::vec3& color);
    void setCeilingColor(const glm::vec3& color);
    void setWallColor(int wallIndex, const glm::vec3& color);
};

#endif
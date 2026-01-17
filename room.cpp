#include "room.h"
#include <iostream>

// Constructor
Room::Room(float width, float height, float depth)
    : roomWidth(width), roomHeight(height), roomDepth(depth),
    needsUpdate(true) {

    // Set default colors
    floorColor = glm::vec3(0.5f, 0.4f, 0.3f);    // Brown
    ceilingColor = glm::vec3(0.8f, 0.8f, 0.85f); // Light gray
    wallColors[0] = glm::vec3(0.2f, 0.3f, 0.8f); // Blue (back)
    wallColors[1] = glm::vec3(0.3f, 0.7f, 0.3f); // Green (front)
    wallColors[2] = glm::vec3(0.8f, 0.2f, 0.2f); // Red (left)
    wallColors[3] = glm::vec3(0.8f, 0.8f, 0.2f); // Yellow (right)

    VAO = VBO = EBO = 0;
}

// Add interior wall with window
void Room::addInteriorWallWithWindow(const glm::vec3& position, const glm::vec3& size,
    const glm::vec3& windowRelativePos, const glm::vec3& windowSize) {
    hasInteriorWall = true;
    interiorWallPos = position;
    interiorWallSize = size;
    windowPosRelative = windowRelativePos;
    interiorWindowSize = windowSize;
    interiorWallColor = glm::vec3(0.6f, 0.6f, 0.6f); // Gray interior wall
    interiorWallNormal = glm::vec3(0.0f, 0.0f, 1.0f); // Facing forward

    needsUpdate = true;
}

// Get window position (absolute)
glm::vec3 Room::getWindowPosition() const {
    return interiorWallPos + windowPosRelative;
}

void Room::setWallColor(int wallIndex, const glm::vec3& color) {
    if (wallIndex >= 0 && wallIndex < 4) {
        wallColors[wallIndex] = color;
        needsUpdate = true;
    }
}

// Helper function to add a quad
void Room::addQuad(const glm::vec3& p1, const glm::vec3& p2,
    const glm::vec3& p3, const glm::vec3& p4,
    const glm::vec3& color, const glm::vec3& normal) {
    int baseIndex = vertices.size() / 9;

    // Vertex 1
    vertices.push_back(p1.x); vertices.push_back(p1.y); vertices.push_back(p1.z);
    vertices.push_back(color.r); vertices.push_back(color.g); vertices.push_back(color.b);
    vertices.push_back(normal.x); vertices.push_back(normal.y); vertices.push_back(normal.z);

    // Vertex 2
    vertices.push_back(p2.x); vertices.push_back(p2.y); vertices.push_back(p2.z);
    vertices.push_back(color.r); vertices.push_back(color.g); vertices.push_back(color.b);
    vertices.push_back(normal.x); vertices.push_back(normal.y); vertices.push_back(normal.z);

    // Vertex 3
    vertices.push_back(p3.x); vertices.push_back(p3.y); vertices.push_back(p3.z);
    vertices.push_back(color.r); vertices.push_back(color.g); vertices.push_back(color.b);
    vertices.push_back(normal.x); vertices.push_back(normal.y); vertices.push_back(normal.z);

    // Vertex 4
    vertices.push_back(p4.x); vertices.push_back(p4.y); vertices.push_back(p4.z);
    vertices.push_back(color.r); vertices.push_back(color.g); vertices.push_back(color.b);
    vertices.push_back(normal.x); vertices.push_back(normal.y); vertices.push_back(normal.z);

    // Add indices for two triangles
    indices.push_back(baseIndex);
    indices.push_back(baseIndex + 1);
    indices.push_back(baseIndex + 2);
    indices.push_back(baseIndex);
    indices.push_back(baseIndex + 2);
    indices.push_back(baseIndex + 3);
}

// Create wall with window hole
void Room::createWallWithWindow(const glm::vec3& position, const glm::vec3& size,
    const glm::vec3& windowPos, const glm::vec3& windowSize,
    const glm::vec3& color, const glm::vec3& normal) {
    float halfWidth = size.x / 2.0f;
    float halfHeight = size.y / 2.0f;
    float windowHalfWidth = windowSize.x / 2.0f;
    float windowHalfHeight = windowSize.y / 2.0f;

    glm::vec3 wallCenter = position;

    // 1. Top rectangle (above window)
    addQuad(
        glm::vec3(wallCenter.x - halfWidth, wallCenter.y + halfHeight, wallCenter.z),
        glm::vec3(wallCenter.x + halfWidth, wallCenter.y + halfHeight, wallCenter.z),
        glm::vec3(wallCenter.x + halfWidth, wallCenter.y + windowHalfHeight + windowPos.y, wallCenter.z),
        glm::vec3(wallCenter.x - halfWidth, wallCenter.y + windowHalfHeight + windowPos.y, wallCenter.z),
        color, normal
    );

    // 2. Bottom rectangle (below window)
    addQuad(
        glm::vec3(wallCenter.x - halfWidth, wallCenter.y - windowHalfHeight + windowPos.y, wallCenter.z),
        glm::vec3(wallCenter.x + halfWidth, wallCenter.y - windowHalfHeight + windowPos.y, wallCenter.z),
        glm::vec3(wallCenter.x + halfWidth, wallCenter.y - halfHeight, wallCenter.z),
        glm::vec3(wallCenter.x - halfWidth, wallCenter.y - halfHeight, wallCenter.z),
        color, normal
    );

    // 3. Left rectangle (left of window)
    addQuad(
        glm::vec3(wallCenter.x - halfWidth, wallCenter.y + windowHalfHeight + windowPos.y, wallCenter.z),
        glm::vec3(wallCenter.x - windowHalfWidth + windowPos.x, wallCenter.y + windowHalfHeight + windowPos.y, wallCenter.z),
        glm::vec3(wallCenter.x - windowHalfWidth + windowPos.x, wallCenter.y - windowHalfHeight + windowPos.y, wallCenter.z),
        glm::vec3(wallCenter.x - halfWidth, wallCenter.y - windowHalfHeight + windowPos.y, wallCenter.z),
        color, normal
    );

    // 4. Right rectangle (right of window)
    addQuad(
        glm::vec3(wallCenter.x + windowHalfWidth + windowPos.x, wallCenter.y + windowHalfHeight + windowPos.y, wallCenter.z),
        glm::vec3(wallCenter.x + halfWidth, wallCenter.y + windowHalfHeight + windowPos.y, wallCenter.z),
        glm::vec3(wallCenter.x + halfWidth, wallCenter.y - windowHalfHeight + windowPos.y, wallCenter.z),
        glm::vec3(wallCenter.x + windowHalfWidth + windowPos.x, wallCenter.y - windowHalfHeight + windowPos.y, wallCenter.z),
        color, normal
    );
}

void Room::generateVertices() {
    vertices.clear();
    indices.clear();

    float halfWidth = roomWidth / 2.0f;
    float halfDepth = roomDepth / 2.0f;

    // Floor (facing up)
    addQuad(
        glm::vec3(-halfWidth, 0.0f, -halfDepth),
        glm::vec3(halfWidth, 0.0f, -halfDepth),
        glm::vec3(halfWidth, 0.0f, halfDepth),
        glm::vec3(-halfWidth, 0.0f, halfDepth),
        floorColor,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    // Ceiling (facing down)
    addQuad(
        glm::vec3(-halfWidth, roomHeight, -halfDepth),
        glm::vec3(halfWidth, roomHeight, -halfDepth),
        glm::vec3(halfWidth, roomHeight, halfDepth),
        glm::vec3(-halfWidth, roomHeight, halfDepth),
        ceilingColor,
        glm::vec3(0.0f, -1.0f, 0.0f)
    );

    // Back wall (blue)
    addQuad(
        glm::vec3(-halfWidth, 0.0f, -halfDepth),
        glm::vec3(halfWidth, 0.0f, -halfDepth),
        glm::vec3(halfWidth, roomHeight, -halfDepth),
        glm::vec3(-halfWidth, roomHeight, -halfDepth),
        wallColors[0],
        glm::vec3(0.0f, 0.0f, 1.0f)
    );

    // Front wall (green) - SOLID
    addQuad(
        glm::vec3(-halfWidth, 0.0f, halfDepth),
        glm::vec3(halfWidth, 0.0f, halfDepth),
        glm::vec3(halfWidth, roomHeight, halfDepth),
        glm::vec3(-halfWidth, roomHeight, halfDepth),
        wallColors[1],
        glm::vec3(0.0f, 0.0f, -1.0f)
    );

    // Left wall (red)
    addQuad(
        glm::vec3(-halfWidth, 0.0f, -halfDepth),
        glm::vec3(-halfWidth, 0.0f, halfDepth),
        glm::vec3(-halfWidth, roomHeight, halfDepth),
        glm::vec3(-halfWidth, roomHeight, -halfDepth),
        wallColors[2],
        glm::vec3(1.0f, 0.0f, 0.0f)
    );

    // Right wall (yellow)
    addQuad(
        glm::vec3(halfWidth, 0.0f, -halfDepth),
        glm::vec3(halfWidth, 0.0f, halfDepth),
        glm::vec3(halfWidth, roomHeight, halfDepth),
        glm::vec3(halfWidth, roomHeight, -halfDepth),
        wallColors[3],
        glm::vec3(-1.0f, 0.0f, 0.0f)
    );

    // INTERIOR WALL WITH WINDOW (if added)
    if (hasInteriorWall) {
        createWallWithWindow(interiorWallPos, interiorWallSize,
            windowPosRelative, interiorWindowSize,
            interiorWallColor, interiorWallNormal);
    }

    needsUpdate = false;
}

void Room::updateBuffers() {
    if (VAO == 0) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
    }

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Room::setup() {
    generateVertices();
    updateBuffers();
}

void Room::draw() {
    if (needsUpdate) {
        generateVertices();
        updateBuffers();
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Room::cleanup() {
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        VAO = VBO = EBO = 0;
    }
}
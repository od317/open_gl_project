#include "room.h"
#include <iostream>

// Constructor definition
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

// Color setter implementations
void Room::setFloorColor(const glm::vec3& color) {
    floorColor = color;
    needsUpdate = true;
}

void Room::setCeilingColor(const glm::vec3& color) {
    ceilingColor = color;
    needsUpdate = true;
}

void Room::setWallColor(int wallIndex, const glm::vec3& color) {
    if (wallIndex >= 0 && wallIndex < 4) {
        wallColors[wallIndex] = color;
        needsUpdate = true;
    }
}

void Room::generateVertices() {
    vertices.clear();
    indices.clear();

    float halfWidth = roomWidth / 2.0f;
    float halfDepth = roomDepth / 2.0f;

    // Helper function to add a quad with proper normals
    auto addQuad = [&](const glm::vec3& p1, const glm::vec3& p2,
        const glm::vec3& p3, const glm::vec3& p4,
        const glm::vec3& color, const glm::vec3& normal) {
            int baseIndex = vertices.size() / 9; // 9 floats per vertex

            // Vertex 1
            vertices.push_back(p1.x); vertices.push_back(p1.y); vertices.push_back(p1.z); // Position
            vertices.push_back(color.r); vertices.push_back(color.g); vertices.push_back(color.b); // Color
            vertices.push_back(normal.x); vertices.push_back(normal.y); vertices.push_back(normal.z); // Normal

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

            // Indices for two triangles
            indices.push_back(baseIndex);
            indices.push_back(baseIndex + 1);
            indices.push_back(baseIndex + 2);
            indices.push_back(baseIndex);
            indices.push_back(baseIndex + 2);
            indices.push_back(baseIndex + 3);
        };

    // Floor (facing up)
    addQuad(
        glm::vec3(-halfWidth, 0.0f, -halfDepth),
        glm::vec3(halfWidth, 0.0f, -halfDepth),
        glm::vec3(halfWidth, 0.0f, halfDepth),
        glm::vec3(-halfWidth, 0.0f, halfDepth),
        floorColor,
        glm::vec3(0.0f, 1.0f, 0.0f)  // Normal pointing up
    );

    // Ceiling (facing down)
    addQuad(
        glm::vec3(-halfWidth, roomHeight, -halfDepth),
        glm::vec3(halfWidth, roomHeight, -halfDepth),
        glm::vec3(halfWidth, roomHeight, halfDepth),
        glm::vec3(-halfWidth, roomHeight, halfDepth),
        ceilingColor,
        glm::vec3(0.0f, -1.0f, 0.0f)  // Normal pointing down
    );

    // Back wall (facing forward/positive Z)
    addQuad(
        glm::vec3(-halfWidth, 0.0f, -halfDepth),
        glm::vec3(halfWidth, 0.0f, -halfDepth),
        glm::vec3(halfWidth, roomHeight, -halfDepth),
        glm::vec3(-halfWidth, roomHeight, -halfDepth),
        wallColors[0],  // Blue
        glm::vec3(0.0f, 0.0f, 1.0f)  // Normal pointing forward
    );

    // Front wall (facing backward/negative Z)
    addQuad(
        glm::vec3(-halfWidth, 0.0f, halfDepth),
        glm::vec3(halfWidth, 0.0f, halfDepth),
        glm::vec3(halfWidth, roomHeight, halfDepth),
        glm::vec3(-halfWidth, roomHeight, halfDepth),
        wallColors[1],  // Green
        glm::vec3(0.0f, 0.0f, -1.0f)  // Normal pointing backward
    );

    // Left wall (facing right/positive X)
    addQuad(
        glm::vec3(-halfWidth, 0.0f, -halfDepth),
        glm::vec3(-halfWidth, 0.0f, halfDepth),
        glm::vec3(-halfWidth, roomHeight, halfDepth),
        glm::vec3(-halfWidth, roomHeight, -halfDepth),
        wallColors[2],  // Red
        glm::vec3(1.0f, 0.0f, 0.0f)  // Normal pointing right
    );

    // Right wall (facing left/negative X)
    addQuad(
        glm::vec3(halfWidth, 0.0f, -halfDepth),
        glm::vec3(halfWidth, 0.0f, halfDepth),
        glm::vec3(halfWidth, roomHeight, halfDepth),
        glm::vec3(halfWidth, roomHeight, -halfDepth),
        wallColors[3],  // Yellow
        glm::vec3(-1.0f, 0.0f, 0.0f)  // Normal pointing left
    );

    needsUpdate = false;
}

void Room::updateBuffers() {
    if (VAO == 0) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
    }

    glBindVertexArray(VAO);

    // Vertex Buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Element Buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Position attribute (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color attribute (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Normal attribute (location = 2)
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
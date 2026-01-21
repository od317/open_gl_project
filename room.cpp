#include "room.h"
#include <iostream>
#include <cmath>

// Constructor - LARGE exhibition hall
Room::Room(float width, float height, float depth)
    : roomWidth(width), roomHeight(height), roomDepth(depth),
    needsUpdate(true) {

    // Modern showroom color scheme - all light gray
    wallColor = glm::vec3(0.95f, 0.95f, 0.96f);     // Very light gray walls
    floorColor = glm::vec3(0.88f, 0.88f, 0.90f);    // Slightly darker floor
    ceilingColor = glm::vec3(0.92f, 0.92f, 0.94f);  // Light gray ceiling
    accentColor = glm::vec3(0.75f, 0.75f, 0.78f);   // Medium gray for details

    // Create large main window by default
    addMainWindow(glm::vec3(0.0f, 4.0f, -14.9f), glm::vec3(25.0f, 8.0f, 0.0f));

    VAO = VBO = EBO = 0;
}

// Add main exhibition window
void Room::addMainWindow(const glm::vec3& position, const glm::vec3& size) {
    hasMainWindow = true;
    windowPos = position;
    windowSize = size;
    windowWallNormal = glm::vec3(0.0f, 0.0f, 1.0f); // Facing forward
    needsUpdate = true;
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

// Helper function to add a triangle
void Room::addTriangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3,
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

    // Add indices for triangle
    indices.push_back(baseIndex);
    indices.push_back(baseIndex + 1);
    indices.push_back(baseIndex + 2);
}

// Create main wall with large exhibition window
void Room::createMainWallWithLargeWindow() {
    float halfWidth = roomWidth / 2.0f;
    float windowHalfWidth = windowSize.x / 2.0f;
    float windowHalfHeight = windowSize.y / 2.0f;
    float windowBottom = windowPos.y - windowHalfHeight;
    float windowTop = windowPos.y + windowHalfHeight;
    float windowLeft = windowPos.x - windowHalfWidth;
    float windowRight = windowPos.x + windowHalfWidth;

    // Back wall (z = -roomDepth/2) with large window
    float wallZ = -roomDepth / 2.0f;

    // 1. Top section (above window)
    addQuad(
        glm::vec3(-halfWidth, roomHeight, wallZ),
        glm::vec3(halfWidth, roomHeight, wallZ),
        glm::vec3(halfWidth, windowTop, wallZ),
        glm::vec3(-halfWidth, windowTop, wallZ),
        wallColor,
        glm::vec3(0.0f, 0.0f, 1.0f)
    );

    // 2. Bottom section (below window)
    addQuad(
        glm::vec3(-halfWidth, windowBottom, wallZ),
        glm::vec3(halfWidth, windowBottom, wallZ),
        glm::vec3(halfWidth, 0.0f, wallZ),
        glm::vec3(-halfWidth, 0.0f, wallZ),
        wallColor,
        glm::vec3(0.0f, 0.0f, 1.0f)
    );

    // 3. Left section (left of window)
    addQuad(
        glm::vec3(-halfWidth, windowTop, wallZ),
        glm::vec3(windowLeft, windowTop, wallZ),
        glm::vec3(windowLeft, windowBottom, wallZ),
        glm::vec3(-halfWidth, windowBottom, wallZ),
        wallColor,
        glm::vec3(0.0f, 0.0f, 1.0f)
    );

    // 4. Right section (right of window)
    addQuad(
        glm::vec3(windowRight, windowTop, wallZ),
        glm::vec3(halfWidth, windowTop, wallZ),
        glm::vec3(halfWidth, windowBottom, wallZ),
        glm::vec3(windowRight, windowBottom, wallZ),
        wallColor,
        glm::vec3(0.0f, 0.0f, 1.0f)
    );
}

// Create architectural columns
void Room::createColumns() {
    float halfWidth = roomWidth / 2.0f;
    float halfDepth = roomDepth / 2.0f;
    float columnRadius = 0.8f;
    int columnSides = 8;
    float columnHeight = roomHeight;

    // Four columns at corners
    glm::vec3 columnPositions[] = {
        glm::vec3(-halfWidth + 2.0f, columnHeight / 2.0f, -halfDepth + 2.0f),
        glm::vec3(halfWidth - 2.0f, columnHeight / 2.0f, -halfDepth + 2.0f),
        glm::vec3(-halfWidth + 2.0f, columnHeight / 2.0f, halfDepth - 2.0f),
        glm::vec3(halfWidth - 2.0f, columnHeight / 2.0f, halfDepth - 2.0f)
    };

    for (const auto& center : columnPositions) {
        // Create column sides (cylinder approximation)
        for (int i = 0; i < columnSides; i++) {
            float angle1 = 2.0f * 3.14159f * i / columnSides;
            float angle2 = 2.0f * 3.14159f * (i + 1) / columnSides;

            glm::vec3 p1(center.x + columnRadius * cos(angle1),
                center.y - columnHeight / 2.0f,
                center.z + columnRadius * sin(angle1));
            glm::vec3 p2(center.x + columnRadius * cos(angle2),
                center.y - columnHeight / 2.0f,
                center.z + columnRadius * sin(angle2));
            glm::vec3 p3(center.x + columnRadius * cos(angle2),
                center.y + columnHeight / 2.0f,
                center.z + columnRadius * sin(angle2));
            glm::vec3 p4(center.x + columnRadius * cos(angle1),
                center.y + columnHeight / 2.0f,
                center.z + columnRadius * sin(angle1));

            glm::vec3 normal = glm::normalize(glm::vec3(cos(angle1), 0.0f, sin(angle1)));

            addQuad(p1, p2, p3, p4, accentColor, normal);
        }

        // Column top cap
        for (int i = 0; i < columnSides; i++) {
            float angle1 = 2.0f * 3.14159f * i / columnSides;
            float angle2 = 2.0f * 3.14159f * (i + 1) / columnSides;

            glm::vec3 centerTop(center.x, center.y + columnHeight / 2.0f, center.z);
            glm::vec3 p1(center.x + columnRadius * cos(angle1),
                center.y + columnHeight / 2.0f,
                center.z + columnRadius * sin(angle1));
            glm::vec3 p2(center.x + columnRadius * cos(angle2),
                center.y + columnHeight / 2.0f,
                center.z + columnRadius * sin(angle2));

            glm::vec3 normal(0.0f, 1.0f, 0.0f);
            addTriangle(centerTop, p1, p2, accentColor, normal);
        }
    }
}

// Create display platforms for cars
void Room::createDisplayPlatforms() {
    float platformHeight = 0.2f;
    float platformWidth = 8.0f;
    float platformDepth = 15.0f;

    // Two large platforms for cars
    glm::vec3 platformPositions[] = {
        glm::vec3(-10.0f, platformHeight / 2.0f, 0.0f),   // Left platform for Porsche
        glm::vec3(10.0f, platformHeight / 2.0f, 0.0f)     // Right platform for Koenigsegg
    };

    for (const auto& center : platformPositions) {
        float halfW = platformWidth / 2.0f;
        float halfD = platformDepth / 2.0f;
        float bottomY = center.y - platformHeight / 2.0f;
        float topY = center.y + platformHeight / 2.0f;

        // Top
        addQuad(
            glm::vec3(center.x - halfW, topY, center.z - halfD),
            glm::vec3(center.x + halfW, topY, center.z - halfD),
            glm::vec3(center.x + halfW, topY, center.z + halfD),
            glm::vec3(center.x - halfW, topY, center.z + halfD),
            floorColor,
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        // Sides
        // Front
        addQuad(
            glm::vec3(center.x - halfW, bottomY, center.z + halfD),
            glm::vec3(center.x + halfW, bottomY, center.z + halfD),
            glm::vec3(center.x + halfW, topY, center.z + halfD),
            glm::vec3(center.x - halfW, topY, center.z + halfD),
            accentColor,
            glm::vec3(0.0f, 0.0f, -1.0f)
        );

        // Back
        addQuad(
            glm::vec3(center.x - halfW, bottomY, center.z - halfD),
            glm::vec3(center.x + halfW, bottomY, center.z - halfD),
            glm::vec3(center.x + halfW, topY, center.z - halfD),
            glm::vec3(center.x - halfW, topY, center.z - halfD),
            accentColor,
            glm::vec3(0.0f, 0.0f, 1.0f)
        );

        // Left
        addQuad(
            glm::vec3(center.x - halfW, bottomY, center.z - halfD),
            glm::vec3(center.x - halfW, bottomY, center.z + halfD),
            glm::vec3(center.x - halfW, topY, center.z + halfD),
            glm::vec3(center.x - halfW, topY, center.z - halfD),
            accentColor,
            glm::vec3(1.0f, 0.0f, 0.0f)
        );

        // Right
        addQuad(
            glm::vec3(center.x + halfW, bottomY, center.z - halfD),
            glm::vec3(center.x + halfW, bottomY, center.z + halfD),
            glm::vec3(center.x + halfW, topY, center.z + halfD),
            glm::vec3(center.x + halfW, topY, center.z - halfD),
            accentColor,
            glm::vec3(-1.0f, 0.0f, 0.0f)
        );
    }
}

// Create entrance arch at the front
void Room::createEntranceArch() {
    float halfWidth = roomWidth / 2.0f;
    float archWidth = 10.0f;
    float archHeight = 8.0f;
    float archDepth = 1.0f;

    // Left pillar
    addQuad(
        glm::vec3(-halfWidth, 0.0f, roomDepth / 2.0f),
        glm::vec3(-halfWidth + archDepth, 0.0f, roomDepth / 2.0f),
        glm::vec3(-halfWidth + archDepth, archHeight, roomDepth / 2.0f),
        glm::vec3(-halfWidth, archHeight, roomDepth / 2.0f),
        accentColor,
        glm::vec3(0.0f, 0.0f, -1.0f)
    );

    // Right pillar
    addQuad(
        glm::vec3(halfWidth - archDepth, 0.0f, roomDepth / 2.0f),
        glm::vec3(halfWidth, 0.0f, roomDepth / 2.0f),
        glm::vec3(halfWidth, archHeight, roomDepth / 2.0f),
        glm::vec3(halfWidth - archDepth, archHeight, roomDepth / 2.0f),
        accentColor,
        glm::vec3(0.0f, 0.0f, -1.0f)
    );

    // Arch top (semi-circle approximation)
    int archSegments = 8;
    for (int i = 0; i < archSegments; i++) {
        float angle1 = 3.14159f * i / archSegments;
        float angle2 = 3.14159f * (i + 1) / archSegments;

        glm::vec3 p1(archWidth / 2 * cos(angle1), archHeight + archWidth / 2 * sin(angle1), roomDepth / 2.0f);
        glm::vec3 p2(archWidth / 2 * cos(angle2), archHeight + archWidth / 2 * sin(angle2), roomDepth / 2.0f);
        glm::vec3 p3(archWidth / 2 * cos(angle2), archHeight + archWidth / 2 * sin(angle2), roomDepth / 2.0f - archDepth);
        glm::vec3 p4(archWidth / 2 * cos(angle1), archHeight + archWidth / 2 * sin(angle1), roomDepth / 2.0f - archDepth);

        addQuad(p1, p2, p3, p4, accentColor, glm::vec3(0.0f, 0.0f, -1.0f));
    }
}

// Generate all vertices for the exhibition hall
void Room::generateVertices() {
    vertices.clear();
    indices.clear();

    float halfWidth = roomWidth / 2.0f;
    float halfDepth = roomDepth / 2.0f;

    // 1. FLOOR (very large)
    addQuad(
        glm::vec3(-halfWidth, 0.0f, -halfDepth),
        glm::vec3(halfWidth, 0.0f, -halfDepth),
        glm::vec3(halfWidth, 0.0f, halfDepth),
        glm::vec3(-halfWidth, 0.0f, halfDepth),
        floorColor,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    // 2. CEILING
    addQuad(
        glm::vec3(-halfWidth, roomHeight, -halfDepth),
        glm::vec3(halfWidth, roomHeight, -halfDepth),
        glm::vec3(halfWidth, roomHeight, halfDepth),
        glm::vec3(-halfWidth, roomHeight, halfDepth),
        ceilingColor,
        glm::vec3(0.0f, -1.0f, 0.0f)
    );

    // 3. LEFT WALL (solid)
    addQuad(
        glm::vec3(-halfWidth, 0.0f, -halfDepth),
        glm::vec3(-halfWidth, 0.0f, halfDepth),
        glm::vec3(-halfWidth, roomHeight, halfDepth),
        glm::vec3(-halfWidth, roomHeight, -halfDepth),
        wallColor,
        glm::vec3(1.0f, 0.0f, 0.0f)
    );

    // 4. RIGHT WALL (solid)
    addQuad(
        glm::vec3(halfWidth, 0.0f, -halfDepth),
        glm::vec3(halfWidth, 0.0f, halfDepth),
        glm::vec3(halfWidth, roomHeight, halfDepth),
        glm::vec3(halfWidth, roomHeight, -halfDepth),
        wallColor,
        glm::vec3(-1.0f, 0.0f, 0.0f)
    );

    // 5. BACK WALL WITH LARGE WINDOW
    if (hasMainWindow) {
        createMainWallWithLargeWindow();
    }
    else {
        // Solid back wall if no window
        addQuad(
            glm::vec3(-halfWidth, 0.0f, -halfDepth),
            glm::vec3(halfWidth, 0.0f, -halfDepth),
            glm::vec3(halfWidth, roomHeight, -halfDepth),
            glm::vec3(-halfWidth, roomHeight, -halfDepth),
            wallColor,
            glm::vec3(0.0f, 0.0f, 1.0f)
        );
    }

    // 6. FRONT WALL WITH ENTRANCE
    createEntranceArch();

    // 7. ARCHITECTURAL COLUMNS
    if (hasColumns) {
        createColumns();
    }

    // 8. DISPLAY PLATFORMS
    if (hasDisplayPlatforms) {
        createDisplayPlatforms();
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
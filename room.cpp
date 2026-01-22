#include "room.h"
#include <iostream>
#include <cmath>

// Constructor - LARGE exhibition hall
Room::Room(float width, float height, float depth)
    : roomWidth(width), roomHeight(height), roomDepth(depth),
    needsUpdate(true) {

    wallColor = glm::vec3(1.0f, 1.0f, 1.0f);     // PURE WHITE walls
    floorColor = glm::vec3(0.95f, 0.95f, 0.95f); // Very light gray floor
    ceilingColor = glm::vec3(1.0f, 1.0f, 1.0f);  // WHITE ceiling
    accentColor = glm::vec3(0.7f, 0.7f, 0.7f);   // Gray for details

    // Create large main window by default
    addMainWindow(glm::vec3(0.0f, 4.0f, -14.9f), glm::vec3(25.0f, 8.0f, 0.0f));

    // Enable side and front windows
    hasSideWindows = true;
    hasFrontWindows = true;

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

    // FIXED: Use wallColor (which should be white) instead of hardcoded colors
    // 1. Top section (above window)
    addQuad(
        glm::vec3(-halfWidth, roomHeight, wallZ),
        glm::vec3(halfWidth, roomHeight, wallZ),
        glm::vec3(halfWidth, windowTop, wallZ),
        glm::vec3(-halfWidth, windowTop, wallZ),
        wallColor,  // Use wallColor instead of hardcoded color
        glm::vec3(0.0f, 0.0f, 1.0f)
    );

    // 2. Bottom section (below window)
    addQuad(
        glm::vec3(-halfWidth, windowBottom, wallZ),
        glm::vec3(halfWidth, windowBottom, wallZ),
        glm::vec3(halfWidth, 0.0f, wallZ),
        glm::vec3(-halfWidth, 0.0f, wallZ),
        wallColor,  // Use wallColor instead of hardcoded color
        glm::vec3(0.0f, 0.0f, 1.0f)
    );

    // 3. Left section (left of window)
    addQuad(
        glm::vec3(-halfWidth, windowTop, wallZ),
        glm::vec3(windowLeft, windowTop, wallZ),
        glm::vec3(windowLeft, windowBottom, wallZ),
        glm::vec3(-halfWidth, windowBottom, wallZ),
        wallColor,  // Use wallColor instead of hardcoded color
        glm::vec3(0.0f, 0.0f, 1.0f)
    );

    // 4. Right section (right of window)
    addQuad(
        glm::vec3(windowRight, windowTop, wallZ),
        glm::vec3(halfWidth, windowTop, wallZ),
        glm::vec3(halfWidth, windowBottom, wallZ),
        glm::vec3(windowRight, windowBottom, wallZ),
        wallColor,  // Use wallColor instead of hardcoded color
        glm::vec3(0.0f, 0.0f, 1.0f)
    );
}

// Helper function to create a wall with multiple windows
void Room::createWallWithWindows(float wallZ, const glm::vec3& normal, bool isFrontWall) {
    float halfWidth = roomWidth / 2.0f;

    // Calculate how many windows fit on the wall
    int numWindows = static_cast<int>((roomWidth - windowSpacing) / windowSpacing);
    if (numWindows < 1) numWindows = 1;

    // Calculate starting position for windows
    float startX = -halfWidth + windowSpacing / 2.0f;

    for (int i = 0; i < numWindows; i++) {
        float windowCenterX = startX + i * windowSpacing;

        // Skip center area for front wall (where entrance is)
        if (isFrontWall && fabs(windowCenterX) < 6.0f) {
            continue;
        }

        // Window dimensions
        float windowBottom = 2.0f;  // Window starts 2m above floor
        float windowTop = windowBottom + windowHeight;
        float windowLeft = windowCenterX - windowWidth / 2.0f;
        float windowRight = windowCenterX + windowWidth / 2.0f;

        // Create wall with window cutout
        // Top section above window
        addQuad(
            glm::vec3(-halfWidth, roomHeight, wallZ),
            glm::vec3(halfWidth, roomHeight, wallZ),
            glm::vec3(halfWidth, windowTop, wallZ),
            glm::vec3(-halfWidth, windowTop, wallZ),
            wallColor,
            normal
        );

        // Bottom section below window
        addQuad(
            glm::vec3(-halfWidth, windowBottom, wallZ),
            glm::vec3(halfWidth, windowBottom, wallZ),
            glm::vec3(halfWidth, 0.0f, wallZ),
            glm::vec3(-halfWidth, 0.0f, wallZ),
            wallColor,
            normal
        );

        // Left section (left of window)
        addQuad(
            glm::vec3(-halfWidth, windowTop, wallZ),
            glm::vec3(windowLeft, windowTop, wallZ),
            glm::vec3(windowLeft, windowBottom, wallZ),
            glm::vec3(-halfWidth, windowBottom, wallZ),
            wallColor,
            normal
        );

        // Right section (right of window)
        addQuad(
            glm::vec3(windowRight, windowTop, wallZ),
            glm::vec3(halfWidth, windowTop, wallZ),
            glm::vec3(halfWidth, windowBottom, wallZ),
            glm::vec3(windowRight, windowBottom, wallZ),
            wallColor,
            normal
        );

        // Middle sections between windows
        if (i > 0) {
            float prevWindowRight = startX + (i - 1) * windowSpacing + windowWidth / 2.0f;
            addQuad(
                glm::vec3(prevWindowRight, windowTop, wallZ),
                glm::vec3(windowLeft, windowTop, wallZ),
                glm::vec3(windowLeft, windowBottom, wallZ),
                glm::vec3(prevWindowRight, windowBottom, wallZ),
                wallColor,
                normal
            );
        }
    }
}

// Create side windows (left and right walls)
void Room::createSideWindows() {
    float halfDepth = roomDepth / 2.0f;

    // LEFT WALL (x = -halfWidth) - with windows
    float leftWallX = -roomWidth / 2.0f;

    // Calculate how many windows fit on the side wall
    int numWindows = static_cast<int>((roomDepth - windowSpacing) / windowSpacing);
    if (numWindows < 1) numWindows = 1;

    float startZ = -halfDepth + windowSpacing / 2.0f;

    for (int i = 0; i < numWindows; i++) {
        float windowCenterZ = startZ + i * windowSpacing;

        // Window dimensions
        float windowBottom = 2.0f;
        float windowTop = windowBottom + windowHeight;
        float windowBack = windowCenterZ - windowWidth / 2.0f;
        float windowFront = windowCenterZ + windowWidth / 2.0f;

        // Create left wall with windows
        // Top section above window
        addQuad(
            glm::vec3(leftWallX, roomHeight, -halfDepth),
            glm::vec3(leftWallX, roomHeight, halfDepth),
            glm::vec3(leftWallX, windowTop, halfDepth),
            glm::vec3(leftWallX, windowTop, -halfDepth),
            wallColor,
            glm::vec3(1.0f, 0.0f, 0.0f)
        );

        // Bottom section below window
        addQuad(
            glm::vec3(leftWallX, windowBottom, -halfDepth),
            glm::vec3(leftWallX, windowBottom, halfDepth),
            glm::vec3(leftWallX, 0.0f, halfDepth),
            glm::vec3(leftWallX, 0.0f, -halfDepth),
            wallColor,
            glm::vec3(1.0f, 0.0f, 0.0f)
        );

        // Front section (front of window)
        addQuad(
            glm::vec3(leftWallX, windowTop, -halfDepth),
            glm::vec3(leftWallX, windowTop, windowBack),
            glm::vec3(leftWallX, windowBottom, windowBack),
            glm::vec3(leftWallX, windowBottom, -halfDepth),
            wallColor,
            glm::vec3(1.0f, 0.0f, 0.0f)
        );

        // Back section (back of window)
        addQuad(
            glm::vec3(leftWallX, windowTop, windowFront),
            glm::vec3(leftWallX, windowTop, halfDepth),
            glm::vec3(leftWallX, windowBottom, halfDepth),
            glm::vec3(leftWallX, windowBottom, windowFront),
            wallColor,
            glm::vec3(1.0f, 0.0f, 0.0f)
        );

        // Middle sections between windows
        if (i > 0) {
            float prevWindowFront = startZ + (i - 1) * windowSpacing + windowWidth / 2.0f;
            addQuad(
                glm::vec3(leftWallX, windowTop, prevWindowFront),
                glm::vec3(leftWallX, windowTop, windowBack),
                glm::vec3(leftWallX, windowBottom, windowBack),
                glm::vec3(leftWallX, windowBottom, prevWindowFront),
                wallColor,
                glm::vec3(1.0f, 0.0f, 0.0f)
            );
        }
    }

    // RIGHT WALL (x = halfWidth) - with windows (similar to left wall but mirrored)
    float rightWallX = roomWidth / 2.0f;

    for (int i = 0; i < numWindows; i++) {
        float windowCenterZ = startZ + i * windowSpacing;

        // Window dimensions
        float windowBottom = 2.0f;
        float windowTop = windowBottom + windowHeight;
        float windowBack = windowCenterZ - windowWidth / 2.0f;
        float windowFront = windowCenterZ + windowWidth / 2.0f;

        // Create right wall with windows
        // Top section above window
        addQuad(
            glm::vec3(rightWallX, roomHeight, -halfDepth),
            glm::vec3(rightWallX, roomHeight, halfDepth),
            glm::vec3(rightWallX, windowTop, halfDepth),
            glm::vec3(rightWallX, windowTop, -halfDepth),
            wallColor,
            glm::vec3(-1.0f, 0.0f, 0.0f)
        );

        // Bottom section below window
        addQuad(
            glm::vec3(rightWallX, windowBottom, -halfDepth),
            glm::vec3(rightWallX, windowBottom, halfDepth),
            glm::vec3(rightWallX, 0.0f, halfDepth),
            glm::vec3(rightWallX, 0.0f, -halfDepth),
            wallColor,
            glm::vec3(-1.0f, 0.0f, 0.0f)
        );

        // Front section (front of window)
        addQuad(
            glm::vec3(rightWallX, windowTop, -halfDepth),
            glm::vec3(rightWallX, windowTop, windowBack),
            glm::vec3(rightWallX, windowBottom, windowBack),
            glm::vec3(rightWallX, windowBottom, -halfDepth),
            wallColor,
            glm::vec3(-1.0f, 0.0f, 0.0f)
        );

        // Back section (back of window)
        addQuad(
            glm::vec3(rightWallX, windowTop, windowFront),
            glm::vec3(rightWallX, windowTop, halfDepth),
            glm::vec3(rightWallX, windowBottom, halfDepth),
            glm::vec3(rightWallX, windowBottom, windowFront),
            wallColor,
            glm::vec3(-1.0f, 0.0f, 0.0f)
        );

        // Middle sections between windows
        if (i > 0) {
            float prevWindowFront = startZ + (i - 1) * windowSpacing + windowWidth / 2.0f;
            addQuad(
                glm::vec3(rightWallX, windowTop, prevWindowFront),
                glm::vec3(rightWallX, windowTop, windowBack),
                glm::vec3(rightWallX, windowBottom, windowBack),
                glm::vec3(rightWallX, windowBottom, prevWindowFront),
                wallColor,
                glm::vec3(-1.0f, 0.0f, 0.0f)
            );
        }
    }
}

// Create front windows (around entrance)
void Room::createFrontWindows() {
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
void Room::createEntranceArch() {
    float halfWidth = roomWidth / 2.0f;
    float halfDepth = roomDepth / 2.0f;
    float wallZ = halfDepth;  // Front wall position

    float doorWidth = 4.0f;       // Individual door width
    float doorHeight = 7.0f;      // Door height
    float windowWidth = 3.0f;     // Window width
    float windowHeight = 4.0f;    // Window height
    float windowY = 4.0f;         // Window base height

    // Create the front wall structure with OPENINGS (no wall behind windows)

    // Left section of wall (left of windows) - FILL THE GAP
    addQuad(
        glm::vec3(-halfWidth, 0.0f, wallZ),
        glm::vec3(-halfWidth + 2.0f, 0.0f, wallZ),
        glm::vec3(-halfWidth + 2.0f, roomHeight, wallZ),
        glm::vec3(-halfWidth, roomHeight, wallZ),
        wallColor,
        glm::vec3(0.0f, 0.0f, -1.0f)
    );

    // Right section of wall (right of windows) - FILL THE GAP
    addQuad(
        glm::vec3(halfWidth - 2.0f, 0.0f, wallZ),
        glm::vec3(halfWidth, 0.0f, wallZ),
        glm::vec3(halfWidth, roomHeight, wallZ),
        glm::vec3(halfWidth - 2.0f, roomHeight, wallZ),
        wallColor,
        glm::vec3(0.0f, 0.0f, -1.0f)
    );

    // Top section above everything
    addQuad(
        glm::vec3(-halfWidth, roomHeight, wallZ),
        glm::vec3(halfWidth, roomHeight, wallZ),
        glm::vec3(halfWidth, roomHeight - 2.0f, wallZ),
        glm::vec3(-halfWidth, roomHeight - 2.0f, wallZ),
        wallColor,
        glm::vec3(0.0f, 0.0f, -1.0f)
    );

    // Fill the space above windows and below ceiling - IMPORTANT!
    // But ONLY above the wall sections, not above windows
    addQuad(
        glm::vec3(-halfWidth, roomHeight - 2.0f, wallZ),
        glm::vec3(-halfWidth + 2.0f, roomHeight - 2.0f, wallZ),
        glm::vec3(-halfWidth + 2.0f, windowY + windowHeight, wallZ),
        glm::vec3(-halfWidth, windowY + windowHeight, wallZ),
        wallColor,
        glm::vec3(0.0f, 0.0f, -1.0f)
    );

    addQuad(
        glm::vec3(halfWidth - 2.0f, roomHeight - 2.0f, wallZ),
        glm::vec3(halfWidth, roomHeight - 2.0f, wallZ),
        glm::vec3(halfWidth, windowY + windowHeight, wallZ),
        glm::vec3(halfWidth - 2.0f, windowY + windowHeight, wallZ),
        wallColor,
        glm::vec3(0.0f, 0.0f, -1.0f)
    );

    // Fill area below doors (just the strip at the bottom)
    addQuad(
        glm::vec3(-doorWidth - 1.0f, 0.0f, wallZ),
        glm::vec3(doorWidth + 1.0f, 0.0f, wallZ),
        glm::vec3(doorWidth + 1.0f, 0.5f, wallZ), // Small strip at bottom
        glm::vec3(-doorWidth - 1.0f, 0.5f, wallZ),
        wallColor,
        glm::vec3(0.0f, 0.0f, -1.0f)
    );

    // Fill area below windows (left side - between wall and first window)
    addQuad(
        glm::vec3(-halfWidth + 2.0f, 0.0f, wallZ),
        glm::vec3(-12.0f + 1.5f, 0.0f, wallZ),
        glm::vec3(-12.0f + 1.5f, windowY, wallZ),
        glm::vec3(-halfWidth + 2.0f, windowY, wallZ),
        wallColor,
        glm::vec3(0.0f, 0.0f, -1.0f)
    );

    // Fill area below windows (right side - between wall and first window)
    addQuad(
        glm::vec3(12.0f - 1.5f, 0.0f, wallZ),
        glm::vec3(halfWidth - 2.0f, 0.0f, wallZ),
        glm::vec3(halfWidth - 2.0f, windowY, wallZ),
        glm::vec3(12.0f - 1.5f, windowY, wallZ),
        wallColor,
        glm::vec3(0.0f, 0.0f, -1.0f)
    );

    // Fill area between windows and doors (left side)
    addQuad(
        glm::vec3(-12.0f + 1.5f, 0.0f, wallZ),
        glm::vec3(-doorWidth - 1.0f, 0.0f, wallZ),
        glm::vec3(-doorWidth - 1.0f, windowY, wallZ),
        glm::vec3(-12.0f + 1.5f, windowY, wallZ),
        wallColor,
        glm::vec3(0.0f, 0.0f, -1.0f)
    );

    // Fill area between windows and doors (right side)
    addQuad(
        glm::vec3(doorWidth + 1.0f, 0.0f, wallZ),
        glm::vec3(12.0f - 1.5f, 0.0f, wallZ),
        glm::vec3(12.0f - 1.5f, windowY, wallZ),
        glm::vec3(doorWidth + 1.0f, windowY, wallZ),
        wallColor,
        glm::vec3(0.0f, 0.0f, -1.0f)
    );

    // Fill vertical strips BETWEEN windows (not behind them)
    // Between left windows
    addQuad(
        glm::vec3(-8.0f, windowY, wallZ),
        glm::vec3(-4.0f, windowY, wallZ),
        glm::vec3(-4.0f, windowY + windowHeight, wallZ),
        glm::vec3(-8.0f, windowY + windowHeight, wallZ),
        wallColor,
        glm::vec3(0.0f, 0.0f, -1.0f)
    );

    // Between right windows
    addQuad(
        glm::vec3(4.0f, windowY, wallZ),
        glm::vec3(8.0f, windowY, wallZ),
        glm::vec3(8.0f, windowY + windowHeight, wallZ),
        glm::vec3(4.0f, windowY + windowHeight, wallZ),
        wallColor,
        glm::vec3(0.0f, 0.0f, -1.0f)
    );

    // Fill area above doors between windows (the horizontal strip)
    addQuad(
        glm::vec3(-doorWidth - 1.0f, doorHeight, wallZ),
        glm::vec3(doorWidth + 1.0f, doorHeight, wallZ),
        glm::vec3(doorWidth + 1.0f, windowY, wallZ),
        glm::vec3(-doorWidth - 1.0f, windowY, wallZ),
        wallColor,
        glm::vec3(0.0f, 0.0f, -1.0f)
    );

    // REMOVED: The wall sections behind the windows (NO MORE SOLID WALLS BEHIND WINDOWS!)
    // The window areas should be completely open

    // Create window frames ONLY (no background wall)
    // Left side windows (2 windows)
    for (int i = 0; i < 2; i++) {
        float windowLeft = -12.0f + i * 8.0f - windowWidth / 2.0f;
        float windowRight = windowLeft + windowWidth;

        // Window frame top (thin strip)
        addQuad(
            glm::vec3(windowLeft - 0.1f, windowY + windowHeight, wallZ),
            glm::vec3(windowRight + 0.1f, windowY + windowHeight, wallZ),
            glm::vec3(windowRight + 0.1f, windowY + windowHeight + 0.2f, wallZ),
            glm::vec3(windowLeft - 0.1f, windowY + windowHeight + 0.2f, wallZ),
            accentColor,
            glm::vec3(0.0f, 0.0f, -1.0f)
        );

        // Window frame bottom (thin strip)
        addQuad(
            glm::vec3(windowLeft - 0.1f, windowY - 0.2f, wallZ),
            glm::vec3(windowRight + 0.1f, windowY - 0.2f, wallZ),
            glm::vec3(windowRight + 0.1f, windowY, wallZ),
            glm::vec3(windowLeft - 0.1f, windowY, wallZ),
            accentColor,
            glm::vec3(0.0f, 0.0f, -1.0f)
        );

        // Window frame left (thin strip)
        addQuad(
            glm::vec3(windowLeft - 0.2f, windowY, wallZ),
            glm::vec3(windowLeft, windowY, wallZ),
            glm::vec3(windowLeft, windowY + windowHeight, wallZ),
            glm::vec3(windowLeft - 0.2f, windowY + windowHeight, wallZ),
            accentColor,
            glm::vec3(0.0f, 0.0f, -1.0f)
        );

        // Window frame right (thin strip)
        addQuad(
            glm::vec3(windowRight, windowY, wallZ),
            glm::vec3(windowRight + 0.2f, windowY, wallZ),
            glm::vec3(windowRight + 0.2f, windowY + windowHeight, wallZ),
            glm::vec3(windowRight, windowY + windowHeight, wallZ),
            accentColor,
            glm::vec3(0.0f, 0.0f, -1.0f)
        );
    }

    // Right side windows (2 windows)
    for (int i = 0; i < 2; i++) {
        float windowLeft = 12.0f - i * 8.0f - windowWidth / 2.0f;
        float windowRight = windowLeft + windowWidth;

        // Window frame top (thin strip)
        addQuad(
            glm::vec3(windowLeft - 0.1f, windowY + windowHeight, wallZ),
            glm::vec3(windowRight + 0.1f, windowY + windowHeight, wallZ),
            glm::vec3(windowRight + 0.1f, windowY + windowHeight + 0.2f, wallZ),
            glm::vec3(windowLeft - 0.1f, windowY + windowHeight + 0.2f, wallZ),
            accentColor,
            glm::vec3(0.0f, 0.0f, -1.0f)
        );

        // Window frame bottom (thin strip)
        addQuad(
            glm::vec3(windowLeft - 0.1f, windowY - 0.2f, wallZ),
            glm::vec3(windowRight + 0.1f, windowY - 0.2f, wallZ),
            glm::vec3(windowRight + 0.1f, windowY, wallZ),
            glm::vec3(windowLeft - 0.1f, windowY, wallZ),
            accentColor,
            glm::vec3(0.0f, 0.0f, -1.0f)
        );

        // Window frame left (thin strip)
        addQuad(
            glm::vec3(windowLeft - 0.2f, windowY, wallZ),
            glm::vec3(windowLeft, windowY, wallZ),
            glm::vec3(windowLeft, windowY + windowHeight, wallZ),
            glm::vec3(windowLeft - 0.2f, windowY + windowHeight, wallZ),
            accentColor,
            glm::vec3(0.0f, 0.0f, -1.0f)
        );

        // Window frame right (thin strip)
        addQuad(
            glm::vec3(windowRight, windowY, wallZ),
            glm::vec3(windowRight + 0.2f, windowY, wallZ),
            glm::vec3(windowRight + 0.2f, windowY + windowHeight, wallZ),
            glm::vec3(windowRight, windowY + windowHeight, wallZ),
            accentColor,
            glm::vec3(0.0f, 0.0f, -1.0f)
        );
    }

    // Create door frames (just frames, no solid wall)
    // Left door frame top
    addQuad(
        glm::vec3(-doorWidth - 1.0f, doorHeight, wallZ),
        glm::vec3(-1.0f, doorHeight, wallZ),
        glm::vec3(-1.0f, doorHeight + 0.2f, wallZ),
        glm::vec3(-doorWidth - 1.0f, doorHeight + 0.2f, wallZ),
        accentColor,
        glm::vec3(0.0f, 0.0f, -1.0f)
    );

    // Right door frame top
    addQuad(
        glm::vec3(1.0f, doorHeight, wallZ),
        glm::vec3(doorWidth + 1.0f, doorHeight, wallZ),
        glm::vec3(doorWidth + 1.0f, doorHeight + 0.2f, wallZ),
        glm::vec3(1.0f, doorHeight + 0.2f, wallZ),
        accentColor,
        glm::vec3(0.0f, 0.0f, -1.0f)
    );

    // Door frames vertical (between doors)
    addQuad(
        glm::vec3(-1.0f, 0.5f, wallZ), // Start above the bottom strip
        glm::vec3(1.0f, 0.5f, wallZ),
        glm::vec3(1.0f, doorHeight + 0.2f, wallZ),
        glm::vec3(-1.0f, doorHeight + 0.2f, wallZ),
        accentColor,
        glm::vec3(0.0f, 0.0f, -1.0f)
    );

    // Left door vertical frame
    addQuad(
        glm::vec3(-doorWidth - 1.0f, 0.5f, wallZ), // Start above the bottom strip
        glm::vec3(-doorWidth - 0.8f, 0.5f, wallZ),
        glm::vec3(-doorWidth - 0.8f, doorHeight + 0.2f, wallZ),
        glm::vec3(-doorWidth - 1.0f, doorHeight + 0.2f, wallZ),
        accentColor,
        glm::vec3(0.0f, 0.0f, -1.0f)
    );

    // Right door vertical frame
    addQuad(
        glm::vec3(doorWidth + 0.8f, 0.5f, wallZ), // Start above the bottom strip
        glm::vec3(doorWidth + 1.0f, 0.5f, wallZ),
        glm::vec3(doorWidth + 1.0f, doorHeight + 0.2f, wallZ),
        glm::vec3(doorWidth + 0.8f, doorHeight + 0.2f, wallZ),
        accentColor,
        glm::vec3(0.0f, 0.0f, -1.0f)
    );
}

// Generate all vertices for the exhibition hall - FIXED
void Room::generateVertices() {
    vertices.clear();
    indices.clear();

    float halfWidth = roomWidth / 2.0f;
    float halfDepth = roomDepth / 2.0f;

    // 1. FLOOR
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

    // 3. LEFT WALL - with windows if enabled
    if (hasSideWindows) {
        createSideWindows();
    }
    else {
        // Solid left wall if no windows
        addQuad(
            glm::vec3(-halfWidth, 0.0f, -halfDepth),
            glm::vec3(-halfWidth, 0.0f, halfDepth),
            glm::vec3(-halfWidth, roomHeight, halfDepth),
            glm::vec3(-halfWidth, roomHeight, -halfDepth),
            wallColor,
            glm::vec3(1.0f, 0.0f, 0.0f)
        );
    }

    // 4. RIGHT WALL - with windows if enabled
    if (hasSideWindows) {
        // Already created in createSideWindows()
    }
    else {
        // Solid right wall if no windows
        addQuad(
            glm::vec3(halfWidth, 0.0f, -halfDepth),
            glm::vec3(halfWidth, 0.0f, halfDepth),
            glm::vec3(halfWidth, roomHeight, halfDepth),
            glm::vec3(halfWidth, roomHeight, -halfDepth),
            wallColor,
            glm::vec3(-1.0f, 0.0f, 0.0f)
        );
    }

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

    // 6. FRONT WALL WITH ENTRANCE AND WINDOWS
    createEntranceArch();

    // Add front windows if enabled
    std::cout << "DEBUG: hasFrontWindows = " << hasFrontWindows << std::endl;  // Add this!

    // Add front windows if enabled
    if (hasFrontWindows) {
        std::cout << "DEBUG: Calling createFrontWindows()" << std::endl;  // Add this!
        createFrontWindows();
    }

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


void Room::cleanup() {
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        VAO = VBO = EBO = 0;
    }
}


// Create door frames for the entrance
void Room::createDoorFrames() {
    float wallZ = roomDepth / 2.0f;
    float doorWidth = 4.0f;
    float doorHeight = 7.0f;

    // Left door frame (thick frame around door opening)
    addQuad(
        glm::vec3(-doorWidth - 1.5f, doorHeight, wallZ),
        glm::vec3(-0.5f, doorHeight, wallZ),
        glm::vec3(-0.5f, 0.0f, wallZ),
        glm::vec3(-doorWidth - 1.5f, 0.0f, wallZ),
        accentColor,
        glm::vec3(0.0f, 0.0f, -1.0f)
    );

    // Right door frame
    addQuad(
        glm::vec3(0.5f, doorHeight, wallZ),
        glm::vec3(doorWidth + 1.5f, doorHeight, wallZ),
        glm::vec3(doorWidth + 1.5f, 0.0f, wallZ),
        glm::vec3(0.5f, 0.0f, wallZ),
        accentColor,
        glm::vec3(0.0f, 0.0f, -1.0f)
    );

    // Top frame connecting both sides
    addQuad(
        glm::vec3(-doorWidth - 1.5f, doorHeight, wallZ),
        glm::vec3(doorWidth + 1.5f, doorHeight, wallZ),
        glm::vec3(doorWidth + 1.5f, doorHeight - 0.2f, wallZ),
        glm::vec3(-doorWidth - 1.5f, doorHeight - 0.2f, wallZ),
        accentColor,
        glm::vec3(0.0f, 0.0f, -1.0f)
    );

    // Create the doors themselves
    createDoors();
}

// Create the actual doors
void Room::createDoors() {
    float wallZ = roomDepth / 2.0f - 0.01f; // Slightly in front of the wall
    float doorWidth = 4.0f;
    float doorHeight = 7.0f;

    // Apply door rotation for animation
    float leftDoorAngle = glm::radians(doorAngle);
    float rightDoorAngle = glm::radians(-doorAngle); // Opposite direction

    // Left door
    glm::vec3 leftDoorHinge(-doorWidth / 2 - 0.75f, 0.0f, wallZ);

    // Calculate left door corners based on rotation
    //glm::mat4 leftRotation = glm::rotate(glm::mat4(1.0f), leftDoorAngle, glm::vec3(0.0f, 1.0f, 0.0f));

    glm::vec3 leftBottomLeft = leftDoorHinge;
    glm::vec3 leftBottomRight = leftDoorHinge + glm::vec3(doorWidth, 0.0f, 0.0f);
    glm::vec3 leftTopRight = leftDoorHinge + glm::vec3(doorWidth, doorHeight, 0.0f);
    glm::vec3 leftTopLeft = leftDoorHinge + glm::vec3(0.0f, doorHeight, 0.0f);

    // Apply rotation around hinge point
    //leftBottomRight = glm::vec3(leftRotation * glm::vec4(leftBottomRight - leftDoorHinge, 1.0f)) + leftDoorHinge;
    //leftTopRight = glm::vec3(leftRotation * glm::vec4(leftTopRight - leftDoorHinge, 1.0f)) + leftDoorHinge;
    //leftTopLeft = glm::vec3(leftRotation * glm::vec4(leftTopLeft - leftDoorHinge, 1.0f)) + leftDoorHinge;

    // Draw left door (dark wood color)
    glm::vec3 doorColor = glm::vec3(0.5f, 0.35f, 0.25f); // Dark wood
    addQuad(leftBottomLeft, leftBottomRight, leftTopRight, leftTopLeft,
        doorColor, glm::vec3(0.0f, 0.0f, -1.0f));

    // Right door
    glm::vec3 rightDoorHinge(doorWidth / 2 + 0.75f, 0.0f, wallZ);

    // Calculate right door corners based on rotation
    //glm::mat4 rightRotation = glm::rotate(glm::mat4(1.0f), rightDoorAngle, glm::vec3(0.0f, 1.0f, 0.0f));

    glm::vec3 rightBottomLeft = rightDoorHinge + glm::vec3(-doorWidth, 0.0f, 0.0f);
    glm::vec3 rightBottomRight = rightDoorHinge;
    glm::vec3 rightTopRight = rightDoorHinge + glm::vec3(0.0f, doorHeight, 0.0f);
    glm::vec3 rightTopLeft = rightDoorHinge + glm::vec3(-doorWidth, doorHeight, 0.0f);

    // Apply rotation around hinge point
    //rightBottomLeft = glm::vec3(rightRotation * glm::vec4(rightBottomLeft - rightDoorHinge, 1.0f)) + rightDoorHinge;
    //rightTopLeft = glm::vec3(rightRotation * glm::vec4(rightTopLeft - rightDoorHinge, 1.0f)) + rightDoorHinge;
    //rightTopRight = glm::vec3(rightRotation * glm::vec4(rightTopRight - rightDoorHinge, 1.0f)) + rightDoorHinge;

    // Draw right door
    addQuad(rightBottomLeft, rightBottomRight, rightTopRight, rightTopLeft,
        doorColor, glm::vec3(0.0f, 0.0f, -1.0f));
}



// Door controls
void Room::toggleDoors() {
    doorsOpen = !doorsOpen;
}

void Room::updateDoors(float deltaTime) {
    float targetAngle = doorsOpen ? 90.0f : 0.0f;
    float speed = 60.0f; // degrees per second

    if (doorAngle < targetAngle) {
        doorAngle += speed * deltaTime;
        if (doorAngle > targetAngle) doorAngle = targetAngle;
    }
    else if (doorAngle > targetAngle) {
        doorAngle -= speed * deltaTime;
        if (doorAngle < targetAngle) doorAngle = targetAngle;
    }

    // Regenerate vertices if door angle changed
    if (fabs(doorAngle - targetAngle) > 0.1f) {
        needsUpdate = true;
    }
}

void Room::setDoorsOpen(bool open) {
    doorsOpen = open;
}
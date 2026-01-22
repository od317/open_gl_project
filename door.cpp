#include "door.h"
#include <iostream>
#include <vector>

Door::Door(glm::vec3 pos, glm::vec3 sz, bool leftDoor)
    : position(pos), size(sz), isLeftDoor(leftDoor) {
    rotationAngle = 0.0f;
    isOpen = false;

    // Dark wood color for doors
    doorColor = glm::vec3(0.5f, 0.35f, 0.25f);
    handleColor = glm::vec3(0.9f, 0.9f, 0.9f); // Silver handles
}

Door::~Door() {
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
}

void Door::setup() {
    // Create door vertices (a simple rectangle with thickness)
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    float halfWidth = size.x / 2.0f;
    float halfHeight = size.y / 2.0f;
    float thickness = size.z / 2.0f;

    // Define the 8 vertices of the door (with thickness)
    glm::vec3 verts[8] = {
        // Front face
        glm::vec3(-halfWidth, -halfHeight, thickness),
        glm::vec3(halfWidth, -halfHeight, thickness),
        glm::vec3(halfWidth, halfHeight, thickness),
        glm::vec3(-halfWidth, halfHeight, thickness),

        // Back face
        glm::vec3(-halfWidth, -halfHeight, -thickness),
        glm::vec3(halfWidth, -halfHeight, -thickness),
        glm::vec3(halfWidth, halfHeight, -thickness),
        glm::vec3(-halfWidth, halfHeight, -thickness)
    };

    // Define indices for 6 faces (12 triangles)
    unsigned int faceIndices[36] = {
        // Front face
        0, 1, 2, 2, 3, 0,
        // Back face
        5, 4, 7, 7, 6, 5,
        // Top face
        3, 2, 6, 6, 7, 3,
        // Bottom face
        4, 5, 1, 1, 0, 4,
        // Left face
        4, 0, 3, 3, 7, 4,
        // Right face
        1, 5, 6, 6, 2, 1
    };

    // Create vertex data with position, color, and normal
    for (int i = 0; i < 8; i++) {
        // Position
        vertices.push_back(verts[i].x);
        vertices.push_back(verts[i].y);
        vertices.push_back(verts[i].z);

        // Color (door color)
        vertices.push_back(doorColor.r);
        vertices.push_back(doorColor.g);
        vertices.push_back(doorColor.b);

        // Calculate normal based on face
        glm::vec3 normal(0.0f, 0.0f, 1.0f); // Default front normal
        if (i >= 4) normal = glm::vec3(0.0f, 0.0f, -1.0f); // Back face

        vertices.push_back(normal.x);
        vertices.push_back(normal.y);
        vertices.push_back(normal.z);
    }

    // Set up OpenGL buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(unsigned int), faceIndices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Normal attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Door::draw(Shader& shader) {
    // Calculate transformation matrix
    glm::mat4 model = glm::mat4(1.0f);

    // Move to door position
    model = glm::translate(model, position);

    // Apply rotation for door opening
    float actualRotation = isLeftDoor ? rotationAngle : -rotationAngle;
    model = glm::rotate(model, glm::radians(actualRotation), glm::vec3(0.0f, 1.0f, 0.0f));

    // For left door, rotate around left edge, for right door rotate around right edge
    if (isLeftDoor) {
        model = glm::translate(model, glm::vec3(size.x / 2.0f, 0.0f, 0.0f));
    }
    else {
        model = glm::translate(model, glm::vec3(-size.x / 2.0f, 0.0f, 0.0f));
    }

    shader.setMat4("model", model);

    // Draw the door
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Door::update(float deltaTime) {
    float targetAngle = isOpen ? 90.0f : 0.0f;
    float speed = 60.0f; // degrees per second

    if (rotationAngle < targetAngle) {
        rotationAngle += speed * deltaTime;
        if (rotationAngle > targetAngle) rotationAngle = targetAngle;
    }
    else if (rotationAngle > targetAngle) {
        rotationAngle -= speed * deltaTime;
        if (rotationAngle < targetAngle) rotationAngle = targetAngle;
    }
}

void Door::open() {
    isOpen = true;
}

void Door::close() {
    isOpen = false;
}

void Door::toggle() {
    isOpen = !isOpen;
}
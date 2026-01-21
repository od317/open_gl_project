// street.cpp
#include "street.h"
#include <iostream>
#include <cmath>

// Street class implementation
Street::Street(const glm::vec3& pos, const glm::vec3& dim, const glm::vec3& col)
    : position(pos), size(dim), color(col), VAO(0), VBO(0), EBO(0) {
    generateVertices();
    setup();
}

Street::~Street() {
    cleanup();
}

void Street::generateVertices() {
    vertices.clear();
    indices.clear();

    float halfWidth = size.x / 2.0f;
    float halfLength = size.z / 2.0f;
    float thickness = size.y;

    // Street main surface (asphalt)
    glm::vec3 asphaltColor = color;

    // Top surface
    addQuad(
        glm::vec3(-halfWidth, thickness, -halfLength),
        glm::vec3(halfWidth, thickness, -halfLength),
        glm::vec3(halfWidth, thickness, halfLength),
        glm::vec3(-halfWidth, thickness, halfLength),
        asphaltColor,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    // Side edges (curbs)
    glm::vec3 curbColor = glm::vec3(0.7f, 0.7f, 0.7f);
    float curbHeight = thickness + 0.1f;
    float curbWidth = 0.3f;

    // Left curb
    addQuad(
        glm::vec3(-halfWidth - curbWidth, thickness, -halfLength),
        glm::vec3(-halfWidth, thickness, -halfLength),
        glm::vec3(-halfWidth, curbHeight, -halfLength),
        glm::vec3(-halfWidth - curbWidth, curbHeight, -halfLength),
        curbColor,
        glm::vec3(0.0f, 0.0f, 1.0f)
    );

    addQuad(
        glm::vec3(-halfWidth - curbWidth, thickness, halfLength),
        glm::vec3(-halfWidth, thickness, halfLength),
        glm::vec3(-halfWidth, curbHeight, halfLength),
        glm::vec3(-halfWidth - curbWidth, curbHeight, halfLength),
        curbColor,
        glm::vec3(0.0f, 0.0f, -1.0f)
    );

    addQuad(
        glm::vec3(-halfWidth - curbWidth, thickness, -halfLength),
        glm::vec3(-halfWidth - curbWidth, thickness, halfLength),
        glm::vec3(-halfWidth - curbWidth, curbHeight, halfLength),
        glm::vec3(-halfWidth - curbWidth, curbHeight, -halfLength),
        curbColor,
        glm::vec3(1.0f, 0.0f, 0.0f)
    );

    // Right curb
    addQuad(
        glm::vec3(halfWidth, thickness, -halfLength),
        glm::vec3(halfWidth + curbWidth, thickness, -halfLength),
        glm::vec3(halfWidth + curbWidth, curbHeight, -halfLength),
        glm::vec3(halfWidth, curbHeight, -halfLength),
        curbColor,
        glm::vec3(0.0f, 0.0f, 1.0f)
    );

    addQuad(
        glm::vec3(halfWidth, thickness, halfLength),
        glm::vec3(halfWidth + curbWidth, thickness, halfLength),
        glm::vec3(halfWidth + curbWidth, curbHeight, halfLength),
        glm::vec3(halfWidth, curbHeight, halfLength),
        curbColor,
        glm::vec3(0.0f, 0.0f, -1.0f)
    );

    addQuad(
        glm::vec3(halfWidth + curbWidth, thickness, -halfLength),
        glm::vec3(halfWidth + curbWidth, thickness, halfLength),
        glm::vec3(halfWidth + curbWidth, curbHeight, halfLength),
        glm::vec3(halfWidth + curbWidth, curbHeight, -halfLength),
        curbColor,
        glm::vec3(-1.0f, 0.0f, 0.0f)
    );

    // Road markings (lane dividers)
    glm::vec3 lineColor = glm::vec3(1.0f, 1.0f, 1.0f);
    float lineThickness = 0.02f;
    float lineLength = 2.0f;
    float lineGap = 3.0f;
    float lineHeight = thickness + 0.01f;

    // Create dashed lines down the center
    int numLines = static_cast<int>(size.z / (lineLength + lineGap));
    for (int i = 0; i < numLines; i++) {
        float zPos = -halfLength + static_cast<float>(i) * (lineLength + lineGap);

        addQuad(
            glm::vec3(-0.05f, lineHeight, zPos),
            glm::vec3(0.05f, lineHeight, zPos),
            glm::vec3(0.05f, lineHeight, zPos + lineLength),
            glm::vec3(-0.05f, lineHeight, zPos + lineLength),
            lineColor,
            glm::vec3(0.0f, 1.0f, 0.0f)
        );
    }

    // Sidewalk
    glm::vec3 sidewalkColor = glm::vec3(0.8f, 0.8f, 0.8f);
    float sidewalkWidth = 2.0f;
    float sidewalkHeight = curbHeight + 0.05f;

    // Left sidewalk
    addQuad(
        glm::vec3(-halfWidth - curbWidth - sidewalkWidth, sidewalkHeight, -halfLength),
        glm::vec3(-halfWidth - curbWidth, sidewalkHeight, -halfLength),
        glm::vec3(-halfWidth - curbWidth, sidewalkHeight, halfLength),
        glm::vec3(-halfWidth - curbWidth - sidewalkWidth, sidewalkHeight, halfLength),
        sidewalkColor,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    // Right sidewalk
    addQuad(
        glm::vec3(halfWidth + curbWidth, sidewalkHeight, -halfLength),
        glm::vec3(halfWidth + curbWidth + sidewalkWidth, sidewalkHeight, -halfLength),
        glm::vec3(halfWidth + curbWidth + sidewalkWidth, sidewalkHeight, halfLength),
        glm::vec3(halfWidth + curbWidth, sidewalkHeight, halfLength),
        sidewalkColor,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
}

void Street::addQuad(const glm::vec3& p1, const glm::vec3& p2,
    const glm::vec3& p3, const glm::vec3& p4,
    const glm::vec3& color, const glm::vec3& normal) {
    size_t baseIndex = vertices.size() / 9;

    // Apply position offset
    glm::vec3 pos1 = position + p1;
    glm::vec3 pos2 = position + p2;
    glm::vec3 pos3 = position + p3;
    glm::vec3 pos4 = position + p4;

    // Vertex 1
    vertices.push_back(pos1.x); vertices.push_back(pos1.y); vertices.push_back(pos1.z);
    vertices.push_back(color.r); vertices.push_back(color.g); vertices.push_back(color.b);
    vertices.push_back(normal.x); vertices.push_back(normal.y); vertices.push_back(normal.z);

    // Vertex 2
    vertices.push_back(pos2.x); vertices.push_back(pos2.y); vertices.push_back(pos2.z);
    vertices.push_back(color.r); vertices.push_back(color.g); vertices.push_back(color.b);
    vertices.push_back(normal.x); vertices.push_back(normal.y); vertices.push_back(normal.z);

    // Vertex 3
    vertices.push_back(pos3.x); vertices.push_back(pos3.y); vertices.push_back(pos3.z);
    vertices.push_back(color.r); vertices.push_back(color.g); vertices.push_back(color.b);
    vertices.push_back(normal.x); vertices.push_back(normal.y); vertices.push_back(normal.z);

    // Vertex 4
    vertices.push_back(pos4.x); vertices.push_back(pos4.y); vertices.push_back(pos4.z);
    vertices.push_back(color.r); vertices.push_back(color.g); vertices.push_back(color.b);
    vertices.push_back(normal.x); vertices.push_back(normal.y); vertices.push_back(normal.z);

    // Add indices for two triangles
    indices.push_back(static_cast<unsigned int>(baseIndex));
    indices.push_back(static_cast<unsigned int>(baseIndex + 1));
    indices.push_back(static_cast<unsigned int>(baseIndex + 2));
    indices.push_back(static_cast<unsigned int>(baseIndex));
    indices.push_back(static_cast<unsigned int>(baseIndex + 2));
    indices.push_back(static_cast<unsigned int>(baseIndex + 3));
}

void Street::setup() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    updateBuffers();
}

void Street::updateBuffers() {
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

void Street::draw() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Street::cleanup() {
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        VAO = VBO = EBO = 0;
    }
}

// StreetLight class implementation
StreetLight::StreetLight(const glm::vec3& pos, float h)
    : position(pos), height(h), VAO(0), VBO(0), EBO(0) {
    createModel();
    setup();
}

StreetLight::~StreetLight() {
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
}

void StreetLight::createModel() {
    vertices.clear();
    indices.clear();

    glm::vec3 poleColor = glm::vec3(0.4f, 0.4f, 0.45f);
    glm::vec3 lightColor = glm::vec3(0.9f, 0.9f, 0.7f);

    // Create pole (cylinder)
    float poleRadius = 0.1f;
    int segments = 8;

    for (int i = 0; i < segments; i++) {
        float angle1 = 2.0f * 3.14159f * static_cast<float>(i) / static_cast<float>(segments);
        float angle2 = 2.0f * 3.14159f * static_cast<float>(i + 1) / static_cast<float>(segments);

        glm::vec3 p1(poleRadius * cos(angle1), 0.0f, poleRadius * sin(angle1));
        glm::vec3 p2(poleRadius * cos(angle2), 0.0f, poleRadius * sin(angle2));
        glm::vec3 p3(poleRadius * cos(angle2), height * 0.7f, poleRadius * sin(angle2));
        glm::vec3 p4(poleRadius * cos(angle1), height * 0.7f, poleRadius * sin(angle1));

        glm::vec3 normal = glm::normalize(glm::vec3(cos(angle1), 0.0f, sin(angle1)));

        // Add quad for pole segment
        size_t base = vertices.size() / 9;
        vertices.insert(vertices.end(), {
            p1.x, p1.y, p1.z, poleColor.r, poleColor.g, poleColor.b, normal.x, normal.y, normal.z,
            p2.x, p2.y, p2.z, poleColor.r, poleColor.g, poleColor.b, normal.x, normal.y, normal.z,
            p3.x, p3.y, p3.z, poleColor.r, poleColor.g, poleColor.b, normal.x, normal.y, normal.z,
            p4.x, p4.y, p4.z, poleColor.r, poleColor.g, poleColor.b, normal.x, normal.y, normal.z
            });

        indices.push_back(static_cast<unsigned int>(base));
        indices.push_back(static_cast<unsigned int>(base + 1));
        indices.push_back(static_cast<unsigned int>(base + 2));
        indices.push_back(static_cast<unsigned int>(base));
        indices.push_back(static_cast<unsigned int>(base + 2));
        indices.push_back(static_cast<unsigned int>(base + 3));
    }

    // Create light housing (box at top)
    float lightSize = 0.3f;
    float lightY = height * 0.7f;

    // Light housing bottom
    size_t base = vertices.size() / 9;
    vertices.insert(vertices.end(), {
        -lightSize, lightY, -lightSize, poleColor.r, poleColor.g, poleColor.b, 0.0f, -1.0f, 0.0f,
         lightSize, lightY, -lightSize, poleColor.r, poleColor.g, poleColor.b, 0.0f, -1.0f, 0.0f,
         lightSize, lightY,  lightSize, poleColor.r, poleColor.g, poleColor.b, 0.0f, -1.0f, 0.0f,
        -lightSize, lightY,  lightSize, poleColor.r, poleColor.g, poleColor.b, 0.0f, -1.0f, 0.0f
        });

    indices.push_back(static_cast<unsigned int>(base));
    indices.push_back(static_cast<unsigned int>(base + 1));
    indices.push_back(static_cast<unsigned int>(base + 2));
    indices.push_back(static_cast<unsigned int>(base));
    indices.push_back(static_cast<unsigned int>(base + 2));
    indices.push_back(static_cast<unsigned int>(base + 3));

    // Light housing sides
    base = vertices.size() / 9;
    vertices.insert(vertices.end(), {
        -lightSize, lightY, -lightSize, poleColor.r, poleColor.g, poleColor.b, 0.0f, 0.0f, -1.0f,
         lightSize, lightY, -lightSize, poleColor.r, poleColor.g, poleColor.b, 0.0f, 0.0f, -1.0f,
         lightSize, lightY + lightSize * 0.5f, -lightSize, lightColor.r, lightColor.g, lightColor.b, 0.0f, 0.0f, -1.0f,
        -lightSize, lightY + lightSize * 0.5f, -lightSize, lightColor.r, lightColor.g, lightColor.b, 0.0f, 0.0f, -1.0f
        });

    indices.push_back(static_cast<unsigned int>(base));
    indices.push_back(static_cast<unsigned int>(base + 1));
    indices.push_back(static_cast<unsigned int>(base + 2));
    indices.push_back(static_cast<unsigned int>(base));
    indices.push_back(static_cast<unsigned int>(base + 2));
    indices.push_back(static_cast<unsigned int>(base + 3));
}

void StreetLight::setup() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

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

void StreetLight::draw() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
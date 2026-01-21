#include "car.h"
#include "model.h"  // Include model.h here, not in car.h
#include <iostream>

Car::Car(const std::string& modelPath, glm::vec3 position)
    : position(position), rotationAngle(0.0f),
    rotationAxis(0.0f, 1.0f, 0.0f), scale(1.0f) {

    try {
        model = new Model(modelPath);
        std::cout << "Car model loaded successfully from: " << modelPath << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "Failed to load car model: " << e.what() << std::endl;
        model = nullptr;
    }
}

Car::~Car() {
    if (model) {
        delete model;
    }
}

void Car::Draw(Shader& shader) {
    if (model) {
        shader.setMat4("model", GetModelMatrix());
        model->Draw(shader);
    }
}

void Car::SetPosition(glm::vec3 newPosition) {
    position = newPosition;
}

void Car::SetRotation(float angle, glm::vec3 axis) {
    rotationAngle = angle;
    rotationAxis = axis;
}

void Car::SetScale(glm::vec3 newScale) {
    scale = newScale;
}

glm::mat4 Car::GetModelMatrix() const {
    glm::mat4 modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat, position);
    modelMat = glm::rotate(modelMat, glm::radians(rotationAngle), rotationAxis);
    modelMat = glm::scale(modelMat, scale);
    return modelMat;
}

// SimpleCar implementation
SimpleCar::SimpleCar(glm::vec3 position) : position(position) {
    VAO = VBO = EBO = 0;
    setup();
}

SimpleCar::~SimpleCar() {
    cleanup();
}

void SimpleCar::createCarMesh() {
    // Simple car shape
    float vertices[] = {
        // Positions          // Normals         // Colors (for testing)
        // Front
        -0.8f, 0.0f, -0.4f,   0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
         0.8f, 0.0f, -0.4f,   0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
         0.8f, 0.5f, -0.4f,   0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
        -0.8f, 0.5f, -0.4f,   0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,

        // Back
        -0.8f, 0.0f,  0.4f,   0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
         0.8f, 0.0f,  0.4f,   0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
         0.8f, 0.5f,  0.4f,   0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        -0.8f, 0.5f,  0.4f,   0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f
    };

    unsigned int indices[] = {
        // Front face
        0, 1, 2, 2, 3, 0,
        // Back face
        4, 5, 6, 6, 7, 4,
        // Left side
        0, 3, 7, 7, 4, 0,
        // Right side
        1, 2, 6, 6, 5, 1,
        // Top
        2, 3, 7, 7, 6, 2,
        // Bottom
        0, 1, 5, 5, 4, 0
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Color attribute (for testing)
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void SimpleCar::Draw(Shader& shader) {
    glm::mat4 modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat, position);
    modelMat = glm::scale(modelMat, glm::vec3(0.5f)); // Scale down
    shader.setMat4("model", modelMat);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void SimpleCar::setup() {
    createCarMesh();
}

void SimpleCar::cleanup() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);
}
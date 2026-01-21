#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.h"

// Forward declaration - don't include model.h if we only use pointer
class Model;

class Car {
public:
    Car(const std::string& modelPath, glm::vec3 position = glm::vec3(0.0f));
    ~Car();  // Add destructor!
    void Draw(Shader& shader);
    void SetPosition(glm::vec3 position);
    void SetRotation(float angle, glm::vec3 axis = glm::vec3(0.0f, 1.0f, 0.0f));
    void SetScale(glm::vec3 scale);
    float GetRotationAngle() const { return rotationAngle; }
    glm::vec3 GetPosition() const { return position; }
    glm::mat4 GetModelMatrix() const;

private:
    Model* model;  // Use pointer since Model is incomplete type here
    glm::vec3 position;
    glm::vec3 rotationAxis;
    float rotationAngle;
    glm::vec3 scale;
};

// Simple car class for testing
class SimpleCar {
public:
    SimpleCar(glm::vec3 position = glm::vec3(0.0f));
    ~SimpleCar();
    void Draw(Shader& shader);
    void setup();
    void cleanup();

private:
    glm::vec3 position;
    unsigned int VAO, VBO, EBO;

    void createCarMesh();
};
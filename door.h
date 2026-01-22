#pragma once
#ifndef DOOR_H
#define DOOR_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.h"

class Door {
private:
    glm::vec3 position;
    glm::vec3 size;
    float rotationAngle;  // For door opening animation
    bool isOpen;
    bool isLeftDoor;      // True for left door, false for right door

    // OpenGL objects
    unsigned int VAO, VBO, EBO;

    // Door properties
    glm::vec3 doorColor;
    glm::vec3 handleColor;

public:
    Door(glm::vec3 pos, glm::vec3 sz, bool leftDoor);
    ~Door();

    void setup();
    void draw(Shader& shader);
    void update(float deltaTime);

    // Door controls
    void open();
    void close();
    void toggle();
    bool getIsOpen() const { return isOpen; }

    void setColor(glm::vec3 color) { doorColor = color; }
    glm::vec3 getColor() const { return doorColor; }

    // For animation
    void setRotation(float angle) { rotationAngle = angle; }
    float getRotation() const { return rotationAngle; }

    glm::vec3 getPosition() const { return position; }
    glm::vec3 getSize() const { return size; }
};

#endif
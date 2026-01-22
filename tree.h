#ifndef TREE_H
#define TREE_H

#include <glm/glm.hpp>
#include <string>
#include "car.h"  // Include car.h directly instead of forward declaration

class Shader;

class Tree {
private:
    Car* treeModel;
    glm::vec3 position;
    float scale;
    glm::vec3 color;

public:
    Tree(glm::vec3 pos = glm::vec3(0.0f), float s = 1.0f, glm::vec3 col = glm::vec3(0.0f, 0.5f, 0.0f));
    ~Tree();

    // Load the tree model
    bool loadModel(const std::string& filename);

    // Draw the tree
    void draw(Shader& shader);

    // Setters
    void setPosition(glm::vec3 pos);
    void setScale(float s);
    void setColor(glm::vec3 col);

    // Getters
    glm::vec3 getPosition() const { return position; }
    float getScale() const { return scale; }
    glm::vec3 getColor() const { return color; }
};

#endif
#include "tree.h"
#include "shader.h"
#include <iostream>

Tree::Tree(glm::vec3 pos, float s, glm::vec3 col)
    : position(pos), scale(s), color(col), treeModel(nullptr) {
}

Tree::~Tree() {
    if (treeModel) {
        delete treeModel;
    }
}

bool Tree::loadModel(const std::string& filename) {
    try {
        // Use Car class to load the model
        treeModel = new Car(filename, position);
        treeModel->SetScale(glm::vec3(scale));
        treeModel->SetColor(color);
        std::cout << "Tree model loaded via Car class from: " << filename << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cout << "Failed to load tree model from " << filename
            << ": " << e.what() << std::endl;
        treeModel = nullptr;
        return false;
    }
}

void Tree::draw(Shader& shader) {
    if (treeModel) {
        treeModel->Draw(shader);
    }
}

void Tree::setPosition(glm::vec3 pos) {
    position = pos;
    if (treeModel) {
        treeModel->SetPosition(pos);
    }
}

void Tree::setScale(float s) {
    scale = s;
    if (treeModel) {
        treeModel->SetScale(glm::vec3(s));
    }
}

void Tree::setColor(glm::vec3 col) {
    color = col;
    if (treeModel) {
        treeModel->SetColor(col);
    }
}
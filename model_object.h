#pragma once
#include <glm/glm.hpp>
#include <string>

class Model;
class Shader;

class ModelObject {
private:
    Model* model;
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 color;
    float rotation;

public:
    ModelObject(const std::string& modelPath, glm::vec3 pos = glm::vec3(0.0f));
    ~ModelObject();

    bool load(const std::string& modelPath);
    void draw(Shader& shader);

    void setPosition(glm::vec3 pos) { position = pos; }
    void setScale(glm::vec3 s) { scale = s; }
    void setColor(glm::vec3 col) { color = col; }
    void setRotation(float rot) { rotation = rot; }

    glm::vec3 getPosition() const { return position; }
};
// street.h
#ifndef STREET_H
#define STREET_H

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Street {
private:
    GLuint VAO, VBO, EBO;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    glm::vec3 position;
    glm::vec3 size;
    glm::vec3 color;

public:
    Street(const glm::vec3& pos, const glm::vec3& dim, const glm::vec3& col = glm::vec3(0.3f, 0.3f, 0.35f));
    ~Street();

    void generateVertices();
    void addQuad(const glm::vec3& p1, const glm::vec3& p2,
        const glm::vec3& p3, const glm::vec3& p4,
        const glm::vec3& color, const glm::vec3& normal);
    void setup();
    void updateBuffers();
    void draw();
    void cleanup();

    glm::vec3 getPosition() const { return position; }
    glm::vec3 getSize() const { return size; }
};

class StreetLight {
private:
    GLuint VAO, VBO, EBO;
    glm::vec3 position;
    float height;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

public:
    StreetLight(const glm::vec3& pos, float h = 6.0f);
    ~StreetLight();

    void createModel();
    void setup();
    void draw();

    glm::vec3 getPosition() const { return position; }
};

#endif
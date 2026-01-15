#ifndef LIGHTING_H
#define LIGHTING_H

#include <glm/glm.hpp>

struct Light {
    glm::vec3 position;
    glm::vec3 color;
    float ambientStrength;
    float specularStrength;
    int shininess;

    Light() : position(0.0f, 2.0f, 0.0f),
        color(1.0f, 1.0f, 1.0f),
        ambientStrength(0.2f),
        specularStrength(0.5f),
        shininess(32) {
    }

    void setupShader(unsigned int shaderProgram, const glm::vec3& viewPos);
};

#endif
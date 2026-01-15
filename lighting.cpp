#include "lighting.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

void Light::setupShader(unsigned int shaderProgram, const glm::vec3& viewPos) {
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(position));
    glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(viewPos));
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, glm::value_ptr(color));
    glUniform1f(glGetUniformLocation(shaderProgram, "ambientStrength"), ambientStrength);
    glUniform1f(glGetUniformLocation(shaderProgram, "specularStrength"), specularStrength);
    glUniform1i(glGetUniformLocation(shaderProgram, "shininess"), shininess);
}
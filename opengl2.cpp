#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "room.h"
#include "lighting.h"

// Camera and input variables
glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool firstMouse = true;
float yaw = -90.0f, pitch = 0.0f;
float lastX = 400.0f, lastY = 300.0f;
float fov = 60.0f;

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window, Room& room);

int main() {
    // Initialize GLFW and create window
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Lighted Room", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD!" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // Create shader, room, and lighting objects
    Shader lightingShader("shader.vert", "shader.frag");
    Room room(10.0f, 4.0f, 10.0f);
    Light light;

    // Setup room
    room.setup();

    // Configure light
    light.position = glm::vec3(0.0f, 3.0f, 0.0f);
    light.color = glm::vec3(1.0f, 1.0f, 1.0f);
    light.ambientStrength = 0.2f;
    light.specularStrength = 0.5f;
    light.shininess = 32;

    // Main render loop
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window, room);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use shader
        lightingShader.use();

        // Set transformation matrices
        glm::mat4 projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        lightingShader.setMat4("model", glm::mat4(1.0f));

        // Setup lighting
        light.setupShader(lightingShader.ID, cameraPos);

        // Draw room
        room.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    room.cleanup();
    glfwTerminate();

    return 0;
}

// Window resize callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Mouse movement callback
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

// Mouse scroll callback
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    fov -= (float)yoffset;
    if (fov < 20.0f)
        fov = 20.0f;
    if (fov > 90.0f)
        fov = 90.0f;
}

// Input processing with collision detection
void processInput(GLFWwindow* window, Room& room) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 5.0f * deltaTime;
    glm::vec3 newPos = cameraPos;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        newPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        newPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        newPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        newPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        newPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        newPos -= cameraSpeed * cameraUp;

    // Collision detection using room dimensions
    float margin = 0.5f;
    float halfWidth = room.getWidth() / 2 - margin;
    float halfDepth = room.getDepth() / 2 - margin;

    if (newPos.x > -halfWidth && newPos.x < halfWidth &&
        newPos.z > -halfDepth && newPos.z < halfDepth &&
        newPos.y > margin && newPos.y < room.getHeight() - margin) {
        cameraPos = newPos;
    }
}
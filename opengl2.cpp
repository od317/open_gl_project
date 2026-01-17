#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "room.h"
#include "light_source.h"
#include "glass.h"

// Camera and input variables
glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 0.0f);  // Start at eye level
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool firstMouse = true;
float yaw = -90.0f, pitch = 0.0f;
float lastX = 400.0f, lastY = 300.0f;
float fov = 60.0f;

// Light source animation
float lightAngle = 0.0f;

// Control flags
bool showControls = true;
bool glassVisible = true;
bool lightMoving = true;

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window, Room& room, LightSource& light, GlassWindow& glass);
void printControls();

float transparencyStep = 0.1f;
bool showTransparencyInfo = true;

void processInput(GLFWwindow* window, Room& room, LightSource& light, GlassWindow& glass);
void printTransparencyControls();

int main() {


    // Initialize GLFW and create window
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Glass Window Demo - Press H for Controls", NULL, NULL);
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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Print controls on startup
    printTransparencyControls();

    // Create shaders
    Shader lightingShader("shader.vert", "shader.frag");
    Shader lightCubeShader("shader.vert", "light_cube.frag");
    Shader glassShader("glass.vert", "glass_simple.frag");  // Use simple glass shader

    Room room(10.0f, 4.0f, 10.0f);
    room.addInteriorWallWithWindow(
        glm::vec3(0.0f, 2.0f, 0.0f),
        glm::vec3(8.0f, 3.0f, 0.1f),
        glm::vec3(0.0f, 0.5f, 0.0f),
        glm::vec3(3.0f, 2.0f, 0.0f)
    );
    room.setup();

    // Create glass window
    glm::vec3 windowPos = room.getWindowPosition();
    windowPos.z += 0.05f;
    GlassWindow glassWindow(windowPos, glm::vec3(3.0f, 2.0f, 0.02f));
    glassWindow.setAsTintedGlass(); // Starts at 70% transparent
    glassWindow.setup();

    // Create light
    LightSource lightSource(glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.8f));
    lightSource.setup();
    lightSource.setAmbientStrength(0.2f);

    // Create a cube behind the window (to see through glass)
    unsigned int cubeVAO, cubeVBO, cubeEBO;
    float cubeVertices[] = {
        // Position              // Color (red)
        -1.0f, 0.0f, 6.0f,      1.0f, 0.0f, 0.0f,
         1.0f, 0.0f, 6.0f,      1.0f, 0.0f, 0.0f,
         1.0f, 2.0f, 6.0f,      1.0f, 0.0f, 0.0f,
        -1.0f, 2.0f, 6.0f,      1.0f, 0.0f, 0.0f,

        -1.0f, 0.0f, 8.0f,      0.0f, 1.0f, 0.0f,
         1.0f, 0.0f, 8.0f,      0.0f, 1.0f, 0.0f,
         1.0f, 2.0f, 8.0f,      0.0f, 1.0f, 0.0f,
        -1.0f, 2.0f, 8.0f,      0.0f, 1.0f, 0.0f
    };

    unsigned int cubeIndices[] = {
        0, 1, 2, 2, 3, 0, // Front face
        4, 5, 6, 6, 7, 4, // Back face
        0, 3, 7, 7, 4, 0, // Left face
        1, 2, 6, 6, 5, 1, // Right face
        0, 1, 5, 5, 4, 0, // Bottom
        2, 3, 7, 7, 6, 2  // Top
    };

    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);

    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Main render loop
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Animate light if enabled
        if (lightMoving) {
            lightAngle += 0.5f * deltaTime;
            glm::vec3 newLightPos = glm::vec3(
                sin(lightAngle) * 3.0f,
                2.5f + sin(lightAngle * 2.0f) * 0.5f,
                cos(lightAngle) * 3.0f
            );
            lightSource.setPosition(newLightPos);
        }

        processInput(window, room, lightSource, glassWindow);

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);  // Dark blue background
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 model = glm::mat4(1.0f);

        // 1. Draw room with lighting
        lightingShader.use();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        lightingShader.setMat4("model", model);

        // Update light in room shader
        lightSource.updateShader(lightingShader.ID, cameraPos);

        // Draw room
        room.draw();

        // 2. Draw cube behind window (to see through glass)
        lightingShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 1.0f, 7.0f));
        lightingShader.setMat4("model", model);

        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // 3. Draw the light source
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);

        model = glm::mat4(1.0f);
        model = glm::translate(model, lightSource.getPosition());
        model = glm::scale(model, glm::vec3(0.2f));  // Smaller light cube
        lightCubeShader.setMat4("model", model);
        lightCubeShader.setVec3("lightColor", lightSource.getColor());
        lightSource.draw(lightCubeShader.ID);

        // 4. Draw glass window (if visible)
        if (glassVisible) {
            glassShader.use();
            glassShader.setMat4("projection", projection);
            glassShader.setMat4("view", view);

            model = glm::mat4(1.0f);
            model = glm::translate(model, glassWindow.getPosition());
            glassShader.setMat4("model", model);

            // Update glass shader with ALL required parameters
            glassWindow.updateShader(glassShader.ID,
                lightSource.getPosition(),
                cameraPos,
                lightSource.getColor(),
                0.3f);  // ambient strength

            // Draw glass with proper transparency
            glDepthMask(GL_FALSE);  // Disable depth writing for transparent objects
            glassWindow.draw(glassShader.ID);
            glDepthMask(GL_TRUE);   // Re-enable depth writing
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    room.cleanup();
    lightSource.cleanup();
    glassWindow.cleanup();
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &cubeEBO);
    glfwTerminate();

    return 0;
}

// Print all controls
void printTransparencyControls() {
    std::cout << "\n=============================================\n";
    std::cout << "        GLASS TRANSPARENCY CONTROLS\n";
    std::cout << "=============================================\n";
    std::cout << "  [+]/[=] : Increase transparency\n";
    std::cout << "  [-]     : Decrease transparency\n";
    std::cout << "  [0]     : Fully transparent (90%)\n";
    std::cout << "  [1]     : Semi-transparent (50%)\n";
    std::cout << "  [2]     : Slightly transparent (20%)\n";
    std::cout << "  [3]     : Opaque (0% transparent)\n";
    std::cout << "  [T]     : Toggle transparency info display\n";
    std::cout << "  [I]     : Print current transparency value\n";
    std::cout << "=============================================\n\n";
}

// Enhanced input processing with transparency controls
void processInput(GLFWwindow* window, Room& room, LightSource& light, GlassWindow& glass) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // TRANSPARENCY CONTROLS

    // Increase transparency (+ or = key)
    static bool plusPressed = false;
    static bool equalPressed = false;

    if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS && !equalPressed) {
        equalPressed = true;
        glass.increaseTransparency(transparencyStep);
        if (showTransparencyInfo) {
            std::cout << "Transparency INCREASED to: " << glass.getTransparency()
                << " (" << (glass.getTransparency() * 100) << "%)" << std::endl;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_RELEASE) {
        equalPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS && !plusPressed) {
        plusPressed = true;
        glass.increaseTransparency(transparencyStep);
        if (showTransparencyInfo) {
            std::cout << "Transparency INCREASED to: " << glass.getTransparency()
                << " (" << (glass.getTransparency() * 100) << "%)" << std::endl;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_RELEASE) {
        plusPressed = false;
    }

    // Decrease transparency (- key)
    static bool minusPressed = false;
    if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS && !minusPressed) {
        minusPressed = true;
        glass.decreaseTransparency(transparencyStep);
        if (showTransparencyInfo) {
            std::cout << "Transparency DECREASED to: " << glass.getTransparency()
                << " (" << (glass.getTransparency() * 100) << "%)" << std::endl;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_RELEASE) {
        minusPressed = false;
    }

    // Quick preset keys (0-3)
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
        static bool zeroPressed = false;
        if (!zeroPressed) {
            zeroPressed = true;
            glass.setFullyTransparent();
            if (showTransparencyInfo) {
                std::cout << "Set to FULLY TRANSPARENT (90%)" << std::endl;
            }
        }
    }
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_RELEASE) {
        static bool zeroReleased = false;
        if (!zeroReleased) {
            zeroReleased = true;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        static bool onePressed = false;
        if (!onePressed) {
            onePressed = true;
            glass.setSemiTransparent();
            if (showTransparencyInfo) {
                std::cout << "Set to SEMI-TRANSPARENT (50%)" << std::endl;
            }
        }
    }
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE) {
        static bool oneReleased = false;
        if (!oneReleased) {
            oneReleased = true;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        static bool twoPressed = false;
        if (!twoPressed) {
            twoPressed = true;
            glass.setSlightlyTransparent();
            if (showTransparencyInfo) {
                std::cout << "Set to SLIGHTLY TRANSPARENT (20%)" << std::endl;
            }
        }
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE) {
        static bool twoReleased = false;
        if (!twoReleased) {
            twoReleased = true;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        static bool threePressed = false;
        if (!threePressed) {
            threePressed = true;
            glass.setOpaque();
            if (showTransparencyInfo) {
                std::cout << "Set to OPAQUE (0% transparent)" << std::endl;
            }
        }
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_RELEASE) {
        static bool threeReleased = false;
        if (!threeReleased) {
            threeReleased = true;
        }
    }

    // Toggle transparency info display (T key)
    static bool tPressed = false;
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && !tPressed) {
        tPressed = true;
        showTransparencyInfo = !showTransparencyInfo;
        std::cout << "Transparency info display "
            << (showTransparencyInfo ? "ENABLED" : "DISABLED") << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE) {
        tPressed = false;
    }

    // Print current transparency (I key)
    static bool iPressed = false;
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS && !iPressed) {
        iPressed = true;
        std::cout << "Current transparency: " << glass.getTransparency()
            << " (" << (glass.getTransparency() * 100) << "%)" << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_RELEASE) {
        iPressed = false;
    }

    // Adjust transparency step size with PageUp/PageDown
    static bool pageUpPressed = false;
    static bool pageDownPressed = false;

    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS && !pageUpPressed) {
        pageUpPressed = true;
        transparencyStep += 0.05f;
        if (transparencyStep > 0.5f) transparencyStep = 0.5f;
        std::cout << "Transparency step size: " << transparencyStep << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_RELEASE) {
        pageUpPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS && !pageDownPressed) {
        pageDownPressed = true;
        transparencyStep -= 0.05f;
        if (transparencyStep < 0.01f) transparencyStep = 0.01f;
        std::cout << "Transparency step size: " << transparencyStep << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_RELEASE) {
        pageDownPressed = false;
    }

    // Camera movement (WASD + QE)
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
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        newPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        newPos -= cameraSpeed * cameraUp;

    // Collision detection
    float margin = 0.5f;
    float halfWidth = room.getWidth() / 2 - margin;
    float halfDepth = room.getDepth() / 2 - margin;

    if (newPos.x > -halfWidth && newPos.x < halfWidth &&
        newPos.z > -halfDepth && newPos.z < halfDepth &&
        newPos.y > margin && newPos.y < room.getHeight() - margin) {
        cameraPos = newPos;
    }
}

// Callback functions (mouse and scroll)
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

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

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    fov -= (float)yoffset;
    if (fov < 20.0f) fov = 20.0f;
    if (fov > 90.0f) fov = 90.0f;
}
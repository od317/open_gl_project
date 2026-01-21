#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <vector>
#include <fstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "skybox.h"
#include "shader.h"
#include "room.h"
#include "light_source.h"
#include "glass.h"
#include "car.h"
#include "floor.h"

// Camera and input variables
glm::vec3 cameraPos = glm::vec3(0.0f, 3.0f, 10.0f);  // Start further back
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool firstMouse = true; 
float yaw = -90.0f, pitch = 0.0f;
float lastX = 400.0f, lastY = 300.0f;
float fov = 60.0f;

Floor* showroomFloor = nullptr;
// Car selection
int selectedCar = 0;  // 0 = Porsche, 1 = Koenigsegg

// Camera modes
enum CameraMode {
    FREE_CAMERA,
    PORSCHE_DRIVER_SEAT,
    KOENIGSEGG_DRIVER_SEAT
};

CameraMode cameraMode = FREE_CAMERA;
bool cameraInCar = false;

// Camera offsets for driver's seat (adjust these values as needed)
glm::vec3 porscheDriverOffset = glm::vec3(-0.2f, 0.9f, -0.6f);
glm::vec3 koenigseggDriverOffset = glm::vec3(-0.15f, 0.8f, -0.5f);
glm::vec3 porscheDriverSeatPos = glm::vec3(-3.67038f, 0.624424f, -0.329982f);  // Adjust these!
glm::vec3 koenigseggDriverSeatPos = glm::vec3(4.0f, 1.3f, 0.0f);  // Adjust these!

// Store original camera position when entering driver seat
glm::vec3 savedCameraPos;
glm::vec3 savedCameraFront;

// Light source animation
float lightAngle = 0.0f;

// Control flags
bool showControls = true;
bool glassVisible = true;
bool lightMoving = true;

// Car variables - TWO CARS!
Car* porsche = nullptr;
Car* koenigsegg = nullptr;
bool porscheLoaded = false;
bool koenigseggLoaded = false;

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window, Room& room, LightSource& light, GlassWindow& glass);
void printTransparencyControls();
void printCarInfo();
void toggleDriverSeatView(int carIndex);
void updateDriverSeatCamera();

float transparencyStep = 0.1f;
bool showTransparencyInfo = true;

// Helper function to check if file exists
bool fileExists(const std::string& path) {
    std::ifstream file(path);
    bool exists = file.good();
    file.close();
    return exists;
}

// Helper function to check shader files
void checkShaderFiles() {
    std::cout << "Checking shader files..." << std::endl;
    std::string shaders[] = {
        "shader.vert",
        "shader.frag",
        "light_cube.frag",
        "glass.vert",
        "glass_simple.frag"
    };

    bool allFound = true;
    for (const auto& shader : shaders) {
        if (fileExists(shader)) {
            std::cout << "  Found: " << shader << std::endl;
        }
        else {
            std::cout << "  MISSING: " << shader << std::endl;
            allFound = false;
        }
    }

    if (!allFound) {
        std::cout << "WARNING: Some shader files are missing!" << std::endl;
        std::cout << "Make sure shader files are in the same folder as the executable." << std::endl;
    }
}

// Load a car model
Car* loadCarModel(const std::string& name, const std::vector<std::string>& paths,
    const glm::vec3& position, float scale) {
    std::cout << "\n=== Loading " << name << " ===" << std::endl;

    for (const auto& path : paths) {
        std::cout << "Trying to load from: " << path << std::endl;

        if (fileExists(path)) {
            std::cout << "File found! Attempting to load..." << std::endl;

            try {
                Car* car = new Car(path, position);
                car->SetScale(glm::vec3(scale));
                std::cout << "SUCCESS: " << name << " loaded from: " << path << std::endl;
                return car;
            }
            catch (const std::exception& e) {
                std::cout << "Failed to load model: " << e.what() << std::endl;
            }
            catch (...) {
                std::cout << "Unknown error loading model." << std::endl;
            }
        }
        else {
            std::cout << "File not found." << std::endl;
        }
    }

    std::cout << "WARNING: Could not load " << name << std::endl;
    return nullptr;
}

void toggleDriverSeatView(int carIndex) {
    if (cameraMode == FREE_CAMERA) {
        // Save current camera state
        savedCameraPos = cameraPos;
        savedCameraFront = cameraFront;

        if (carIndex == 0 && porscheLoaded) {
            cameraMode = PORSCHE_DRIVER_SEAT;

            // Set camera to driver seat position (it will be updated in updateDriverSeatCamera)
            glm::vec3 carPos = porsche->GetPosition();
            float carRotation = porsche->GetRotationAngle();

            // Calculate relative offset
            glm::vec3 relativeOffset = porscheDriverSeatPos - glm::vec3(-4.0f, 0.3f, 0.0f);

            // Apply car's rotation
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(carRotation), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::vec4 rotatedOffset = rotationMatrix * glm::vec4(relativeOffset, 1.0f);

            // Set initial position
            cameraPos = carPos + glm::vec3(rotatedOffset);
            cameraFront = glm::vec3(0.0f, 0.0f, -1.0f); // Look forward
            yaw = -90.0f; // Reset yaw to look forward
            pitch = 0.0f; // Reset pitch

            std::cout << "Entered Porsche driver's seat view" << std::endl;
            std::cout << "Position: (" << cameraPos.x << ", "
                << cameraPos.y << ", " << cameraPos.z << ")" << std::endl;
            std::cout << "Camera will follow car movement!" << std::endl;
        }
        else if (carIndex == 1 && koenigseggLoaded) {
            cameraMode = KOENIGSEGG_DRIVER_SEAT;

            // Same logic for Koenigsegg
            glm::vec3 carPos = koenigsegg->GetPosition();
            float carRotation = koenigsegg->GetRotationAngle();

            glm::vec3 relativeOffset = koenigseggDriverSeatPos - glm::vec3(4.0f, 0.3f, 0.0f);

            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(carRotation), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::vec4 rotatedOffset = rotationMatrix * glm::vec4(relativeOffset, 1.0f);

            cameraPos = carPos + glm::vec3(rotatedOffset);
            cameraFront = glm::vec3(0.0f, 0.0f, -1.0f); // Look forward
            yaw = -90.0f; // Reset yaw to look forward
            pitch = 0.0f; // Reset pitch

            std::cout << "Entered Koenigsegg driver's seat view" << std::endl;
            std::cout << "Position: (" << cameraPos.x << ", "
                << cameraPos.y << ", " << cameraPos.z << ")" << std::endl;
            std::cout << "Camera will follow car movement!" << std::endl;
        }
        cameraInCar = true;
    }
    else {
        // Restore saved camera state
        cameraMode = FREE_CAMERA;
        cameraInCar = false;
        cameraPos = savedCameraPos;
        cameraFront = savedCameraFront;
        std::cout << "Returned to free camera mode" << std::endl;
    }
}

// Update driver seat camera position
void updateDriverSeatCamera() {
    if (cameraMode == PORSCHE_DRIVER_SEAT && porsche) {
        glm::vec3 carPos = porsche->GetPosition();
        float carRotation = porsche->GetRotationAngle();

        // Get the RELATIVE position from when we set the driver seat
        // The driver seat position is relative to the car's original position (-4.0f, 0.3f, 0.0f)
        glm::vec3 relativeOffset = porscheDriverSeatPos - glm::vec3(-4.0f, 0.3f, 0.0f);

        // Apply car's rotation to the offset
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(carRotation), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::vec4 rotatedOffset = rotationMatrix * glm::vec4(relativeOffset, 1.0f);

        // Update camera position to follow the car
        cameraPos = carPos + glm::vec3(rotatedOffset);
    }
    else if (cameraMode == KOENIGSEGG_DRIVER_SEAT && koenigsegg) {
        glm::vec3 carPos = koenigsegg->GetPosition();
        float carRotation = koenigsegg->GetRotationAngle();

        // Same logic for Koenigsegg
        glm::vec3 relativeOffset = koenigseggDriverSeatPos - glm::vec3(4.0f, 0.3f, 0.0f);

        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(carRotation), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::vec4 rotatedOffset = rotationMatrix * glm::vec4(relativeOffset, 1.0f);

        cameraPos = carPos + glm::vec3(rotatedOffset);
    }
}


void findDriverSeatPosition() {
    if (selectedCar == 0) {
        std::cout << "\n=== FINDING PORSCHE DRIVER SEAT ===" << std::endl;
        std::cout << "Current camera position: (" << cameraPos.x << ", "
            << cameraPos.y << ", " << cameraPos.z << ")" << std::endl;
        std::cout << "Use WASD to move around, then press 'M' to set as Porsche driver seat" << std::endl;
    }
    else {
        std::cout << "\n=== FINDING KOENIGSEGG DRIVER SEAT ===" << std::endl;
        std::cout << "Current camera position: (" << cameraPos.x << ", "
            << cameraPos.y << ", " << cameraPos.z << ")" << std::endl;
        std::cout << "Use WASD to move around, then press 'M' to set as Koenigsegg driver seat" << std::endl;
    }
}

int main() {
    // Initialize GLFW and create window
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1000, 800, "Car Showroom - Porsche 911 GT2 & Koenigsegg", NULL, NULL);
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

    // Check shader files first
    checkShaderFiles();

    // Print controls on startup
    printTransparencyControls();

    // Create shaders
    Shader lightingShader("shader.vert", "shader.frag");
    Shader lightCubeShader("shader.vert", "light_cube.frag");
    Shader glassShader("glass.vert", "glass_simple.frag");
    Shader skyboxShader("skybox.vert", "skybox.frag");
    Shader texturedShader("shader_with_texture.vert", "shader_with_texture.frag");

    // Load Porsche 911 GT2
    std::vector<std::string> porschePaths = {
        "models/Porsche_911_GT2/Porsche_911_GT2.obj",
        "../models/Porsche_911_GT2/Porsche_911_GT2.obj",
        "C:/projects/university/opengl/opengl2/models/Porsche_911_GT2/Porsche_911_GT2.obj",
        "C:/Users/HP/Downloads/Porsche_911_GT2.obj"
    };

    porsche = loadCarModel("Porsche 911 GT2", porschePaths, glm::vec3(-4.0f, 1.0f, 0.0f), 0.8f);
    porscheLoaded = (porsche != nullptr);

    if (porscheLoaded) {
        porsche->SetRotation(180.0f);  // Face forward
        porsche->SetPosition(glm::vec3(-4.0f, 0.3f, 0.0f));  // Add this line to ensure position
    }

    // Load Koenigsegg
    std::vector<std::string> koenigseggPaths = {
        "models/uploads_files_2792345_Koenigsegg.obj",
        "../models/uploads_files_2792345_Koenigsegg.obj",
        "C:/projects/university/opengl/opengl2/models/uploads_files_2792345_Koenigsegg.obj",
        "C:/Users/HP/Downloads/82-koenigsegg-agera/uploads_files_2792345_Koenigsegg.obj"
    };

    koenigsegg = loadCarModel("Koenigsegg", koenigseggPaths, glm::vec3(4.0f, 0.3f, 0.0f), 0.08f);
    koenigseggLoaded = (koenigsegg != nullptr);

    if (koenigseggLoaded) {
        koenigsegg->SetRotation(180.0f);  // Face forward
        koenigsegg->SetPosition(glm::vec3(4.0f, 0.3f, 0.0f));  // Add this line
    }

    std::cout << "\n=== Car Loading Complete ===" << std::endl;
    std::cout << "Porsche loaded: " << (porscheLoaded ? "YES" : "NO") << std::endl;
    std::cout << "Koenigsegg loaded: " << (koenigseggLoaded ? "YES" : "NO") << std::endl;

    Skybox skybox;
    skybox.setup();
    bool skyboxLoaded = skybox.loadCrossFormatCubemap("textures/skybox/Cubemap_Sky_06-512x512.png");




    // If that fails, try a relative path
    if (!skyboxLoaded) {
        skyboxLoaded = skybox.loadCrossFormatCubemap(
            "textures/skybox/Cubemap_Sky_06-512x512.png"
        );
    }

    if (!skyboxLoaded) {
        std::cout << "Failed to load skybox image, using default gradient skybox" << std::endl;
    }


    showroomFloor = new Floor(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(100.0f, 1.0f, 100.0f),
        glm::vec3(0.7f, 0.7f, 0.7f)  // Neutral gray
    );

    // Setup floor
    showroomFloor->setup();

    // Try to load the rubber tiles texture
    std::string floorTexturePath = "textures/floor/rubber_tiles_diff_1k.jpg";


    if (fileExists(floorTexturePath)) {
        std::cout << "Loading floor texture: " << floorTexturePath << std::endl;
        bool textureLoaded = showroomFloor->loadTexture(floorTexturePath);

        if (textureLoaded) {
            std::cout << "Successfully loaded rubber tiles floor texture!" << std::endl;
            // Adjust texture repeat for large floor
            // For a 25x25 floor, we might want the texture to repeat 5x5 times
            // We'll handle this in the shader or by modifying UVs
        }
        else {
            std::cout << "Failed to load rubber tiles texture, using default checkerboard" << std::endl;
        }
    }
    else {
        std::cout << "Floor texture not found at: " << floorTexturePath << std::endl;
        std::cout << "Using default checkerboard floor pattern" << std::endl;

        // Try alternative paths
        std::vector<std::string> alternativePaths = {
            "rubber_tiles_diff_1k.jpg",
            "../textures/floor/rubber_tiles_diff_1k.jpg",
            "C:/Users/HP/Downloads/rubber_tiles_diff_1k.jpg"
        };

        for (const auto& path : alternativePaths) {
            if (fileExists(path)) {
                std::cout << "Found floor texture at: " << path << std::endl;
                showroomFloor->loadTexture(path);
                break;
            }
        }
    }

    // Create LARGER room (20x8x20)
    Room room(20.0f, 8.0f, 20.0f);  // Width, Height, Depth - MUCH larger!
    room.addInteriorWallWithWindow(
        glm::vec3(0.0f, 3.0f, 0.0f),      // Position
        glm::vec3(15.0f, 5.0f, 0.1f),     // Size (wider window)
        glm::vec3(0.0f, 1.0f, 0.0f),      // Wall color
        glm::vec3(8.0f, 4.0f, 0.0f)       // Window size
    );
    room.setup();

    // Create glass window
    glm::vec3 windowPos = room.getWindowPosition();
    windowPos.z += 0.05f;
    GlassWindow glassWindow(windowPos, glm::vec3(8.0f, 4.0f, 0.02f));  // Larger window
    glassWindow.setAsTintedGlass();
    glassWindow.setup();

    // Create light - positioned higher for larger room
    LightSource lightSource(glm::vec3(0.0f, 6.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.8f));
    lightSource.setup();
    lightSource.setAmbientStrength(0.3f);  // Brighter for larger space

    // Create decorative elements for the showroom
    unsigned int cubeVAO, cubeVBO, cubeEBO;
    float cubeVertices[] = {
        // Position              // Color
        -1.0f, 0.0f, -1.0f,      0.8f, 0.8f, 0.8f,  // Gray platform
         1.0f, 0.0f, -1.0f,      0.8f, 0.8f, 0.8f,
         1.0f, 0.1f, -1.0f,      0.9f, 0.9f, 0.9f,
        -1.0f, 0.1f, -1.0f,      0.9f, 0.9f, 0.9f,

        -1.0f, 0.0f,  1.0f,      0.8f, 0.8f, 0.8f,
         1.0f, 0.0f,  1.0f,      0.8f, 0.8f, 0.8f,
         1.0f, 0.1f,  1.0f,      0.9f, 0.9f, 0.9f,
        -1.0f, 0.1f,  1.0f,      0.9f, 0.9f, 0.9f
    };

    unsigned int cubeIndices[] = {
        0, 1, 2, 2, 3, 0, // Top
        4, 5, 6, 6, 7, 4, // Bottom
        0, 3, 7, 7, 4, 0, // Left
        1, 2, 6, 6, 5, 1, // Right
        0, 1, 5, 5, 4, 0, // Front
        2, 3, 7, 7, 6, 2  // Back
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

        // Update driver seat camera if active
        if (cameraInCar) {
            updateDriverSeatCamera();
        }

        // Animate light if enabled
        if (lightMoving) {
            lightAngle += 0.3f * deltaTime;  // Slower rotation for larger room
            glm::vec3 newLightPos = glm::vec3(
                sin(lightAngle) * 8.0f,    // Larger circle
                6.0f + sin(lightAngle * 1.5f) * 1.0f,
                cos(lightAngle) * 8.0f
            );
            lightSource.setPosition(newLightPos);
        }

        processInput(window, room, lightSource, glassWindow);

        glClearColor(0.15f, 0.15f, 0.2f, 1.0f);  // Darker blue for showroom
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(fov), 1000.0f / 800.0f, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        glDepthMask(GL_FALSE); // Disable depth writing for skybox
        skybox.draw(skyboxShader.ID, view, projection);
        glDepthMask(GL_TRUE); // Re-enable depth writing


        //if (showroomFloor) {
        //    texturedShader.use();
        //    texturedShader.setMat4("projection", projection);
        //    texturedShader.setMat4("view", view);

        //    // Set lighting uniforms for textured shader
        //    texturedShader.setVec3("lightPos", lightSource.getPosition());
        //    texturedShader.setVec3("viewPos", cameraPos);
        //    texturedShader.setVec3("lightColor", lightSource.getColor());
        //    texturedShader.setFloat("ambientStrength", 0.3f);
        //    texturedShader.setFloat("specularStrength", 0.5f);
        //    texturedShader.setInt("shininess", 32);
        //    texturedShader.setFloat("constant", 1.0f);
        //    texturedShader.setFloat("linear", 0.09f);
        //    texturedShader.setFloat("quadratic", 0.032f);

        //    showroomFloor->draw(texturedShader.ID);
        //}

        // 1. Draw room with lighting
        lightingShader.use();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        lightingShader.setMat4("model", glm::mat4(1.0f));
        lightSource.updateShader(lightingShader.ID, cameraPos);
        room.draw();

        // 2. Draw car platforms
        lightingShader.use();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // Platform for Porsche
        glm::mat4 platform = glm::mat4(1.0f);
        platform = glm::translate(platform, glm::vec3(-4.0f, 0.0f, 0.0f));
        platform = glm::scale(platform, glm::vec3(3.0f, 1.0f, 5.0f));
        lightingShader.setMat4("model", platform);
        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // Platform for Koenigsegg
        platform = glm::mat4(1.0f);
        platform = glm::translate(platform, glm::vec3(4.0f, 0.0f, 0.0f));
        platform = glm::scale(platform, glm::vec3(3.0f, 1.0f, 5.0f));
        lightingShader.setMat4("model", platform);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // 3. Draw the cars (NO ROTATION)
        lightingShader.use();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // Draw Porsche (left side)
        if (porscheLoaded && porsche) {
            porsche->Draw(lightingShader);
        }
        else {
            // Placeholder for Porsche
            glm::mat4 placeholder = glm::mat4(1.0f);
            placeholder = glm::translate(placeholder, glm::vec3(-4.0f, 0.5f, 0.0f));
            placeholder = glm::scale(placeholder, glm::vec3(1.5f, 0.8f, 3.0f));
            lightingShader.setMat4("model", placeholder);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }

        // Draw Koenigsegg (right side)
        if (koenigseggLoaded && koenigsegg) {
            koenigsegg->Draw(lightingShader);
        }
        else {
            // Placeholder for Koenigsegg
            glm::mat4 placeholder = glm::mat4(1.0f);
            placeholder = glm::translate(placeholder, glm::vec3(4.0f, 0.5f, 0.0f));
            placeholder = glm::scale(placeholder, glm::vec3(1.5f, 0.8f, 3.0f));
            lightingShader.setMat4("model", placeholder);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }

        // 4. Draw decorative elements behind window
        lightingShader.use();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 2.0f, 10.0f));
        model = glm::scale(model, glm::vec3(5.0f, 3.0f, 1.0f));
        lightingShader.setMat4("model", model);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // 5. Draw the light source
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);

        model = glm::mat4(1.0f);
        model = glm::translate(model, lightSource.getPosition());
        model = glm::scale(model, glm::vec3(0.3f));  // Slightly larger light
        lightCubeShader.setMat4("model", model);
        lightCubeShader.setVec3("lightColor", lightSource.getColor());
        lightSource.draw(lightCubeShader.ID);

        // 6. Draw glass window
        if (glassVisible) {
            glassShader.use();
            glassShader.setMat4("projection", projection);
            glassShader.setMat4("view", view);

            model = glm::mat4(1.0f);
            model = glm::translate(model, glassWindow.getPosition());
            glassShader.setMat4("model", model);

            glassWindow.updateShader(glassShader.ID,
                lightSource.getPosition(),
                cameraPos,
                lightSource.getColor(),
                0.3f);

            glDepthMask(GL_FALSE);
            glassWindow.draw(glassShader.ID);
            glDepthMask(GL_TRUE);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    skybox.cleanup();
    room.cleanup();
    lightSource.cleanup();
    glassWindow.cleanup();
    if (porsche) delete porsche;
    if (koenigsegg) delete koenigsegg;
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &cubeEBO);
    glfwTerminate();

    return 0;
}

void printCarInfo() {
    std::cout << "\n=== CAR SHOWROOM ===" << std::endl;
    std::cout << "Porsche 911 GT2: " << (porscheLoaded ? "Loaded" : "Not Loaded") << std::endl;
    std::cout << "Koenigsegg: " << (koenigseggLoaded ? "Loaded" : "Not Loaded") << std::endl;
    std::cout << "Selected Car: " << (selectedCar == 0 ? "Porsche" : "Koenigsegg") << std::endl;
    std::cout << "Camera Mode: ";
    switch (cameraMode) {
    case FREE_CAMERA: std::cout << "Free Camera"; break;
    case PORSCHE_DRIVER_SEAT: std::cout << "Porsche Driver Seat"; break;
    case KOENIGSEGG_DRIVER_SEAT: std::cout << "Koenigsegg Driver Seat"; break;
    }
    std::cout << std::endl;
}

// Enhanced input processing with driver seat controls
void processInput(GLFWwindow* window, Room& room, LightSource& light, GlassWindow& glass) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        // If in driver seat, exit to free camera
        if (cameraInCar) {
            cameraMode = FREE_CAMERA;
            cameraInCar = false;
            std::cout << "Exited driver's seat view" << std::endl;
        }
        else {
            glfwSetWindowShouldClose(window, true);
        }
    }

    static bool mPressed = false;
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && !mPressed) {
        mPressed = true;

        if (selectedCar == 0) {
            porscheDriverSeatPos = cameraPos;
            std::cout << "Porsche driver seat position set to: ("
                << cameraPos.x << ", " << cameraPos.y << ", " << cameraPos.z << ")" << std::endl;
        }
        else {
            koenigseggDriverSeatPos = cameraPos;
            std::cout << "Koenigsegg driver seat position set to: ("
                << cameraPos.x << ", " << cameraPos.y << ", " << cameraPos.z << ")" << std::endl;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE) {
        mPressed = false;
    }

    // Helper to find driver seat position (F key)
    static bool f3Pressed = false;
    if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS && !f3Pressed) {
        f3Pressed = true;
        findDriverSeatPosition();
    }
    if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_RELEASE) {
        f3Pressed = false;
    }

    // DRIVER SEAT TOGGLE CONTROLS (F1 and F2 keys)
    static bool f1Pressed = false;
    static bool f2Pressed = false;

    if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS && !f1Pressed) {
        f1Pressed = true;
        toggleDriverSeatView(0);
    }
    if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_RELEASE) {
        f1Pressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS && !f2Pressed) {
        f2Pressed = true;
        toggleDriverSeatView(1);
    }
    if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_RELEASE) {
        f2Pressed = false;
    }

    // Exit driver seat with E key
    static bool ePressed = false;
    if (cameraInCar && glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && !ePressed) {
        ePressed = true;
        cameraMode = FREE_CAMERA;
        cameraInCar = false;
        std::cout << "Exited driver's seat view" << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) {
        ePressed = false;
    }

    // DRIVER SEAT ADJUSTMENT CONTROLS
    if (cameraInCar) {
        float adjustSpeed = 0.0005f;

        if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
            if (cameraMode == PORSCHE_DRIVER_SEAT) {
                porscheDriverOffset.y += adjustSpeed;
                std::cout << "Porsche camera raised" << std::endl;
            }
            else {
                koenigseggDriverOffset.y += adjustSpeed;
                std::cout << "Koenigsegg camera raised" << std::endl;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
            if (cameraMode == PORSCHE_DRIVER_SEAT) {
                porscheDriverOffset.y -= adjustSpeed;
                std::cout << "Porsche camera lowered" << std::endl;
            }
            else {
                koenigseggDriverOffset.y -= adjustSpeed;
                std::cout << "Koenigsegg camera lowered" << std::endl;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
            if (cameraMode == PORSCHE_DRIVER_SEAT) {
                porscheDriverOffset.x -= adjustSpeed;
                std::cout << "Porsche camera moved left" << std::endl;
            }
            else {
                koenigseggDriverOffset.x -= adjustSpeed;
                std::cout << "Koenigsegg camera moved left" << std::endl;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
            if (cameraMode == PORSCHE_DRIVER_SEAT) {
                porscheDriverOffset.x += adjustSpeed;
                std::cout << "Porsche camera moved right" << std::endl;
            }
            else {
                koenigseggDriverOffset.x += adjustSpeed;
                std::cout << "Koenigsegg camera moved right" << std::endl;
            }
        }

        // Print current offsets for debugging
        static bool pPressed = false;
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && !pPressed) {
            pPressed = true;
            if (cameraMode == PORSCHE_DRIVER_SEAT) {
                std::cout << "Porsche driver seat offset: ("
                    << porscheDriverOffset.x << ", "
                    << porscheDriverOffset.y << ", "
                    << porscheDriverOffset.z << ")" << std::endl;
            }
            else {
                std::cout << "Koenigsegg driver seat offset: ("
                    << koenigseggDriverOffset.x << ", "
                    << koenigseggDriverOffset.y << ", "
                    << koenigseggDriverOffset.z << ")" << std::endl;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE) {
            pPressed = false;
        }
    }

    // CAR SELECTION (1 and 2 keys) - only works in free camera mode
    static bool onePressed = false;
    static bool twoPressed = false;

    if (cameraMode == FREE_CAMERA) {
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !onePressed) {
            onePressed = true;
            selectedCar = 0;
            std::cout << "Selected: Porsche 911 GT2" << std::endl;
        }
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE) {
            onePressed = false;
        }

        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && !twoPressed) {
            twoPressed = true;
            selectedCar = 1;
            std::cout << "Selected: Koenigsegg" << std::endl;
        }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE) {
            twoPressed = false;
        }
    }

    // Get currently selected car
    Car* currentCar = (selectedCar == 0) ? porsche : koenigsegg;
    bool currentCarLoaded = (selectedCar == 0) ? porscheLoaded : koenigseggLoaded;

    // CAR CONTROLS (only for selected car, works in both modes)
    static bool cPressed = false;
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !cPressed) {
        cPressed = true;
        if (currentCar && currentCarLoaded) {
            glm::vec3 pos = currentCar->GetPosition();
            std::cout << "\n=== " << (selectedCar == 0 ? "PORSCHE" : "KOENIGSEGG") << " INFO ===" << std::endl;
            std::cout << "Position: (" << pos.x << ", " << pos.y << ", " << pos.z << ")" << std::endl;
            std::cout << "Rotation: " << currentCar->GetRotationAngle() << " degrees" << std::endl;
            std::cout << "Use arrow keys to move, F/G to scale" << std::endl;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE) {
        cPressed = false;
    }

    // Car movement controls (arrow keys)
    float carSpeed = 3.0f * deltaTime;
    if (currentCar && currentCarLoaded) {
        glm::vec3 carPos = currentCar->GetPosition();

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            carPos.z -= carSpeed;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            carPos.z += carSpeed;
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            carPos.x -= carSpeed;
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            carPos.x += carSpeed;

        currentCar->SetPosition(carPos);

        // Car scale controls (F and G keys)
        static bool fPressed = false;
        static bool gPressed = false;
        static float porscheScale = 0.8f;
        static float koenigseggScale = 0.08f;

        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !fPressed) {
            fPressed = true;
            if (selectedCar == 0) {
                porscheScale *= 1.1f;
                if (porscheScale > 2.0f) porscheScale = 2.0f;
                porsche->SetScale(glm::vec3(porscheScale));
                std::cout << "Porsche scale: " << porscheScale << std::endl;
            }
            else {
                koenigseggScale *= 1.1f;
                if (koenigseggScale > 0.1f) koenigseggScale = 0.1f;
                koenigsegg->SetScale(glm::vec3(koenigseggScale));
                std::cout << "Koenigsegg scale: " << koenigseggScale << std::endl;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE) {
            fPressed = false;
        }

        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS && !gPressed) {
            gPressed = true;
            if (selectedCar == 0) {
                porscheScale *= 0.9f;
                if (porscheScale < 0.1f) porscheScale = 0.1f;
                porsche->SetScale(glm::vec3(porscheScale));
                std::cout << "Porsche scale: " << porscheScale << std::endl;
            }
            else {
                koenigseggScale *= 0.9f;
                if (koenigseggScale < 0.001f) koenigseggScale = 0.001f;
                koenigsegg->SetScale(glm::vec3(koenigseggScale));
                std::cout << "Koenigsegg scale: " << koenigseggScale << std::endl;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE) {
            gPressed = false;
        }
    }

    // TRANSPARENCY CONTROLS - COMPLETE VERSION

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
        static bool onePressed2 = false;
        if (!onePressed2) {
            onePressed2 = true;
            glass.setSemiTransparent();
            if (showTransparencyInfo) {
                std::cout << "Set to SEMI-TRANSPARENT (50%)" << std::endl;
            }
        }
    }
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE) {
        static bool oneReleased2 = false;
        if (!oneReleased2) {
            oneReleased2 = true;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        static bool twoPressed2 = false;
        if (!twoPressed2) {
            twoPressed2 = true;
            glass.setSlightlyTransparent();
            if (showTransparencyInfo) {
                std::cout << "Set to SLIGHTLY TRANSPARENT (20%)" << std::endl;
            }
        }
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE) {
        static bool twoReleased2 = false;
        if (!twoReleased2) {
            twoReleased2 = true;
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

    // CAMERA MOVEMENT - Only in free camera mode
    if (cameraMode == FREE_CAMERA) {
        float cameraSpeed = 8.0f * deltaTime;
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

        // DEBUG: Print camera position to see if it's changing
        static bool spacePressed = false;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !spacePressed) {
            spacePressed = true;
            std::cout << "Free Camera Position: (" << cameraPos.x << ", "
                << cameraPos.y << ", " << cameraPos.z << ")" << std::endl;
            std::cout << "Camera Front: (" << cameraFront.x << ", "
                << cameraFront.y << ", " << cameraFront.z << ")" << std::endl;
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
            spacePressed = false;
        }

        // Update camera position WITHOUT collision detection for now
        cameraPos = newPos;
    }
}

void printTransparencyControls() {
    std::cout << "\n=============================================\n";
    std::cout << "          CAR SHOWROOM CONTROLS\n";
    std::cout << "=============================================\n";
    std::cout << "      CAR SELECTION & MOVEMENT\n";
    std::cout << "  [1]     : Select Porsche 911 GT2 (left)\n";
    std::cout << "  [2]     : Select Koenigsegg (right)\n";
    std::cout << "  [F1]    : Toggle Porsche driver seat view\n";
    std::cout << "  [F2]    : Toggle Koenigsegg driver seat view\n";
    std::cout << "  [ESC/E] : Exit driver seat view\n";
    std::cout << "  [C]     : Show selected car info\n";
    std::cout << "  [↑↓←→]  : Move selected car\n";
    std::cout << "  [F/G]   : Increase/decrease car size\n";
    std::cout << "\n      DRIVER SEAT SETUP\n";
    std::cout << "  [F3]    : Help for finding driver seat position\n";
    std::cout << "  [M]     : Set current position as driver seat (for selected car)\n";
    std::cout << "\n      GLASS WINDOW CONTROLS\n";
    std::cout << "  [+]/[=] : Increase transparency\n";
    std::cout << "  [-]     : Decrease transparency\n";
    std::cout << "  [0-3]   : Transparency presets\n";
    std::cout << "  [T]     : Toggle transparency info\n";
    std::cout << "  [I]     : Print current transparency\n";
    std::cout << "\n      CAMERA CONTROLS\n";
    std::cout << "  Mouse   : Look around (works in all modes)\n";
    std::cout << "  [WASD]  : Move camera (free mode only)\n";
    std::cout << "  [QE]    : Move up/down (free mode only)\n";
    std::cout << "  [ESC]   : Exit program (in free mode)\n";
    std::cout << "=============================================\n\n";
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
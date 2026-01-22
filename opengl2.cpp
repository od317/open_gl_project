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
#include "street.h"
#include "door.h"
#include "tree.h"
// Camera and input variables
glm::vec3 cameraPos = glm::vec3(0.0f, 8.0f, 20.0f);  // Start further back
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool firstMouse = true;
float yaw = -90.0f, pitch = 0.0f;
float lastX = 400.0f, lastY = 300.0f;
float fov = 60.0f;


Car* trafficCar = nullptr;
bool trafficCarLoaded = false;
float trafficCarSpeed = 15.0f;  // Speed for traffic car
float trafficCarPosition = -100.0f;  // Start position (at beginning of street)
bool trafficMoving = true;  // Control whether traffic moves


Car* trafficCar2 = nullptr;  // Second traffic car
bool trafficCar2Loaded = false;
float trafficCar2Position = 100.0f;  // Start at the opposite end
float trafficCar2Speed = 12.0f;  // Slightly different speed for realism

Door* leftDoor = nullptr;
Door* rightDoor = nullptr;
bool doorsLoaded = false;

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
glm::vec3 porscheDriverSeatPos = glm::vec3(-4.07038f, 1.3f, 3.329982f);  // Adjust these!
glm::vec3 koenigseggDriverSeatPos = glm::vec3(4.2f, 0.9f, 0.0f);  // Adjust these!

float treeScale = 0.1f;

// Store original camera position when entering driver seat
glm::vec3 savedCameraPos;
glm::vec3 savedCameraFront;

// Light source animation
float lightAngle = 0.0f;

// Control flags
bool showControls = true;
bool glassVisible = true;
bool lightMoving = true;



glm::vec3 porscheColor = glm::vec3(1.0f, 0.0f, 0.0f);  // Red
glm::vec3 koenigseggColor = glm::vec3(0.0f, 0.0f, 1.0f);  // Blue
glm::vec3 trafficCarColor = glm::vec3(0.0f, 1.0f, 0.0f);  // Green
glm::vec3 trafficCar2Color = glm::vec3(1.0f, 1.0f, 0.0f);  // Yellow

// Car variables - TWO CARS!
Car* porsche = nullptr;
Car* koenigsegg = nullptr;
bool porscheLoaded = false;
bool koenigseggLoaded = false;

Street* mainStreet = nullptr;
float streetRotation = 90.0f;  // ADD THIS: Rotate street by 90 degrees
bool streetCreated = false;
std::vector<StreetLight*> streetLights;
std::vector<glm::vec3> streetLightPositions;
std::vector<glm::vec3> streetLightColors;
std::vector<GlassWindow> sideWindows;
std::vector<GlassWindow> frontWindows;


std::vector<Tree*> trees;
std::vector<glm::vec3> treePositions;
// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window, Room& room, LightSource& light, GlassWindow& glass);
void printTransparencyControls();
void printCarInfo();
void toggleDriverSeatView(int carIndex);
void updateDriverSeatCamera();
Tree* loadTreeModel(const glm::vec3& position, float scale,
    const std::vector<std::string>& paths,
    const glm::vec3& color = glm::vec3(0.0f, 0.5f, 0.0f));
void updateMultipleLights(Shader& shader, const std::vector<glm::vec3>& positions,
    const std::vector<glm::vec3>& colors, const glm::vec3& viewPos);

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

    std::vector<std::string> trafficCarPaths = {
    "models/Porsche_911_GT2/Porsche_911_GT2.obj",
    "../models/Porsche_911_GT2/Porsche_911_GT2.obj",
    "C:/projects/university/opengl/opengl2/models/Porsche_911_GT2/Porsche_911_GT2.obj",
    "C:/Users/HP/Downloads/Porsche_911_GT2.obj"
    };

    std::vector<std::string> trafficCar2Paths = {
    "models/uploads_files_2792345_Koenigsegg.obj",
    "../models/uploads_files_2792345_Koenigsegg.obj",
    "C:/projects/university/opengl/opengl2/models/uploads_files_2792345_Koenigsegg.obj",
    "C:/Users/HP/Downloads/82-koenigsegg-agera/uploads_files_2792345_Koenigsegg.obj"
    };

    porsche = loadCarModel("Porsche 911 GT2", porschePaths, glm::vec3(-4.0f, 1.0f, 0.0f), 0.8f);
    trafficCar2 = loadCarModel("Traffic Car 2 (Koenigsegg)", trafficCar2Paths, glm::vec3(0.0f, 0.3f, 0.0f), 0.15f);
    trafficCar2Loaded = (trafficCar2 != nullptr);
    porscheLoaded = (porsche != nullptr);

    if (porscheLoaded) {
        porsche->SetRotation(180.0f);  // Face forward
        porsche->SetPosition(glm::vec3(-10.0f, 0.7f, 0.0f));  // Left exhibition platform
    }

    // Load Koenigsegg
    std::vector<std::string> koenigseggPaths = {
        "models/uploads_files_2792345_Koenigsegg.obj",
        "../models/uploads_files_2792345_Koenigsegg.obj",
        "C:/projects/university/opengl/opengl2/models/uploads_files_2792345_Koenigsegg.obj",
        "C:/Users/HP/Downloads/82-koenigsegg-agera/uploads_files_2792345_Koenigsegg.obj"
    };

    std::vector<std::string> treePaths = {
    "models/Tree.obj",  // Your tree model
    "Tree.obj",
    "../models/Tree.obj",
    "C:/projects/university/opengl/opengl2/models/Tree.obj"
    };


    std::cout << "Checking tree model paths:" << std::endl;
    for (const auto& path : treePaths) {
        if (fileExists(path)) {
            std::cout << "Found: " << path << std::endl;
        }
        else {
            std::cout << "Missing: " << path << std::endl;
        }
    }

    koenigsegg = loadCarModel("Koenigsegg", koenigseggPaths, glm::vec3(4.0f, 0.3f, 0.0f), 0.08f);
    koenigseggLoaded = (koenigsegg != nullptr);

    if (koenigseggLoaded) {
        koenigsegg->SetRotation(180.0f);  // Face forward
        koenigsegg->SetPosition(glm::vec3(10.0f, 0.3f, 0.0f));  // Right exhibition platform
    }

    trafficCar = loadCarModel("Traffic Car", trafficCarPaths, glm::vec3(0.0f, 0.3f, 0.0f), 1.5f);
    trafficCarLoaded = (trafficCar != nullptr);

    if (trafficCarLoaded) {
        // Street is rotated 90°, so it runs along X-axis
        // Street center is at X=8.0f, length is 200 units
        // Car should face 270° (or -90°) to move from left to right
        trafficCar->SetRotation(270.0f);  // Changed from 90.0f to 270.0f

        // Position at the beginning of the street (left side)
        // Street runs from X = 8.0f - 100.0f to X = 8.0f + 100.0f
        trafficCarPosition = -100.0f;  // Start at left end

        // INCREASE Y POSITION SIGNIFICANTLY - try 0.5f instead of 0.15f
        // Also put it in a lane (not center of street)
        glm::vec3 trafficStartPos = glm::vec3(8.0f + trafficCarPosition, 0.5f, 45.0f + 8.0f);
        // Y = 0.5f (higher), Z = 45.0f + 8.0f (right lane)

        trafficCar->SetPosition(trafficStartPos);

        std::cout << "Traffic car loaded and positioned at street start" << std::endl;
        std::cout << "Position: (" << trafficStartPos.x << ", " << trafficStartPos.y << ", " << trafficStartPos.z << ")" << std::endl;
        std::cout << "Rotation: 270° (facing left to right along street)" << std::endl;
        std::cout << "Street center: X=8.0, Y=0.0, Z=45.0" << std::endl;
    }

    if (trafficCar2Loaded) {
        // This car will move from right to left (opposite direction)
        // But if it's moving backward, we need to fix the rotation

        // Let's try facing 270° to move from left to right (same as trafficCar)
        trafficCar2->SetRotation(270.0f);  // CHANGED from 90.0f to 270.0f

        // Position at the right end of the street
        // Street runs from X = 8.0f - 100.0f to X = 8.0f + 100.0f
        trafficCar2Position = 100.0f;  // Start at right end

        // Place on left lane (opposite side from first car) with correct Y position
        glm::vec3 trafficStartPos2 = glm::vec3(8.0f + trafficCar2Position, 0.5f, 45.0f - 8.0f);
        // Y = 0.5f (same as trafficCar), Z = 45.0f - 8.0f (left lane)

        trafficCar2->SetPosition(trafficStartPos2);

        std::cout << "Second traffic car (Koenigsegg) loaded" << std::endl;
        std::cout << "Position: (" << trafficStartPos2.x << ", " << trafficStartPos2.y << ", " << trafficStartPos2.z << ")" << std::endl;
        std::cout << "Rotation: 270° (facing left to right along street)" << std::endl;
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

    // Create HUGE exhibition hall for car showcase
    Room room(40.0f, 15.0f, 30.0f);  // Width: 40m, Height: 15m, Depth: 30m - EXHIBITION SIZE!
    room.setWallColor(glm::vec3(1.0f, 1.0f, 1.0f));      // PURE WHITE walls
    room.setFloorColor(glm::vec3(0.95f, 0.95f, 0.95f));  // Very light gray floor
    room.setCeilingColor(glm::vec3(1.0f, 1.0f, 1.0f));   // WHITE ceiling
    room.setup();

    leftDoor = new Door(glm::vec3(-2.5f, 3.5f, room.getDepth() / 2.0f - 0.01f),
        glm::vec3(4.0f, 7.0f, 0.1f), true);
    rightDoor = new Door(glm::vec3(2.5f, 3.5f, room.getDepth() / 2.0f - 0.01f),
        glm::vec3(4.0f, 7.0f, 0.1f), false);

    leftDoor->setup();
    rightDoor->setup();
    doorsLoaded = true;

    // Create glass window for the exhibition (much larger)
    glm::vec3 windowPos = room.getWindowPosition();
    windowPos.z += 0.05f;  // Bring it slightly forward
    GlassWindow glassWindow(windowPos, glm::vec3(25.0f, 8.0f, 0.02f));  // Large exhibition window
    glassWindow.setAsTintedGlass();
    glassWindow.setup();

        float halfDepth = room.getDepth() / 2.0f;  // Changed from roomDepth to room.getDepth()

        float roomHalfDepth = room.getDepth() / 2.0f;  // Use different name to avoid conflict
        float roomHalfWidth = room.getWidth() / 2.0f;  // Get half width

        float windowSpacing = 5.0f;
        float windowHeight = 4.0f;
        float windowWidth = 3.0f;

        // Clear any existing windows
        sideWindows.clear();
        frontWindows.clear();

        // Left side windows
        for (int i = 0; i < 3; i++) {
            float windowCenterZ = -roomHalfDepth + 2.5f + i * windowSpacing;
            glm::vec3 windowPos(-roomHalfWidth + 0.02f, 4.0f, windowCenterZ); // Slightly in front of wall
            GlassWindow sideWindow(windowPos, glm::vec3(windowWidth, windowHeight, 0.02f));
            sideWindow.setAsTintedGlass();
            sideWindow.setup();
            sideWindows.push_back(sideWindow);
        }

        // Right side windows
        for (int i = 0; i < 3; i++) {
            float windowCenterZ = -roomHalfDepth + 2.5f + i * windowSpacing;
            glm::vec3 windowPos(roomHalfWidth - 0.02f, 4.0f, windowCenterZ); // Slightly in front of wall
            GlassWindow sideWindow(windowPos, glm::vec3(windowWidth, windowHeight, 0.02f));
            sideWindow.setAsTintedGlass();
            sideWindow.setup();
            sideWindows.push_back(sideWindow);
        }


        for (int i = 0; i < 2; i++) {
            float windowCenterX = -12.0f + i * 8.0f;  // Adjusted spacing
            glm::vec3 windowPos(windowCenterX, 4.0f, roomHalfDepth + 0.02f); // Slightly in front of wall
            GlassWindow frontWindow(windowPos, glm::vec3(3.0f, 4.0f, 0.02f));
            frontWindow.setAsTintedGlass();
            frontWindow.setup();
            frontWindows.push_back(frontWindow);
        }

        // Windows on right side of entrance
        for (int i = 0; i < 2; i++) {
            float windowCenterX = 12.0f - i * 8.0f;  // Mirror of left side
            glm::vec3 windowPos(windowCenterX, 4.0f, roomHalfDepth + 0.02f); // Slightly in front of wall
            GlassWindow frontWindow(windowPos, glm::vec3(3.0f, 4.0f, 0.02f));
            frontWindow.setAsTintedGlass();
            frontWindow.setup();
            frontWindows.push_back(frontWindow);
        }



    // Create main light - positioned high for exhibition hall
    LightSource lightSource(glm::vec3(0.0f, 12.0f, 0.0f), glm::vec3(1.2f, 1.2f, 1.0f));  // Bright warm light
    lightSource.setup();
    lightSource.setAmbientStrength(0.8f);    // Much brighter ambient
    lightSource.setSpecularStrength(1.2f);   // Brighter specular

    // Create street in front of the exhibition hall
    mainStreet = new Street(
        glm::vec3(8.0f, 0.0f, 45.0f),  // Position (moved further) - CHANGE THIS
        glm::vec3(40.0f, 0.1f, 200.0f), // Dimensions
        glm::vec3(0.05f, 0.05f, 0.05f) // BLACK street color (very dark)
    );

    // Clear light position vectors
    streetLightPositions.clear();
    streetLightColors.clear();

    // Create street lights along the street
    for (int i = -8; i <= 8; i++) {  // CHANGED: from -4..4 to -8..8 (more lights)
        float xPos = static_cast<float>(i) * 5.0f + 8.0f;  // ADD +8.0f to match street X position

        // Since street is rotated 90 degrees, sidewalk is along what was originally Z-axis
        // Place lights on both sides of the rotated street
        glm::vec3 lightPosLeft = glm::vec3(xPos, 0.0f, 45.0f - 20.0f);   // Left sidewalk: 20 units from center in -Z
        glm::vec3 lightPosRight = glm::vec3(xPos, 0.0f, 45.0f + 20.0f);  // Right sidewalk: 20 units from center in +Z

        streetLights.push_back(new StreetLight(lightPosLeft));
        streetLights.push_back(new StreetLight(lightPosRight));

        // For lighting calculations, the light source is at height
        streetLightPositions.push_back(glm::vec3(xPos, 3.0f, 45.0f - 20.0f));  // Light at 3m height
        streetLightPositions.push_back(glm::vec3(xPos, 3.0f, 45.0f + 20.0f));  // Light at 3m height
        streetLightColors.push_back(glm::vec3(1.0f, 1.0f, 0.8f));  // Warm light
        streetLightColors.push_back(glm::vec3(1.0f, 1.0f, 0.8f));  // Warm light
    }

    for (int i = -8; i <= 8; i++) {
        float xPos = static_cast<float>(i) * 5.0f + 8.0f;

        // Alternate trees on left and right sides
        for (int side = -1; side <= 1; side += 2) {
            if ((i + side) % 3 == 0) {  // Create trees every 3 positions
                float zOffset = 45.0f + (22.0f * side);  // Same position as street lights

                // Add some randomness to tree positions
                float randomX = (rand() % 100) / 100.0f - 0.5f;
                float randomZ = (rand() % 100) / 100.0f - 0.5f;

                glm::vec3 treePos = glm::vec3(xPos + randomX, 0.0f, zOffset + randomZ);

                // Random tree color variations (different shades of green)
                glm::vec3 treeColor;
                int colorChoice = rand() % 3;
                switch (colorChoice) {
                case 0: treeColor = glm::vec3(0.0f, 0.5f, 0.0f); break;  // Standard green
                case 1: treeColor = glm::vec3(0.1f, 0.6f, 0.1f); break;  // Bright green
                case 2: treeColor = glm::vec3(0.0f, 0.4f, 0.1f); break;  // Dark green
                }

                // Random scale between 0.3 and 0.8
                float treeScale = 0.3f + (rand() % 50) / 100.0f;

                // Load tree model
                Tree* tree = loadTreeModel(treePos, treeScale, treePaths);




                if (tree) {
                    trees.push_back(tree);
                    treePositions.push_back(treePos);
                }
            }
        }
    }



    std::cout << "Created " << trees.size() << " trees along the street" << std::endl;

    // Add the main hall light to lighting calculations
    streetLightPositions.push_back(lightSource.getPosition());
    streetLightColors.push_back(lightSource.getColor());

    // Add a general ambient light for the street area
    streetLightPositions.push_back(glm::vec3(8.0f, 8.0f, 45.0f));  // Changed from (0.0f, 8.0f, 45.0f)
    streetLightColors.push_back(glm::vec3(1.0f, 1.0f, 0.9f));

    // Store index of animated light (main hall light)
    int animatedLightIndex = streetLightPositions.size() - 2; // Second to last

    // Create decorative elements for the showroom
    unsigned int cubeVAO, cubeVBO, cubeEBO;
    float cubeVertices[] = {
        // Position              // Color
        -1.0f, 0.0f, -1.0f,      0.9f, 0.9f, 0.9f,  // Light gray platform
         1.0f, 0.0f, -1.0f,      0.9f, 0.9f, 0.9f,
         1.0f, 0.1f, -1.0f,      1.0f, 1.0f, 1.0f,
        -1.0f, 0.1f, -1.0f,      1.0f, 1.0f, 1.0f,

        -1.0f, 0.0f,  1.0f,      0.9f, 0.9f, 0.9f,
         1.0f, 0.0f,  1.0f,      0.9f, 0.9f, 0.9f,
         1.0f, 0.1f,  1.0f,      1.0f, 1.0f, 1.0f,
        -1.0f, 0.1f,  1.0f,      1.0f, 1.0f, 1.0f
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
        float currentFrame = static_cast<float>(glfwGetTime());
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

            room.updateDoors(deltaTime);

            // Update traffic car position (automatic movement)
               // Update traffic car position (automatic movement) - MOVE THIS OUTSIDE THE LIGHT ANIMATION!
            if (trafficCarLoaded && trafficMoving) {
                // Street is rotated 90°, so it runs along X-axis
                // Move from X = 8.0f - 100.0f to X = 8.0f + 100.0f

                trafficCarPosition += trafficCarSpeed * deltaTime;

                // Reset to beginning when reaching end
                if (trafficCarPosition > 100.0f) {
                    trafficCarPosition = -100.0f;
                }

                // Update traffic car position - move along X-axis
                // Increased Y position to 1.0f for bigger car
                glm::vec3 trafficPos = glm::vec3(8.0f + trafficCarPosition, 1.0f, 45.0f + 8.0f);
                trafficCar->SetPosition(trafficPos);

                // Car should face along the street direction
                trafficCar->SetRotation(270.0f);
            }

            if (trafficCar2Loaded && trafficMoving) {
                // This car moves from right to left (opposite direction)
                // Since we're now facing 270°, we need to move in the opposite direction

                // Move from right to left (negative X direction)
                trafficCar2Position -= trafficCar2Speed * deltaTime;

                // Reset to beginning when reaching end
                if (trafficCar2Position < -100.0f) {
                    trafficCar2Position = 100.0f;
                }

                // Update position - moves along X-axis in opposite direction
                glm::vec3 trafficPos2 = glm::vec3(8.0f + trafficCar2Position, 0.5f, 45.0f - 8.0f);
                trafficCar2->SetPosition(trafficPos2);

                // Car faces 270° to move from left to right (same as trafficCar)
                trafficCar2->SetRotation(270.0f);  // CHANGED from 90.0f to 270.0f
            }

            // Update the animated light position in our lighting array
            if (animatedLightIndex >= 0 && animatedLightIndex < streetLightPositions.size()) {
                streetLightPositions[animatedLightIndex] = newLightPos;
            }
        }

        processInput(window, room, lightSource, glassWindow);

        glClearColor(0.2f, 0.2f, 0.25f, 1.0f);  // BRIGHTER background
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(fov), 1000.0f / 800.0f, 0.1f, 200.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        glDepthMask(GL_FALSE); // Disable depth writing for skybox
        skybox.draw(skyboxShader.ID, view, projection);
        glDepthMask(GL_TRUE); // Re-enable depth writing



        // 1. Draw the street and outdoor environment FIRST (farthest objects)
        lightingShader.use();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // Update multiple lights (street lights + main light)
        updateMultipleLights(lightingShader, streetLightPositions, streetLightColors, cameraPos);

        // Draw street
        if (mainStreet) {
            glm::mat4 streetModel = glm::mat4(1.0f);
            streetModel = glm::translate(streetModel, glm::vec3(8.0f, 0.0f, 45.0f));  // Use the same position: (8.0f, 0.0f, 45.0f)
            streetModel = glm::rotate(streetModel, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            streetModel = glm::translate(streetModel, glm::vec3(-8.0f, 0.0f, -45.0f));  // Adjusted to -8.0f instead of 0.0f
            lightingShader.setMat4("model", streetModel);
            mainStreet->draw();
        }

        // Draw street lights
        for (size_t i = 0; i < streetLights.size(); i++) {
            glm::mat4 lightModel = glm::mat4(1.0f);

            // Street lights are already in correct world position
            // No rotation needed - they're on the sidewalk, not part of the street mesh
            glm::vec3 lightPos = streetLights[i]->getPosition();
            lightModel = glm::translate(lightModel, lightPos);

            lightingShader.setMat4("model", lightModel);
            streetLights[i]->draw();
        }

        // Draw trees
        for (size_t i = 0; i < trees.size(); i++) {
            trees[i]->draw(lightingShader);
        }

        lightingShader.use();
        lightingShader.setBool("useColorOverride", false);  // Reset this!
        lightingShader.setFloat("shininess", 32.0f);  // Reset any other uniforms

        // 2. Draw the exhibition hall
        lightingShader.setMat4("model", glm::mat4(1.0f));
        room.draw();

        // 3. Draw car platforms
        lightingShader.use();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // Platform for Porsche (updated position)
        glm::mat4 platform = glm::mat4(1.0f);
        platform = glm::translate(platform, glm::vec3(-10.0f, 0.0f, 0.0f));
        platform = glm::scale(platform, glm::vec3(8.0f, 0.2f, 15.0f));
        lightingShader.setMat4("model", platform);
        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // Platform for Koenigsegg (updated position)
        platform = glm::mat4(1.0f);
        platform = glm::translate(platform, glm::vec3(10.0f, 0.0f, 0.0f));
        platform = glm::scale(platform, glm::vec3(8.0f, 0.2f, 15.0f));
        lightingShader.setMat4("model", platform);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // 4. Draw the cars
        lightingShader.use();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        if (trafficCarLoaded && trafficCar) {
            trafficCar->Draw(lightingShader);
        }

        if (trafficCar2Loaded && trafficCar2) {
            trafficCar2->Draw(lightingShader);
        }

        // Draw Porsche (left side)
        if (porscheLoaded && porsche) {
            porsche->Draw(lightingShader);
        }
        else {
            // Placeholder for Porsche
            glm::mat4 placeholder = glm::mat4(1.0f);
            placeholder = glm::translate(placeholder, glm::vec3(-10.0f, 0.5f, 0.0f));
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
            placeholder = glm::translate(placeholder, glm::vec3(10.0f, 0.5f, 0.0f));
            placeholder = glm::scale(placeholder, glm::vec3(1.5f, 0.8f, 3.0f));
            lightingShader.setMat4("model", placeholder);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }

        // 5. Draw the light source (visual representation)
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, lightSource.getPosition());
        model = glm::scale(model, glm::vec3(0.3f));
        lightCubeShader.setMat4("model", model);
        lightCubeShader.setVec3("lightColor", lightSource.getColor());
        lightSource.draw(lightCubeShader.ID);

        // 6. Draw glass window (transparent, drawn last)
        if (glassVisible) {
            glassShader.use();
            glassShader.setMat4("projection", projection);
            glassShader.setMat4("view", view);

            // Draw main back window
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glassWindow.getPosition());
            glassShader.setMat4("model", model);
            glassWindow.updateShader(glassShader.ID, lightSource.getPosition(),
                cameraPos, lightSource.getColor(), 0.3f);

            glDepthMask(GL_FALSE);
            glassWindow.draw(glassShader.ID);


            // Draw front windows (on left and right sides of entrance)
            float roomHalfDepth = room.getDepth() / 2.0f;
            float roomHalfWidth = room.getWidth() / 2.0f;

            // Left side front window
            for (int i = 0; i < 2; i++) {
                float windowCenterX = -12.0f + i * 8.0f;
                glm::vec3 windowPos(windowCenterX, 4.0f, roomHalfDepth + 0.02f);

                // Create glass window for this position
                GlassWindow frontWindow(windowPos, glm::vec3(3.0f, 4.0f, 0.02f));
                frontWindow.setAsTintedGlass();
                frontWindow.setup();

                // Set up transformation
                glm::mat4 frontModel = glm::mat4(1.0f);
                frontModel = glm::translate(frontModel, windowPos);
                glassShader.setMat4("model", frontModel);

                // Update shader with lighting
                frontWindow.updateShader(glassShader.ID, lightSource.getPosition(),
                    cameraPos, lightSource.getColor(), 0.3f);

                // Draw the window
                frontWindow.draw(glassShader.ID);
            }

            // Right side front window
            for (int i = 0; i < 2; i++) {
                float windowCenterX = 12.0f - i * 8.0f;
                glm::vec3 windowPos(windowCenterX, 4.0f, roomHalfDepth + 0.02f);

                // Create glass window for this position
                GlassWindow frontWindow(windowPos, glm::vec3(3.0f, 4.0f, 0.02f));
                frontWindow.setAsTintedGlass();
                frontWindow.setup();

                // Set up transformation
                glm::mat4 frontModel = glm::mat4(1.0f);
                frontModel = glm::translate(frontModel, windowPos);
                glassShader.setMat4("model", frontModel);

                // Update shader with lighting
                frontWindow.updateShader(glassShader.ID, lightSource.getPosition(),
                    cameraPos, lightSource.getColor(), 0.3f);

                // Draw the window
                frontWindow.draw(glassShader.ID);
            }

            glDepthMask(GL_TRUE);
        }

        if (doorsLoaded) {
            lightingShader.use();
            lightingShader.setMat4("projection", projection);
            lightingShader.setMat4("view", view);

            // Update door animations
            leftDoor->update(deltaTime);
            rightDoor->update(deltaTime);

            // Draw doors
            leftDoor->draw(lightingShader);
            rightDoor->draw(lightingShader);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    skybox.cleanup();
    room.cleanup();
    lightSource.cleanup();
    glassWindow.cleanup();
    // Cleanup
    if (leftDoor) delete leftDoor;
    if (rightDoor) delete rightDoor;
    for (size_t i = 0; i < trees.size(); i++) {
        delete trees[i];
    }
    trees.clear();
    if (mainStreet) {
        mainStreet->cleanup();
        delete mainStreet;
    }

    // Cleanup street lights
    for (size_t i = 0; i < streetLights.size(); i++) {
        delete streetLights[i];
    }
    streetLights.clear();

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

    static bool dPressed = false;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && !dPressed) {
        dPressed = true;
        room.toggleDoors();
        std::cout << "Toggled doors" << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE) {
        dPressed = false;
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
    float carSpeed = 10.0f * deltaTime;  // Increased for street driving
    if (currentCar && currentCarLoaded) {
        glm::vec3 carPos = currentCar->GetPosition();

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            carPos.z += carSpeed;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            carPos.z -= carSpeed;
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            carPos.x -= carSpeed;
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            carPos.x += carSpeed;

        currentCar->SetPosition(carPos);
     
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

    static bool rPressed = false, gPressed = false, bPressed = false, yPressed = false;
    static bool wPressed = false, oPressed = false, pPressed2 = false;
    static bool resetPressed = false;

    // Red color
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && !rPressed) {
        rPressed = true;
        if (selectedCar == 0 && porsche) {
            porsche->SetColor(glm::vec3(1.0f, 0.0f, 0.0f));  // Red
            porscheColor = glm::vec3(1.0f, 0.0f, 0.0f);
        }
        else if (selectedCar == 1 && koenigsegg) {
            //koenigsegg->SetColor(glm::vec3(1.0f, 0.0f, 0.0f));
            //koenigseggColor = glm::vec3(1.0f, 0.0f, 0.0f);
        }
        std::cout << "Set car color to RED" << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE) rPressed = false;

    // Green color
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS && !gPressed) {
        gPressed = true;
        if (selectedCar == 0 && porsche) {
            porsche->SetColor(glm::vec3(0.0f, 1.0f, 0.0f));  // Green
            porscheColor = glm::vec3(0.0f, 1.0f, 0.0f);
        }
        else if (selectedCar == 1 && koenigsegg) {
            //koenigsegg->SetColor(glm::vec3(0.0f, 1.0f, 0.0f));
            //koenigseggColor = glm::vec3(0.0f, 1.0f, 0.0f);
        }
        std::cout << "Set car color to GREEN" << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE) gPressed = false;

    // Blue color
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !bPressed) {
        bPressed = true;
        if (selectedCar == 0 && porsche) {
            porsche->SetColor(glm::vec3(0.0f, 0.0f, 1.0f));  // Blue
            porscheColor = glm::vec3(0.0f, 0.0f, 1.0f);
        }
        else if (selectedCar == 1 && koenigsegg) {
            //koenigsegg->SetColor(glm::vec3(0.0f, 0.0f, 1.0f));
            //koenigseggColor = glm::vec3(0.0f, 0.0f, 1.0f);
        }
        std::cout << "Set car color to BLUE" << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE) bPressed = false;

    // Yellow color
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS && !yPressed) {
        yPressed = true;
        if (selectedCar == 0 && porsche) {
            porsche->SetColor(glm::vec3(1.0f, 1.0f, 0.0f));  // Yellow
            porscheColor = glm::vec3(1.0f, 1.0f, 0.0f);
        }
        else if (selectedCar == 1 && koenigsegg) {
            //koenigsegg->SetColor(glm::vec3(1.0f, 1.0f, 0.0f));
            //koenigseggColor = glm::vec3(1.0f, 1.0f, 0.0f);
        }
        std::cout << "Set car color to YELLOW" << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_RELEASE) yPressed = false;

    // White color
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && !wPressed) {
        wPressed = true;
        if (selectedCar == 0 && porsche) {
            porsche->SetColor(glm::vec3(1.0f, 1.0f, 1.0f));  // White
            porscheColor = glm::vec3(1.0f, 1.0f, 1.0f);
        }
        else if (selectedCar == 1 && koenigsegg) {
            //koenigsegg->SetColor(glm::vec3(1.0f, 1.0f, 1.0f));
            //koenigseggColor = glm::vec3(1.0f, 1.0f, 1.0f);
        }
        std::cout << "Set car color to WHITE" << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE) wPressed = false;

    // Orange color
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && !oPressed) {
        oPressed = true;
        if (selectedCar == 0 && porsche) {
            porsche->SetColor(glm::vec3(1.0f, 0.5f, 0.0f));  // Orange
            porscheColor = glm::vec3(1.0f, 0.5f, 0.0f);
        }
        else if (selectedCar == 1 && koenigsegg) {
            //koenigsegg->SetColor(glm::vec3(1.0f, 0.5f, 0.0f));
            //koenigseggColor = glm::vec3(1.0f, 0.5f, 0.0f);
        }
        std::cout << "Set car color to ORANGE" << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_RELEASE) oPressed = false;

    // Purple color
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && !pPressed2) {
        pPressed2 = true;
        if (selectedCar == 0 && porsche) {
            porsche->SetColor(glm::vec3(0.5f, 0.0f, 1.0f));  // Purple
            porscheColor = glm::vec3(0.5f, 0.0f, 1.0f);
        }
        else if (selectedCar == 1 && koenigsegg) {
            //koenigsegg->SetColor(glm::vec3(0.5f, 0.0f, 1.0f));
            //koenigseggColor = glm::vec3(0.5f, 0.0f, 1.0f);
        }
        std::cout << "Set car color to PURPLE" << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE) pPressed2 = false;

    // Reset to original colors
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS && !resetPressed) {
        resetPressed = true;
        if (selectedCar == 0 && porsche) {
            porsche->ResetColor();
        }
        else if (selectedCar == 1 && koenigsegg) {
            //koenigsegg->ResetColor();
        }
        std::cout << "Reset car to original colors" << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_RELEASE) resetPressed = false;

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
        float cameraSpeed = 15.0f * deltaTime;  // Increased speed for larger space
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

        // DEBUG: Print camera position
        static bool spacePressed = false;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !spacePressed) {
            spacePressed = true;
            std::cout << "Camera Position: (" << cameraPos.x << ", "
                << cameraPos.y << ", " << cameraPos.z << ")" << std::endl;
            std::cout << "Camera Front: (" << cameraFront.x << ", "
                << cameraFront.y << ", " << cameraFront.z << ")" << std::endl;
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
            spacePressed = false;
        }

        // Update camera position
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
    std::cout << "  [Arrow Up]   : Move car forward\n";
    std::cout << "  [Arrow Down] : Move car backward\n";
    std::cout << "  [Arrow Left] : Turn/Move car left\n";
    std::cout << "  [Arrow Right]: Turn/Move car right\n";
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
        lastX = static_cast<float>(xpos);
        lastY = static_cast<float>(ypos);
        firstMouse = false;
    }

    float xoffset = static_cast<float>(xpos) - lastX;
    float yoffset = lastY - static_cast<float>(ypos);
    lastX = static_cast<float>(xpos);
    lastY = static_cast<float>(ypos);

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
    fov -= static_cast<float>(yoffset);
    if (fov < 20.0f) fov = 20.0f;
    if (fov > 90.0f) fov = 90.0f;
}



void updateMultipleLights(Shader& shader, const std::vector<glm::vec3>& positions,
    const std::vector<glm::vec3>& colors, const glm::vec3& viewPos) {
    shader.use();

    // Pass view position
    shader.setVec3("viewPos", viewPos);

    // Pass number of lights
    int numLights = std::min((int)positions.size(), 10);  // Limit to 10 lights
    shader.setInt("numLights", numLights);

    // Pass each light's properties
    for (int i = 0; i < numLights; i++) {
        std::string lightStr = "lights[" + std::to_string(i) + "]";
        shader.setVec3(lightStr + ".position", positions[i]);
        shader.setVec3(lightStr + ".color", colors[i]);
        shader.setFloat(lightStr + ".ambient", 0.3f);
        shader.setFloat(lightStr + ".diffuse", 0.8f);
        shader.setFloat(lightStr + ".specular", 0.5f);
        shader.setFloat(lightStr + ".constant", 1.0f);
        shader.setFloat(lightStr + ".linear", 0.09f);
        shader.setFloat(lightStr + ".quadratic", 0.032f);
    }
}


Tree* loadTreeModel(const glm::vec3& position, float scale,
    const std::vector<std::string>& paths,
    const glm::vec3&color) {

    std::cout << "\n=== Loading Tree ===" << std::endl;

    for (const auto& path : paths) {
        std::cout << "Trying to load tree from: " << path << std::endl;

        if (fileExists(path)) {
            std::cout << "File found! Attempting to load..." << std::endl;

            try {
                // Create tree with position, scale, and color
                Tree* tree = new Tree(position, scale, color);

                // Load the model
                if (tree->loadModel(path)) {
                    std::cout << "SUCCESS: Tree loaded from: " << path << std::endl;
                    return tree;
                }
                else {
                    std::cout << "Failed to load tree model" << std::endl;
                    delete tree;
                }
            }
            catch (const std::exception& e) {
                std::cout << "Exception loading tree: " << e.what() << std::endl;
            }
        }
        else {
            std::cout << "File not found." << std::endl;
        }
    }

    std::cout << "WARNING: Could not load tree model from any path" << std::endl;
    return nullptr;
}
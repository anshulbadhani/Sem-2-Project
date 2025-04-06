// main.cpp
#include <GL/glew.h> // Include glew before glfw
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // For ortho
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath> // For std::sin, std::cos

#include "../include/shader.hpp"
#include "../include/renderer.hpp"

// --- Settings ---
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;
const int GRID_RANGE = 10; // How far the grid extends from the origin
const float ANIMATION_DURATION = 2.0f; // Seconds for one transformation

// --- Global State (for simplicity in this example) ---
glm::mat3 currentTransform = glm::mat3(1.0f); // Start with identity
glm::mat3 targetTransform = glm::mat3(1.0f);
float animationTime = 0.0f;
bool isAnimating = false;
int currentMode = 0; // 0: Identity, 1: Rotate, 2: Shear, 3: Scale

// --- Function Declarations ---
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
glm::mat3 interpolate(const glm::mat3& start, const glm::mat3& end, float t);

int main() {
    // --- GLFW Initialization ---
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // --- GLFW Window Creation ---
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Linear Transformation Visualizer", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

    // --- GLEW Initialization ---
    glewExperimental = GL_TRUE; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    // --- OpenGL Global State ---
    glEnable(GL_LINE_SMOOTH); // Make lines look nicer
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    //glEnable(GL_BLEND); // Enable blending for anti-aliasing
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // --- Build and compile shader program ---
    Shader lineShader("shaders/vertex.glsl", "shaders/fragment.glsl");

    // --- Setup Renderer ---
    Renderer renderer(GRID_RANGE);
    renderer.setup();

    // --- Projection Matrix ---
    // Orthographic projection mapping world space [-GRID_RANGE, GRID_RANGE] to screen space
    float aspect = (float)SCR_WIDTH / (float)SCR_HEIGHT;
    float orthoRange = (float)GRID_RANGE + 1.0f; // Add a little padding
    glm::mat4 projection = glm::ortho(-orthoRange * aspect, orthoRange * aspect, -orthoRange, orthoRange, -1.0f, 1.0f);


    // --- Render Loop ---
    float lastFrameTime = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        // --- Time Logic ---
        float currentFrameTime = glfwGetTime();
        float deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        // --- Animation Update ---
        if (isAnimating) {
            animationTime += deltaTime;
            float t = std::min(animationTime / ANIMATION_DURATION, 1.0f);
            // Smoothstep interpolation
            t = t * t * (3.0f - 2.0f * t);
            currentTransform = interpolate(glm::mat3(1.0f), targetTransform, t); // Animate from identity

            if (animationTime >= ANIMATION_DURATION) {
                isAnimating = false;
                currentTransform = targetTransform; // Ensure it ends exactly at the target
            }
        }

        // --- Rendering ---
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Dark background
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw the scene
        renderer.draw(lineShader, currentTransform, projection);

        // --- GLFW: Swap buffers and poll IO events ---
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // --- Cleanup ---
    // Renderer cleans up its own buffers/arrays in its destructor
    glfwTerminate();
    return 0;
}

// --- Function Implementations ---

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
     // Update projection matrix on resize
    float aspect = (float)width / (float)height;
    float orthoRange = (float)GRID_RANGE + 1.0f;
    // Need to update the projection matrix used in the main loop or make it accessible.
    // For now, we just log it. A better design might pass projection updates.
     std::cout << "Window resized, aspect ratio: " << aspect << std::endl;
     // Ideally, recalculate and update the projection matrix here.
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        bool startAnim = false;
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, true);
        }
        else if (key == GLFW_KEY_SPACE || key == GLFW_KEY_R || key == GLFW_KEY_S || key == GLFW_KEY_H || key == GLFW_KEY_I) {
            startAnim = true;
            animationTime = 0.0f; // Reset animation timer
             currentTransform = glm::mat3(1.0f); // Reset starting point for interpolation

            if (key == GLFW_KEY_I || key == GLFW_KEY_SPACE) {
                std::cout << "Transform: Identity\n";
                targetTransform = glm::mat3(1.0f); // Target is Identity
            } else if (key == GLFW_KEY_R) {
                std::cout << "Transform: Rotate 45 degrees\n";
                float angle = glm::radians(45.0f);
                targetTransform = glm::mat3(
                    cos(angle), sin(angle), 0.0f, // Column 1
                   -sin(angle), cos(angle), 0.0f, // Column 2 (Note GLM is column-major)
                    0.0f,       0.0f,      1.0f  // Column 3
                );
                // Adjust for standard math notation (counter-clockwise rotation)
                 targetTransform = glm::transpose(targetTransform); // GLM build matrices often transposed vs math books
            } else if (key == GLFW_KEY_H) {
                 std::cout << "Transform: Shear\n";
                 float shearX = 1.0f; float shearY = 0.5f;
                 targetTransform = glm::mat3(
                     1.0f,   shearY, 0.0f,
                     shearX, 1.0f,   0.0f,
                     0.0f,   0.0f,   1.0f
                 );
                 targetTransform = glm::transpose(targetTransform);
            } else if (key == GLFW_KEY_S) {
                std::cout << "Transform: Scale (x*1.5, y*0.5)\n";
                targetTransform = glm::mat3(
                    1.5f, 0.0f, 0.0f,
                    0.0f, 0.5f, 0.0f,
                    0.0f, 0.0f, 1.0f
                );
                 targetTransform = glm::transpose(targetTransform);
            }
        }

         if(startAnim) {
            isAnimating = true;
         }
    }
}

// Simple linear interpolation for matrices
glm::mat3 interpolate(const glm::mat3& start, const glm::mat3& end, float t) {
    return start * (1.0f - t) + end * t;
}
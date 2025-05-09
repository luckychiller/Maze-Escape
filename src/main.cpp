#include <iostream>
#include <vector>
#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Graphics/Shader.h"
#include "Graphics/Camera.h"
#include "Game/Maze.h"
#include "Graphics/Mesh.h"
#include "Graphics/Renderer.h"
#include "Graphics/Texture.h"

// Window dimensions
unsigned int SCR_WIDTH = 1280;
unsigned int SCR_HEIGHT = 720;

// Camera
Camera camera(glm::vec3(0.0f, 1.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Time
float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

// Maze properties
const float wallHeight = 2.0f;
const float wallThickness = 0.1f;

// GLFW error callback function
void glfw_error_callback(int error, const char *description)
{
    std::cerr << "GLFW Error (" << error << "): " << description << std::endl;
}

// GLFW framebuffer size callback function
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;   // Update globals
    SCR_HEIGHT = height; // Update globals
}

// Mouse callback function
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn); // Convert to float
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    { // first time mouse is moved
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// Scroll callback function
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// Process camera movement from keys input
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(DOWN, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        // move faster
        camera.MovementSpeed = 5.0f;
    }
    else
    {
        camera.MovementSpeed = 2.5f;
    }
}

// Generate indices for a cube
std::vector<unsigned int> GenerateIndices(const float *vertices)
{
    std::vector<unsigned int> indices;
    for (int i = 0; i < 8; i++)
    {
        for (int j = i + 1; j < 8; j++)
        {
            for (int k = j + 1; k < 8; k++)
            {
                // Check all unique triplets
                if (i != j && j != k && i != k)
                {
                    indices.push_back(i);
                    indices.push_back(j);
                    indices.push_back(k);
                }
            }
        }
    }
    return indices;
}

// Initialize GLFW and create window
GLFWwindow *initializeWindow()
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return nullptr;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Maze Escape", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    // glfwFocusWindow(window); // Usually not needed, MakeContextCurrent implies focus for input
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    return window;
}

// Initialize GLAD
bool initializeGLAD()
{
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    const GLubyte *renderer = glGetString(GL_RENDERER);
    const GLubyte *version = glGetString(GL_VERSION);
    std::cout << "Renderer: " << renderer << std::endl;
    std::cout << "OpenGL version supported: " << version << std::endl;

    return true;
}

// Setup OpenGL state
void setupOpenGL()
{
    glEnable(GL_DEPTH_TEST);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Uncomment to see wireframe
}


int main() {
    GLFWwindow* window = initializeWindow();
    if (!window) {return -1;}

    if (!initializeGLAD())
    {
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    setupOpenGL(); // Initial viewport set by framebuffer_callback or GLAD init

    Renderer renderer;

    // Create and generate maze
    int mazeGridW = 15; // For clarity with maze size vs world units
    int mazeGridH = 15;
    Maze gameMaze(mazeGridW, mazeGridH);
    gameMaze.GenerateMaze(0, 0);
    gameMaze.PrintToConsole();
    // Position camera within the maze

    camera.Position = glm::vec3( (float)mazeGridW / 2.0f, wallHeight / 2.0f, (float)mazeGridH / 2.0f );
    camera.UpdateCameraVectors();

    // --- Shaders (Construct directly) ---
    Shader wallShader("shaders/wall.vert", "shaders/wall.frag");
    if (wallShader.ID == 0)
    {
        /* error handling */
        std::cerr << "Failed to load wall shader." << std::endl; return -1;
    }

    Shader floorShader("shaders/floor.vert", "shaders/floor.frag");
    if (floorShader.ID == 0) {
        /* error handling */
        std::cerr << "Failed to load floor shader." << std::endl; return -1;
    }

    // --- Load Textures ---
    std::unique_ptr<Texture> wallTexture = std::make_unique<Texture>("textures/wall.jpg");
    std::unique_ptr<Texture> floorTexture = std::make_unique<Texture>("textures/floor.jpg");
    std::unique_ptr<Texture> ceilingTexture = std::make_unique<Texture>("textures/ceiling.jpg");

    // Cube vertices for walls with proper texture coordinates for each face
    std::vector<Vertex> cubeVerticesData = {
        // Back face
        {{ -0.5f, -0.5f, -0.5f }, {0.0f, 0.0f}}, // Bottom-left
        {{  0.5f, -0.5f, -0.5f }, {1.0f, 0.0f}}, // Bottom-right
        {{  0.5f,  0.5f, -0.5f }, {1.0f, 1.0f}}, // Top-right
        {{ -0.5f,  0.5f, -0.5f }, {0.0f, 1.0f}}, // Top-left

        // Front face
        {{ -0.5f, -0.5f,  0.5f }, {0.0f, 0.0f}}, // Bottom-left
        {{  0.5f, -0.5f,  0.5f }, {1.0f, 0.0f}}, // Bottom-right
        {{  0.5f,  0.5f,  0.5f }, {1.0f, 1.0f}}, // Top-right
        {{ -0.5f,  0.5f,  0.5f }, {0.0f, 1.0f}}, // Top-left

        // Left face
        {{ -0.5f,  0.5f,  0.5f }, {1.0f, 0.0f}}, // Top-right
        {{ -0.5f,  0.5f, -0.5f }, {1.0f, 1.0f}}, // Top-left
        {{ -0.5f, -0.5f, -0.5f }, {0.0f, 1.0f}}, // Bottom-left
        {{ -0.5f, -0.5f,  0.5f }, {0.0f, 0.0f}}, // Bottom-right

        // Right face
        {{  0.5f,  0.5f,  0.5f }, {0.0f, 0.0f}}, // Top-left
        {{  0.5f,  0.5f, -0.5f }, {1.0f, 0.0f}}, // Top-right
        {{  0.5f, -0.5f, -0.5f }, {1.0f, 1.0f}}, // Bottom-right
        {{  0.5f, -0.5f,  0.5f }, {0.0f, 1.0f}}, // Bottom-left

        // Bottom face
        {{ -0.5f, -0.5f, -0.5f }, {0.0f, 1.0f}}, // Top-left
        {{  0.5f, -0.5f, -0.5f }, {1.0f, 1.0f}}, // Top-right
        {{  0.5f, -0.5f,  0.5f }, {1.0f, 0.0f}}, // Bottom-right
        {{ -0.5f, -0.5f,  0.5f }, {0.0f, 0.0f}}, // Bottom-left

        // Top face
        {{ -0.5f,  0.5f, -0.5f }, {0.0f, 0.0f}}, // Bottom-left
        {{  0.5f,  0.5f, -0.5f }, {1.0f, 0.0f}}, // Bottom-right
        {{  0.5f,  0.5f,  0.5f }, {1.0f, 1.0f}}, // Top-right
        {{ -0.5f,  0.5f,  0.5f }, {0.0f, 1.0f}}  // Top-left
    };

    // Now we have 24 vertices (4 for each face) with proper texture coordinates
    // Update indices to match the new vertex layout
    std::vector<unsigned int> cubeIndicesData = {
        0, 1, 2, 2, 3, 0,       // Back face
        4, 5, 6, 6, 7, 4,       // Front face
        8, 9, 10, 10, 11, 8,    // Left face
        12, 13, 14, 14, 15, 12, // Right face
        16, 17, 18, 18, 19, 16, // Bottom face
        20, 21, 22, 22, 23, 20  // Top face
    };
    // For walls, we probably only need position. Let's adjust Mesh setup if wall shader doesn't use texCoords.
    // For now, the Mesh class expects Vertex struct with TexCoords.
    // If wall.vert only uses aPos (location 0), the TexCoords data won't be used but must be provided for stride.

    std::unique_ptr<Mesh> cubeMesh = std::make_unique<Mesh>(cubeVerticesData, cubeIndicesData);

    // --- Define Vertices for Plane (Floor/Ceiling) ---
    std::vector<Vertex> planeVerticesData = {
        {{  0.5f, 0.0f,  0.5f }, { (float)mazeGridW, (float)mazeGridH }}, // Top Right
        {{  0.5f, 0.0f, -0.5f }, { (float)mazeGridW, 0.0f            }}, // Bottom Right
        {{ -0.5f, 0.0f, -0.5f }, { 0.0f,            0.0f            }}, // Bottom Left
        {{ -0.5f, 0.0f,  0.5f }, { 0.0f,            (float)mazeGridH }}  // Top Left
    };
    std::vector<unsigned int> planeIndicesData = { 0, 1, 3, 1, 2, 3 };
    std::unique_ptr<Mesh> planeMesh = std::make_unique<Mesh>(planeVerticesData, planeIndicesData);


    // --- Game Loop ---
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        renderer.Clear(); // Use renderer to clear screen
        renderer.BeginScene(camera, (float)SCR_WIDTH, (float)SCR_HEIGHT);

        floorShader.use(); // Activate shader before setting uniforms
        renderer.SetShaderMatrices(floorShader); // Set view and projection matrices

        // Set texture for floor
        floorTexture->Bind(0); // Bind to texture unit 0
        floorShader.setInt("floorTexture", 0); // Tell shader to use texture unit 0
        floorShader.setBool("isCeiling", false); // This is the floor

        glm::mat4 floorModel = glm::mat4(1.0f);
        floorModel = glm::translate(floorModel, glm::vec3((float)(mazeGridW - 1) / 2.0f, 0.0f, (float)(mazeGridH - 1) / 2.0f));
        floorModel = glm::scale(floorModel, glm::vec3((float)mazeGridW, 1.0f, (float)mazeGridH));
        renderer.Submit(floorShader, *planeMesh, floorModel);

        // Set texture for ceiling (can use a different texture or the same with a modifier)
        ceilingTexture->Bind(0); // Bind to texture unit 0
        floorShader.setInt("floorTexture", 0); // Tell shader to use texture unit 0
        floorShader.setBool("isCeiling", true); // This is the ceiling

        glm::mat4 ceilingModel = glm::mat4(1.0f);
        ceilingModel = glm::translate(ceilingModel, glm::vec3((float)(mazeGridW - 1) / 2.0f, wallHeight, (float)(mazeGridH - 1) / 2.0f));
        ceilingModel = glm::scale(ceilingModel, glm::vec3((float)mazeGridW, 1.0f, (float)mazeGridH));
        renderer.Submit(floorShader, *planeMesh, ceilingModel); // Submit takes shader, mesh, model

        // --- Render Maze Walls ---
        wallShader.use();
        renderer.SetShaderMatrices(wallShader); // Set view and projection matrices

        // Set texture for walls
        wallTexture->Bind(0); // Bind to texture unit 0
        wallShader.setInt("wallTexture", 0); // Tell shader to use texture unit 0

        for (int y = 0; y < gameMaze.GetHeight(); ++y) {
            for (int x = 0; x < gameMaze.GetWidth(); ++x) {
                const Cell& cell = gameMaze.GetCell(x, y);
                glm::vec3 cellOrigin = glm::vec3(static_cast<float>(x), 0.0f, static_cast<float>(y));

                if (cell.wallTop) {
                    glm::mat4 model = glm::mat4(1.0f);
                    model = glm::translate(model, cellOrigin + glm::vec3(0.5f, wallHeight / 2.0f, 0.0f) );
                    model = glm::scale(model, glm::vec3(1.0f, wallHeight, wallThickness));
                    renderer.Submit(wallShader, *cubeMesh, model);
                }
                if (cell.wallBottom) {
                    glm::mat4 model = glm::mat4(1.0f);
                    model = glm::translate(model, cellOrigin + glm::vec3(0.5f, wallHeight / 2.0f, 1.0f) );
                    model = glm::scale(model, glm::vec3(1.0f, wallHeight, wallThickness));
                    renderer.Submit(wallShader, *cubeMesh, model);
                }
                if (cell.wallLeft) {
                    glm::mat4 model = glm::mat4(1.0f);
                    model = glm::translate(model, cellOrigin + glm::vec3(0.0f, wallHeight / 2.0f, 0.5f) );
                    model = glm::scale(model, glm::vec3(wallThickness, wallHeight, 1.0f));
                    renderer.Submit(wallShader, *cubeMesh, model);
                }
                if (cell.wallRight) {
                    glm::mat4 model = glm::mat4(1.0f);
                    model = glm::translate(model, cellOrigin + glm::vec3(1.0f, wallHeight / 2.0f, 0.5f) );
                    model = glm::scale(model, glm::vec3(wallThickness, wallHeight, 1.0f));
                    renderer.Submit(wallShader, *cubeMesh, model);
                }
            }
        }
        // renderer.EndScene(); // Call if it does something

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
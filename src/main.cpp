// main.cpp - Maze Escape main entry point

// --- Standard Library Includes ---
#include <iostream>
#include <vector>
#include <memory>

// --- External Library Includes ---
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

// --- Project Includes ---
#include "Graphics/Shader.h"
#include "Graphics/Camera.h"
#include "Game/Maze.h"
#include "Graphics/Mesh.h"
#include "Graphics/Renderer.h"
#include "Graphics/Texture.h"
#include "Game/Player.h"
#include "Game/GameLogic.h"
#include "Graphics/GLUtils.h"
#include "Utils/Utils.h"

// --- Global State (grouped in a namespace for clarity) ---
namespace Globals {
    unsigned int SCR_WIDTH = 1280;
    unsigned int SCR_HEIGHT = 720;
    Camera camera(glm::vec3(0.0f, 1.0f, 3.0f));
    float lastX = SCR_WIDTH / 2.0f;
    float lastY = SCR_HEIGHT / 2.0f;
    bool firstMouse = true;
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    bool pKeyPressed = false;
    const float wallHeight = 2.0f;
    const float wallThickness = 0.1f;
    Player *g_Player = nullptr;
    Maze *g_Maze = nullptr;
    GameLogic *g_GameLogic = nullptr;
}
using namespace Globals;

// --- Utility and Callback Functions ---
// Process player movement from keys input
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    // Check if player and maze are initialized
    if (!g_Player || !g_Maze)
    {
        return;
    }

    // Check if game is won
    if (g_GameLogic && g_GameLogic->GetState() == GameState::WON)
    {
        // If R key is pressed, reset the game
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        {
            g_GameLogic->Reset();
        }
        return; // Don't process movement if game is won
    }

    // Set movement speed
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        // move faster
        camera.MovementSpeed = 5.0f;
    }
    else
    {
        camera.MovementSpeed = 2.5f;
    }

    // Process movement with collision detection
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        g_Player->ProcessKeyboard(FORWARD, deltaTime, *g_Maze);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        g_Player->ProcessKeyboard(BACKWARD, deltaTime, *g_Maze);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        g_Player->ProcessKeyboard(LEFT, deltaTime, *g_Maze);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        g_Player->ProcessKeyboard(RIGHT, deltaTime, *g_Maze);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        g_Player->ProcessKeyboard(UP, deltaTime, *g_Maze);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        g_Player->ProcessKeyboard(DOWN, deltaTime, *g_Maze);
    }
}

// --- Main Function ---
int main()
{
    // --- Initialization ---
    GLFWwindow *window = InitializeWindow(Globals::SCR_WIDTH, Globals::SCR_HEIGHT, "Maze Escape");
    if (!window){
        
    system("pause");
        return -1;
    }
    if (!InitializeGLAD()) {
        glfwDestroyWindow(window);
        glfwTerminate();
    system("pause");
        return -1;
    }
    SetupOpenGL();
    Renderer renderer;

    // Create and generate maze
    int mazeGridW = 5; // For clarity with maze size vs world units
    int mazeGridH = 5;
    Maze gameMaze(mazeGridW, mazeGridH);
    gameMaze.GenerateMaze(0, 0);
    gameMaze.PrintToConsole();

    // Set global maze pointer for input processing
    g_Maze = &gameMaze;

    // Position camera within the maze
    camera.Position = glm::vec3((float)mazeGridW / 2.0f, wallHeight / 2.0f, (float)mazeGridH / 2.0f);
    camera.UpdateCameraVectors();

    // Create player and game logic
    Player player(camera);
    GameLogic gameLogic(player, gameMaze);

    // Set global pointers for input processing
    g_Player = &player;
    g_GameLogic = &gameLogic;

    // Position player at the start cell
    gameLogic.Reset();

    // Print instructions
    std::cout << "Press P to view the maze with your current position." << std::endl;
    std::cout << "Press R to restart the game after reaching the exit." << std::endl;

    // --- Shaders (Construct directly) ---
    Shader wallShader("shaders/wall.vert", "shaders/wall.frag");
    if (wallShader.ID == 0)
    {
        /* error handling */
        std::cerr << "Failed to load wall shader." << std::endl;
    system("pause");
        return -1;
    }

    Shader floorShader("shaders/floor.vert", "shaders/floor.frag");
    if (floorShader.ID == 0)
    {
        /* error handling */
        std::cerr << "Failed to load floor shader." << std::endl;
    system("pause");
        return -1;
    }

    Shader skyboxShader("shaders/skybox.vert", "shaders/skybox.frag"); // New Skybox Shader
    if (skyboxShader.ID == 0)
    {
        /* error handling */
        std::cerr << "Failed to load skybox shader." << std::endl;
    system("pause");
        return -1;
    }

    // --- Load Textures ---
    std::unique_ptr<Texture> wallTexture = std::make_unique<Texture>("textures/wall.jpg");
    std::unique_ptr<Texture> floorTexture = std::make_unique<Texture>("textures/floor.jpg");
    std::unique_ptr<Texture> ceilingTexture = std::make_unique<Texture>("textures/ceiling.jpg");
    std::unique_ptr<Texture> exitTexture = std::make_unique<Texture>("textures/exit.jpg");
    
    std::vector<std::string> faces {
        "textures/skybox/right.png", "textures/skybox/left.png",
        "textures/skybox/top.png",   "textures/skybox/bottom.png",
        "textures/skybox/front.png", "textures/skybox/back.png"
    };
    unsigned int cubemapTextureID = LoadCubemap(faces);
    if (cubemapTextureID == 0) { std::cerr << "Failed to load cubemap texture." << std::endl; return -1; }



    // Cube vertices for walls with proper texture coordinates for each face
    std::vector<Vertex> cubeVerticesData = {
        // Back face
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}}, // Bottom-left
        {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},  // Bottom-right
        {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},   // Top-right
        {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},  // Top-left

        // Front face
        {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}, // Bottom-left
        {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},  // Bottom-right
        {{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},   // Top-right
        {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},  // Top-left

        // Left face
        {{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},   // Top-right
        {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},  // Top-left
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}}, // Bottom-left
        {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},  // Bottom-right

        // Right face
        {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},   // Top-left
        {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},  // Top-right
        {{0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}}, // Bottom-right
        {{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},  // Bottom-left

        // Bottom face
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}}, // Top-left
        {{0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},  // Top-right
        {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},   // Bottom-right
        {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},  // Bottom-left

        // Top face
        {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}, // Bottom-left
        {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},  // Bottom-right
        {{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},   // Top-right
        {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}}   // Top-left
    };

    float skyboxVertices[] = {
    // positions
    -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f
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
        {{0.5f, 0.0f, 0.5f}, {(float)mazeGridW, (float)mazeGridH}, {0.0f, 1.0f, 0.0f}}, // Top Right
        {{0.5f, 0.0f, -0.5f}, {(float)mazeGridW, 0.0f}, {0.0f, 1.0f, 0.0f}},            // Bottom Right
        {{-0.5f, 0.0f, -0.5f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},                       // Bottom Left
        {{-0.5f, 0.0f, 0.5f}, {0.0f, (float)mazeGridH}, {0.0f, 1.0f, 0.0f}}             // Top Left
    };
    std::vector<unsigned int> planeIndicesData = {0, 1, 3, 1, 2, 3};
    std::unique_ptr<Mesh> planeMesh = std::make_unique<Mesh>(planeVerticesData, planeIndicesData);

    // Create exit marker mesh (a simple colored cube)
    std::vector<Vertex> exitMarkerVerticesData = cubeVerticesData;     // Use same vertices as cube
    std::vector<unsigned int> exitMarkerIndicesData = cubeIndicesData; // Use same indices as cube
    std::unique_ptr<Mesh> exitMarkerMesh = std::make_unique<Mesh>(exitMarkerVerticesData, exitMarkerIndicesData);

    // Create exit marker shader
    Shader exitMarkerShader("shaders/exit.vert", "shaders/exit.frag");
    if (exitMarkerShader.ID == 0)
    {
        std::cerr << "Failed to load exit marker shader." << std::endl;
    system("pause");
        return -1;
    }

    
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0); // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);


    // Light properties
    glm::vec3 lightDir = glm::normalize(glm::vec3(0.5f, -1.0f, 0.7f)); // Direction FROM light source
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 0.9f);                // Slightly yellowish white
    float ambientIntensity = 0.3f;
    float materialShininess = 32.0f;
    float materialSpecularStrength = 0.4f; // For walls and floor

    // --- Game Loop ---
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        // Update player and game logic
        player.Update(deltaTime, gameMaze);
        gameLogic.Update(deltaTime);

        // Print maze with player position when P key is pressed (with debounce)
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        {
            if (!pKeyPressed)
            {
                // Clear console (Windows-specific)
                system("cls");
                // Print maze with player position
                gameMaze.PrintToConsole(player.GetCurrentCell());
                pKeyPressed = true;
            }
        }
        else
        {
            pKeyPressed = false;
        }

        // --- Prepare View and Projection Matrices ---
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        // --- Render Skybox ---
        RenderSkybox(skyboxShader, skyboxVAO, cubemapTextureID, view, projection);

        // --- Render Scene ---
        renderer.BeginScene(camera, (float)SCR_WIDTH, (float)SCR_HEIGHT);

        floorShader.use();                       // Activate shader before setting uniforms
        renderer.SetShaderMatrices(floorShader); // Set view and projection matrices

        // Set texture for floor
        floorTexture->Bind(0);                   // Bind to texture unit 0
        floorShader.setInt("floorTexture", 0);   // Tell shader to use texture unit 0
        floorShader.setBool("isCeiling", false); // This is the floor

        // Set lighting uniforms for floorShader
        floorShader.setVec3("light_direction", lightDir);
        floorShader.setVec3("light_color", lightColor);
        floorShader.setFloat("light_ambientIntensity", ambientIntensity);
        floorShader.setVec3("viewPos", camera.Position);
        floorShader.setFloat("material_shininess", materialShininess);                      // Could be different for floor
        floorShader.setFloat("material_specularStrength", materialSpecularStrength * 0.5f); // Floor less shiny

        glm::mat4 floorModel = glm::mat4(1.0f);
        floorModel = glm::translate(floorModel, glm::vec3((float)(mazeGridW - 1) / 2.0f, 0.0f, (float)(mazeGridH - 1) / 2.0f));
        floorModel = glm::scale(floorModel, glm::vec3((float)mazeGridW, 1.0f, (float)mazeGridH));
        renderer.Submit(floorShader, *planeMesh, floorModel);

        // Set texture for ceiling (can use a different texture or the same with a modifier)
        ceilingTexture->Bind(0);                // Bind to texture unit 0
        floorShader.setInt("floorTexture", 0);  // Tell shader to use texture unit 0
        floorShader.setBool("isCeiling", true); // This is the ceiling

        glm::mat4 ceilingModel = glm::mat4(1.0f);
        ceilingModel = glm::translate(ceilingModel, glm::vec3((float)(mazeGridW - 1) / 2.0f, wallHeight, (float)(mazeGridH - 1) / 2.0f));
        ceilingModel = glm::scale(ceilingModel, glm::vec3((float)mazeGridW, 1.0f, (float)mazeGridH));
        renderer.Submit(floorShader, *planeMesh, ceilingModel); // Submit takes shader, mesh, model

        // --- Render Maze Walls ---
        wallShader.use();
        renderer.SetShaderMatrices(wallShader); // Set view and projection matrices

        // Set texture for walls
        wallTexture->Bind(0);                // Bind to texture unit 0
        wallShader.setInt("wallTexture", 0); // Tell shader to use texture unit 0

        // Set lighting uniforms for wallShader
        wallShader.setVec3("light_direction", lightDir);
        wallShader.setVec3("light_color", lightColor);
        wallShader.setFloat("light_ambientIntensity", ambientIntensity);
        wallShader.setVec3("viewPos", camera.Position);
        wallShader.setFloat("material_shininess", materialShininess);
        wallShader.setFloat("material_specularStrength", materialSpecularStrength);

        for (int y = 0; y < gameMaze.GetHeight(); ++y)
        {
            for (int x = 0; x < gameMaze.GetWidth(); ++x)
            {
                const Cell &cell = gameMaze.GetCell(x, y);
                glm::vec3 cellOrigin = glm::vec3(static_cast<float>(x), 0.0f, static_cast<float>(y));

                if (cell.wallTop)
                {
                    glm::mat4 model = glm::mat4(1.0f);
                    model = glm::translate(model, cellOrigin + glm::vec3(0.5f, wallHeight / 2.0f, 0.0f));
                    model = glm::scale(model, glm::vec3(1.0f, wallHeight, wallThickness));
                    renderer.Submit(wallShader, *cubeMesh, model);
                }
                if (cell.wallBottom)
                {
                    glm::mat4 model = glm::mat4(1.0f);
                    model = glm::translate(model, cellOrigin + glm::vec3(0.5f, wallHeight / 2.0f, 1.0f));
                    model = glm::scale(model, glm::vec3(1.0f, wallHeight, wallThickness));
                    renderer.Submit(wallShader, *cubeMesh, model);
                }
                if (cell.wallLeft)
                {
                    glm::mat4 model = glm::mat4(1.0f);
                    model = glm::translate(model, cellOrigin + glm::vec3(0.0f, wallHeight / 2.0f, 0.5f));
                    model = glm::scale(model, glm::vec3(wallThickness, wallHeight, 1.0f));
                    renderer.Submit(wallShader, *cubeMesh, model);
                }
                if (cell.wallRight)
                {
                    glm::mat4 model = glm::mat4(1.0f);
                    model = glm::translate(model, cellOrigin + glm::vec3(1.0f, wallHeight / 2.0f, 0.5f));
                    model = glm::scale(model, glm::vec3(wallThickness, wallHeight, 1.0f));
                    renderer.Submit(wallShader, *cubeMesh, model);
                }
            }
        }

        // Render exit marker
        exitMarkerShader.use();
        renderer.SetShaderMatrices(exitMarkerShader);
        exitTexture->Bind(0);
        exitMarkerShader.setInt("exitTexture", 0);
        glm::ivec2 exitCoords = gameMaze.GetEndCellCoords();
        if (exitCoords.x >= 0 && exitCoords.y >= 0)
        {
            glm::vec3 exitPosition = glm::vec3(exitCoords.x + 0.5f, 0.5f, exitCoords.y + 0.5f);
            glm::mat4 exitModel = glm::mat4(1.0f);
            exitModel = glm::translate(exitModel, exitPosition);
            exitModel = glm::scale(exitModel, glm::vec3(0.3f, 0.3f, 0.3f));
            exitMarkerShader.setVec3("color", glm::vec3(1.0f, 1.0f, 1.0f)); // No tint
            renderer.Submit(exitMarkerShader, *exitMarkerMesh, exitModel);
        }

        // Display game state
        if (gameLogic.GetState() == GameState::WON)
        {
            // In a real game, you would render text or a UI element here
            // For now, we just print to the console (already done in GameLogic::CheckWinCondition)
            // You could add a 2D overlay with text or a simple colored quad
        }

        renderer.EndScene();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    system("pause");
    return 0;
}
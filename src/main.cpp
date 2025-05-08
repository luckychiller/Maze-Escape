#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Graphics/Shader.h"
#include "Graphics/Camera.h"
#include "Game/Maze.h"

// Window dimensions
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Time
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

// GLFW error callback function
void glfw_error_callback(int error, const char* description) {
    std::cerr << "GLFW Error (" << error << "): " << description << std::endl;
}

// GLFW framebuffer size callback function
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

std::vector<unsigned int> GenerateIndices(const float* vertices) {
    std::vector<unsigned int> indices;
    for (int i = 0; i < 8; i++) {
        for (int j = i + 1 ; j < 8; j++) {
            for (int k = j + 1; k < 8; k++) {
                // Check all unique triplets
                if (i != j && j != k && i != k) {
                    indices.push_back(i);
                    indices.push_back(j);
                    indices.push_back(k);
                }
            }
        }
    }
    return indices;
}

// process camera movement from keys input
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        camera.ProcessKeyboard(UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        camera.ProcessKeyboard(DOWN, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        // move faster
        camera.MovementSpeed = 5.0f;
    }
    else {
        camera.MovementSpeed = 2.5f;
    }
}

// mouse callback function
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn); // Convert to float
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) { // first time mouse is moved
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

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

int main() {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Maze Escape - Camera test", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwFocusWindow(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    std::cout << "Renderer: " << renderer << std::endl;
    std::cout << "OpenGL version supported: " << version << std::endl;

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glEnable(GL_DEPTH_TEST);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // test
    int mazeWidth = 15; // Example width
    int mazeHeight = 15; // Example height
    Maze gameMaze(mazeWidth, mazeHeight);
    gameMaze.GenerateMaze(0, 0); // Start generation from cell (0,0)
    std::cout << "Generated Maze (" << mazeWidth << "x" << mazeHeight << "):" << std::endl;
    gameMaze.PrintToConsole();
    // test

    Shader ourShader("shaders/basic.vert", "shaders/basic.frag");
     if (ourShader.ID == 0 || glIsProgram(ourShader.ID) == GL_FALSE) {
         std::cerr << "ERROR::MAIN::SHADER_CREATION_FAILED" << std::endl;
    }

    // --- Set up vertex data (8 unique vertices) ---
    float vertices[] = {
        // positions         // (vertex index for comments)
        -0.5f, -0.5f, -0.5f,  // 0: Bottom-left-back
         0.5f, -0.5f, -0.5f,  // 1: Bottom-right-back
         0.5f,  0.5f, -0.5f,  // 2: Top-right-back
        -0.5f,  0.5f, -0.5f,  // 3: Top-left-back
        -0.5f, -0.5f,  0.5f,  // 4: Bottom-left-front
         0.5f, -0.5f,  0.5f,  // 5: Bottom-right-front
         0.5f,  0.5f,  0.5f,  // 6: Top-right-front
        -0.5f,  0.5f,  0.5f   // 7: Top-left-front
    };

    std::vector<unsigned int> indices = GenerateIndices(vertices);

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO); // Generate Element Buffer Object

    glBindVertexArray(VAO);

    // Copy vertex data to VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Copy index data to EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // Bind EBO
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Set vertex attribute pointers
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // IMPORTANT: When a VAO is bound, the GL_ELEMENT_ARRAY_BUFFER binding IS stored as part of the VAO's state.
    // So, you don't need to re-bind the EBO every time in the render loop if the VAO is bound.
    // However, unbinding the GL_ARRAY_BUFFER (VBO) is fine after glVertexAttribPointer.
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Optional: VBO is no longer needed for configuration once attributes are set

    glBindVertexArray(0); // Unbind VAO
    // It's also good practice to unbind EBO after VAO if you're not immediately drawing,
    // but it's less critical if the VAO itself is unbound.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // --- Game Loop ---
    while (!glfwWindowShouldClose(window)) {
        // Calculate deltaTime
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.5f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();

        // Create transformation matrices
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = camera.GetProjectionMatrix(SCR_WIDTH, SCR_HEIGHT);

        // Rotate the model slightly to see it better
        model = glm::rotate(model, (float)glfwGetTime()* glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

        // Pass transformation matrices to the shader using our helper method
        ourShader.setMat4("model", model);
        ourShader.setMat4("view", view);
        ourShader.setMat4("projection", projection);

        glBindVertexArray(VAO); // Bind VAO (this also re-binds the EBO associated with this VAO)

        // Use glDrawElements instead of glDrawArrays when using an EBO
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        // Param 1: Mode (GL_TRIANGLES)
        // Param 2: Count - number of elements (indices) to be rendered (36 indices = 12 triangles)
        // Param 3: Type of the indices (GL_UNSIGNED_INT as our 'indices' array is unsigned int)
        // Param 4: Offset - a pointer to the location where the indices begin in the EBO (0 for start)

        glBindVertexArray(0); // Optional unbind

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

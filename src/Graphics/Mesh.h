#pragma once

#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp> // For potential future use with vertex data struct

// A simple vertex structure (can be expanded)
struct Vertex {
    glm::vec3 Position;
    glm::vec2 TexCoords;
    glm::vec3 Normal; // For lighting later
};

class Shader; // Forward declaration of Shader class

class Mesh {
public:
    // Mesh Data
    std::vector<Vertex>       vertices; // Using a struct for vertices
    std::vector<unsigned int> indices;
    // unsigned int              textureId; // If mesh has a specific texture

    // Render state
    unsigned int VAO, VBO, EBO;

    // Constructor for indexed meshes
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    // Constructor for non-indexed meshes (less common for complex shapes but possible)
    // Mesh(const std::vector<Vertex>& vertices);
    ~Mesh();

    // Render the mesh
    void Draw(Shader& shader); // Shader is passed in for setting uniforms specific to this mesh/material

private:
    // Initializes all the buffer objects/arrays
    void setupMesh();
};

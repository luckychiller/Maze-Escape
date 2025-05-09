#pragma once

#include "Shader.h"
#include "Mesh.h"
#include "Camera.h" // Renderer needs to know about the camera for view/projection

#include <glm/glm.hpp>

class Renderer {
public:
    Renderer();
    ~Renderer();

    void Clear() const;

    // Sets up view and projection matrices for the scene
    void BeginScene(Camera& camera, float screenWidth, float screenHeight);

    // Submits a mesh to be rendered with a specific shader and model transformation
    void Submit(Shader& shader, Mesh& mesh, const glm::mat4& modelTransform = glm::mat4(1.0f));
    // Alternatively, a more direct Draw function:
    // void Draw(Shader& shader, Mesh& mesh, const glm::mat4& modelTransform = glm::mat4(1.0f));

    void EndScene(); // For now, might not do much

    // Getter methods for view and projection matrices
    const glm::mat4& GetViewMatrix() const { return M_ViewMatrix; }
    const glm::mat4& GetProjectionMatrix() const { return M_ProjectionMatrix; }

    // Method to set view and projection uniforms for a shader
    void SetShaderMatrices(Shader& shader);

private:
    // Store current view and projection matrices for the frame
    // This avoids passing them around constantly or recalculating if camera hasn't moved
    glm::mat4 M_ViewMatrix;
    glm::mat4 M_ProjectionMatrix;
};

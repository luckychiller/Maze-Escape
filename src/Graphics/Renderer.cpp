#include "Renderer.h"
#include <glad/glad.h> // For glClear, etc.

Renderer::Renderer() {
    // Constructor: Could initialize default clear color or other states if needed
    // For now, we assume OpenGL state like depth testing is enabled elsewhere (e.g., main)
}

Renderer::~Renderer() {
    // Destructor
}

void Renderer::Clear() const {
    // Default clear color, can be made configurable
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Default clear color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::BeginScene(Camera& camera, float screenWidth, float screenHeight) {
    // Calculate and store view and projection matrices for this frame
    M_ViewMatrix = camera.GetViewMatrix();
    M_ProjectionMatrix = camera.GetProjectionMatrix(screenWidth, screenHeight);

    // Note: Shaders will still need to be activated (`shader.use()`) before setting these
    // uniforms, typically done just before calling Submit/Draw.
}

void Renderer::Submit(Shader& shader, Mesh& mesh, const glm::mat4& modelTransform) {
    // It's assumed the correct shader is already active (`shader.use()`)
    // and view/projection are set if BeginScene was called.

    shader.setMat4("model", modelTransform);
    // If BeginScene wasn't called, or if a shader needs VP matrices directly:
    // shader.setMat4("view", M_ViewMatrix);       // This implies shader must be active
    // shader.setMat4("projection", M_ProjectionMatrix); // This implies shader must be active

    mesh.Draw(shader); // Mesh::Draw binds VAO and calls glDrawElements/Arrays
}

void Renderer::EndScene() {
    // Could be used for post-processing passes, flushing render queues, etc.
    // For now, it does nothing.
}

void Renderer::SetShaderMatrices(Shader& shader) {
    // Set view and projection matrices in the shader
    shader.setMat4("view", M_ViewMatrix);
    shader.setMat4("projection", M_ProjectionMatrix);
}
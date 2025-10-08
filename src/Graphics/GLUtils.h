#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include "Shader.h"

GLFWwindow* InitializeWindow(unsigned int width, unsigned int height, const char* title);
bool InitializeGLAD();
void SetupOpenGL();
unsigned int LoadCubemap(const std::vector<std::string>& faces, bool flipVerticallyOnLoad = false);
void RenderSkybox(Shader &skyboxShader, unsigned int skyboxVAO, unsigned int cubemapTextureID, const glm::mat4 &view, const glm::mat4 &projection);

cmake_minimum_required(VERSION 3.12)
project(MazeEscape VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # Optional: Be stricter about C++ standards

# Include directories for external libraries
include_directories(${CMAKE_SOURCE_DIR}/external/glad/include)
include_directories(${CMAKE_SOURCE_DIR}/external/glfw/include)
include_directories(${CMAKE_SOURCE_DIR}/external/glm)
include_directories(${CMAKE_SOURCE_DIR}/external)

# Add source files
set(SOURCES
    external/glad/src/glad.c
    external/stb_image.c
    src/main.cpp
    src/Core/Application.cpp
    src/Core/Input.cpp
    src/Core/Time.cpp
    src/Graphics/Camera.cpp
    src/Graphics/Mesh.cpp
    src/Graphics/Model.cpp
    src/Graphics/Renderer.cpp
    src/Graphics/Shader.cpp
    src/Graphics/Texture.cpp
    src/Graphics/GLUtils.cpp
    src/Game/Maze.cpp
    src/Game/Player.cpp
    src/Game/GameLogic.cpp
    src/Utils/FileSystem.cpp
    src/Utils/Logging.cpp
    src/Utils/Utils.h

)

# Add header files
set(HEADERS
    external/glad/include/glad/glad.h
    external/glfw/include/GLFW/glfw3.h
    external/glm/glm.hpp
    external/stb_image.h
    src/Core/Application.h
    src/Core/Input.h
    src/Core/Time.h
    src/Graphics/Camera.h
    src/Graphics/Mesh.h
    src/Graphics/Model.h
    src/Graphics/Renderer.h
    src/Graphics/Shader.h
    src/Graphics/Texture.h
    src/Graphics/GLUtils.h
    src/Game/Maze.h
    src/Game/Player.h
    src/Game/GameLogic.h
    src/Utils/FileSystem.h
    src/Utils/Logging.h
    src/Utils/Utils.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link GLFW
if(WIN32)
    target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/external/glfw/lib-vc2017/glfw3.lib)
else()
    target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/external/glfw/lib-vc2017/glfw3.lib)
endif()


# On Windows, we might need to specify additional libraries
if(WIN32)
    target_link_libraries(${PROJECT_NAME} opengl32)
endif()

# Copy shader files to build directory
file(COPY ${CMAKE_SOURCE_DIR}/shaders DESTINATION ${CMAKE_BINARY_DIR}/Debug)

# Copy texture files to build directory
file(COPY ${CMAKE_SOURCE_DIR}/textures DESTINATION ${CMAKE_BINARY_DIR}/Debug)

# Maze Escape

A 3D first-person maze escape game built with modern OpenGL.

## Description

Maze Escape is a simple 3D game where you navigate through a procedurally generated maze to find the exit. The maze is different each time you play, ensuring a unique experience with every run.

## Features

- Procedurally generated mazes
- First-person navigation
- Modern OpenGL rendering with shaders
- Dynamic lighting
- Collision detection
- Visual indicators for start and end points

## Controls

- **W, A, S, D**: Move forward, left, backward, right
- **Mouse**: Look around
- **Escape**: Exit the game

## Requirements

- OpenGL 3.3+ compatible graphics card
- GLFW
- GLAD
- GLM
- stb_image

## Building

### Windows with Visual Studio

1. Clone the repository
2. Open the project in Visual Studio
3. Build the solution
4. Run the executable

### Using CMake

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Project Structure

- `src/`: Source code files
  - `main.cpp`: Entry point and game loop
  - `shader.h/cpp`: Shader loading and management
  - `camera.h/cpp`: First-person camera implementation
  - `maze.h/cpp`: Maze generation and management
  - `renderer.h/cpp`: OpenGL rendering
- `shaders/`: GLSL shader files
  - `basic.vert`: Vertex shader
  - `basic.frag`: Fragment shader
- `external/`: External libraries
  - `glad/`: OpenGL function loader
  - `glfw/`: Window creation and input handling
  - `glm/`: Mathematics library
  - `stb_image.h`: Image loading

## Future Improvements

- Texture mapping for walls and floor
- More complex maze generation algorithms
- Monsters and combat mechanics
- Multiple levels with increasing difficulty
- Sound effects and music

## License

This project is open source and available under the MIT License.

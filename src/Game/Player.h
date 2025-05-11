#pragma once

#include <glm/glm.hpp>
#include "../Graphics/Camera.h"
#include "Maze.h"

// Represents the player in the game
class Player {
public:
    // Constructor
    Player(Camera& camera, float playerHeight = 1.0f, float playerRadius = 0.3f);

    // Update player position based on input and check for collisions
    void Update(float deltaTime, const Maze& maze);

    // Process keyboard input for movement
    void ProcessKeyboard(Camera_Movement direction, float deltaTime, const Maze& maze);

    // Get/Set position
    glm::vec3 GetPosition() const;
    void SetPosition(const glm::vec3& position);

    // Get current cell coordinates in the maze
    glm::ivec2 GetCurrentCell() const;

    // Check if player is at the exit
    bool IsAtExit(const Maze& maze) const;

private:
    // Reference to the camera (player's view)
    Camera& m_Camera;

    // Player dimensions for collision detection
    float m_Height;
    float m_Radius;

    // Current cell coordinates in the maze
    int m_CellX;
    int m_CellY;

    // Check if a position would collide with maze walls
    bool CheckCollision(const glm::vec3& position, const Maze& maze) const;

    // Update current cell coordinates based on position
    void UpdateCurrentCell();
};
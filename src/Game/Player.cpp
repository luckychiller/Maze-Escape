#include "Player.h"
#include <algorithm>
#include <iostream>

Player::Player(Camera& camera, float playerHeight, float playerRadius)
    : m_Camera(camera), m_Height(playerHeight), m_Radius(playerRadius), m_CellX(0), m_CellY(0)
{
    // Initialize current cell based on camera position
    UpdateCurrentCell();
}

void Player::Update(float deltaTime, const Maze& maze)
{
    // Update current cell coordinates based on position
    UpdateCurrentCell();
}

void Player::ProcessKeyboard(Camera_Movement direction, float deltaTime, const Maze& maze)
{
    float velocity = m_Camera.MovementSpeed * deltaTime;
    glm::vec3 newPosition = m_Camera.Position;

    // Calculate potential new position based on direction
    if (direction == FORWARD)
        newPosition += m_Camera.Front * velocity;
    if (direction == BACKWARD)
        newPosition -= m_Camera.Front * velocity;
    if (direction == LEFT)
        newPosition -= m_Camera.Right * velocity;
    if (direction == RIGHT)
        newPosition += m_Camera.Right * velocity;
    if (direction == UP)
        newPosition += m_Camera.WorldUp * velocity;
    if (direction == DOWN)
        newPosition -= m_Camera.WorldUp * velocity;

    // Check for collision with maze walls
    if (!CheckCollision(newPosition, maze))
    {
        // No collision, update camera position
        m_Camera.Position = newPosition;
        UpdateCurrentCell();
    }
    else
    {
        // Try to slide along walls (X and Z separately)
        glm::vec3 xOnlyMove = m_Camera.Position;
        xOnlyMove.x = newPosition.x;

        glm::vec3 zOnlyMove = m_Camera.Position;
        zOnlyMove.z = newPosition.z;

        // Check if we can move in X direction only
        if (!CheckCollision(xOnlyMove, maze))
        {
            m_Camera.Position = xOnlyMove;
        }

        // Check if we can move in Z direction only
        if (!CheckCollision(zOnlyMove, maze))
        {
            m_Camera.Position = zOnlyMove;
        }

        UpdateCurrentCell();
    }
}

glm::vec3 Player::GetPosition() const
{
    return m_Camera.Position;
}

void Player::SetPosition(const glm::vec3& position)
{
    m_Camera.Position = position;
    UpdateCurrentCell();
}

glm::ivec2 Player::GetCurrentCell() const
{
    return glm::ivec2(m_CellX, m_CellY);
}

bool Player::IsAtExit(const Maze& maze) const
{
    glm::ivec2 exitCoords = maze.GetEndCellCoords();
    return (m_CellX == exitCoords.x && m_CellY == exitCoords.y);
}

bool Player::CheckCollision(const glm::vec3& position, const Maze& maze) const
{
    // Calculate the cell coordinates for the new position
    int cellX = static_cast<int>(position.x);
    int cellY = static_cast<int>(position.z);

    // Check if position is out of maze bounds
    if (cellX < 0 || cellX >= maze.GetWidth() || cellY < 0 || cellY >= maze.GetHeight())
    {
        return true; // Collision with maze boundary
    }

    // Get the current cell and check for wall collisions
    const Cell& cell = maze.GetCell(cellX, cellY);

    // Calculate position within the cell (0.0 to 1.0)
    float cellPosX = position.x - cellX;
    float cellPosZ = position.z - cellY;

    // Wall thickness (from main.cpp)
    const float wallThickness = 0.1f;

    // Check collision with each wall if it exists
    if (cell.wallTop && cellPosZ < m_Radius)
    {
        return true; // Collision with top wall
    }

    if (cell.wallBottom && cellPosZ > (1.0f - m_Radius))
    {
        return true; // Collision with bottom wall
    }

    if (cell.wallLeft && cellPosX < m_Radius)
    {
        return true; // Collision with left wall
    }

    if (cell.wallRight && cellPosX > (1.0f - m_Radius))
    {
        return true; // Collision with right wall
    }

    // Check adjacent cells for their walls that might intersect with player radius

    // Check cell to the left if player is close to left edge
    if (cellX > 0 && cellPosX < m_Radius)
    {
        const Cell& leftCell = maze.GetCell(cellX - 1, cellY);
        if (leftCell.wallRight)
        {
            return true; // Collision with right wall of left cell
        }
    }

    // Check cell to the right if player is close to right edge
    if (cellX < maze.GetWidth() - 1 && cellPosX > (1.0f - m_Radius))
    {
        const Cell& rightCell = maze.GetCell(cellX + 1, cellY);
        if (rightCell.wallLeft)
        {
            return true; // Collision with left wall of right cell
        }
    }

    // Check cell above if player is close to top edge
    if (cellY > 0 && cellPosZ < m_Radius)
    {
        const Cell& topCell = maze.GetCell(cellX, cellY - 1);
        if (topCell.wallBottom)
        {
            return true; // Collision with bottom wall of top cell
        }
    }

    // Check cell below if player is close to bottom edge
    if (cellY < maze.GetHeight() - 1 && cellPosZ > (1.0f - m_Radius))
    {
        const Cell& bottomCell = maze.GetCell(cellX, cellY + 1);
        if (bottomCell.wallTop)
        {
            return true; // Collision with top wall of bottom cell
        }
    }

    return false; // No collision
}

void Player::UpdateCurrentCell()
{
    // Update current cell coordinates based on camera position
    m_CellX = static_cast<int>(m_Camera.Position.x);
    m_CellY = static_cast<int>(m_Camera.Position.z);
}
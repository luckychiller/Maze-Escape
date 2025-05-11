#include "GameLogic.h"
#include <iostream>

GameLogic::GameLogic(Player& player, Maze& maze)
    : m_Player(player), m_Maze(maze), m_State(GameState::PLAYING)
{
}

void GameLogic::Update(float deltaTime)
{
    // Update game state based on player position
    CheckWinCondition();
}

GameState GameLogic::GetState() const
{
    return m_State;
}

void GameLogic::Reset()
{
    // Reset game state to playing
    m_State = GameState::PLAYING;

    // Reset player position to start cell
    glm::ivec2 startCoords = m_Maze.GetStartCellCoords();
    if (startCoords.x >= 0 && startCoords.y >= 0)
    {
        // Position player in the center of the start cell
        m_Player.SetPosition(glm::vec3(startCoords.x + 0.5f, 1.0f, startCoords.y + 0.5f));
    }
    else
    {
        // Fallback if no start cell is defined
        m_Player.SetPosition(glm::vec3(0.5f, 1.0f, 0.5f));
    }
}

void GameLogic::CheckWinCondition()
{
    // Check if player has reached the exit
    if (m_Player.IsAtExit(m_Maze))
    {
        m_State = GameState::WON;
        std::cout << "Congratulations! You've reached the exit!" << std::endl;
    }
}
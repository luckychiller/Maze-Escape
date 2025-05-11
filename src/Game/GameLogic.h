#pragma once

#include "Player.h"
#include "Maze.h"

// Game states
enum class GameState {
    PLAYING,
    WON
};

// Manages game logic and state
class GameLogic {
public:
    // Constructor
    GameLogic(Player& player, Maze& maze);

    // Update game state
    void Update(float deltaTime);

    // Get current game state
    GameState GetState() const;

    // Reset the game
    void Reset();

private:
    // References to player and maze
    Player& m_Player;
    Maze& m_Maze;

    // Current game state
    GameState m_State;

    // Check if player has reached the exit
    void CheckWinCondition();
};
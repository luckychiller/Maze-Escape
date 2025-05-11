#pragma once

#include <vector>
#include <random>    // For maze generation
#include <stack>     // For Recursive Backtracker
#include <algorithm> // For std::shuffle
#include <iostream>  // For debugging
#include <glm/glm.hpp>

// Represents a single cell in the maze
struct Cell
{
    bool visited = false;
    bool wallTop = true;
    bool wallBottom = true;
    bool wallLeft = true;
    bool wallRight = true;

    // Optional: for pathfinding or marking start/end
    bool isStart = false;
    bool isEnd = false;
    // Optional: for rendering different floor types or items
    // int floorType = 0;
};

class Maze
{
public:
    Maze(int width, int height);
    ~Maze();

    void GenerateMaze(int startX, int startY); // Start cell for generation

    // Accessor for cell data (const version for querying)
    const Cell &GetCell(int x, int y) const;
    // Accessor for cell data (non-const for modification during generation, carefully)
    // Cell& GetCell(int x, int y); // We might not expose this publicly

    // For debugging: Print the maze to console
    void PrintToConsole() const;

    // Print the maze to console with player position
    void PrintToConsole(const glm::ivec2& playerPos) const;

    int GetWidth() const ;
    int GetHeight() const ;

    // TODO: Add methods to find a valid start and end point after generation.
    void SetStartCell(int x, int y);
    void SetEndCell(int x, int y);
    glm::ivec2 GetStartCellCoords() const;
    glm::ivec2 GetEndCellCoords() const;

private:
    int M_Width;
    int M_Height;
    std::vector<std::vector<Cell>> M_Grid;

    // Helper for generation (Recursive Backtracker)
    void carvePassagesFrom(int cx, int cy, std::mt19937 &rng);
    std::vector<std::pair<int, int>> getUnvisitedNeighbors(int x, int y);
    void removeWall(Cell &current, Cell &neighbor, int dx, int dy);

    // Random number generator for maze generation
    std::mt19937 M_Rng;
};
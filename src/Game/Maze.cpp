#include "Maze.h"
#include <chrono> // For seeding the random number generator

Maze::Maze(int width, int height) : M_Width(width), M_Height(height) {
    if (M_Width <= 0 || M_Height <= 0) {
        // Handle invalid dimensions, e.g., throw an error or default to a minimum size
        M_Width = 10;
        M_Height = 10;
        std::cerr << "Warning: Invalid maze dimensions. Defaulting to 10x10." << std::endl;
    }

    // Initialize the grid with default cells (all walls up, not visited)
    M_Grid.resize(M_Height, std::vector<Cell>(M_Width));

    // Seed the random number generator
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    M_Rng.seed(seed);
}

Maze::~Maze() {
    // Destructor, nothing specific to clean up for M_Grid if using std::vector
}

void Maze::GenerateMaze(int startX, int startY) {
    // Ensure start coordinates are within bounds
    if (startX < 0 || startX >= M_Width || startY < 0 || startY >= M_Height) {
        std::cerr << "Error: Start coordinates for maze generation are out of bounds. Using 0,0." << std::endl;
        startX = 0;
        startY = 0;
    }

    // Reset visited status for all cells (in case GenerateMaze is called multiple times)
    for (int y = 0; y < M_Height; ++y) {
        for (int x = 0; x < M_Width; ++x) {
            M_Grid[y][x].visited = false;
            // Ensure all walls are initially true
            M_Grid[y][x].wallTop = true;
            M_Grid[y][x].wallBottom = true;
            M_Grid[y][x].wallLeft = true;
            M_Grid[y][x].wallRight = true;
        }
    }
    
    // Set start and end points (simple example: start at (startX, startY), end at bottom-right)
    // You can make this more sophisticated later
    M_Grid[startY][startX].isStart = true;
    // A simple end point, could be made more robust
    int endX = M_Width - 1;
    int endY = M_Height - 1;
    if (endX == startX && endY == startY) { // Avoid start and end being the same for tiny mazes
        if (M_Width > 1) endX = M_Width - 2;
        else if (M_Height > 1) endY = M_Height - 2;
    }
    if(endX >=0 && endY >=0) M_Grid[endY][endX].isEnd = true;


    // Start the recursive backtracker algorithm
    // Using an explicit stack instead of recursion to avoid stack overflow for large mazes
    std::stack<std::pair<int, int>> stack;
    stack.push({startX, startY});
    M_Grid[startY][startX].visited = true;

    // Possible neighbor offsets (dx, dy) and corresponding wall directions
    // {dx, dy, wall_in_current, wall_in_neighbor}
    // For neighbors: (x+dx, y+dy)
    // Directions: 0: Up (-y), 1: Right (+x), 2: Down (+y), 3: Left (-x)
    // Order here will be used for shuffling: Up, Right, Down, Left
    std::vector<std::tuple<int, int, char, char>> directions = {
        {0, -1, 'T', 'B'}, // Up
        {1, 0,  'R', 'L'}, // Right
        {0, 1,  'B', 'T'}, // Down
        {-1, 0, 'L', 'R'}  // Left
    };


    while(!stack.empty()){
        std::pair<int, int> currentCoords = stack.top();
        int cx = currentCoords.first;
        int cy = currentCoords.second;

        std::vector<std::pair<int, int>> neighbors; // Store (nx, ny) of unvisited neighbors
        std::vector<std::pair<int,int>> neighbor_indices_in_directions; // Store index in 'directions' vector

        // Shuffle directions to ensure random path selection
        std::shuffle(directions.begin(), directions.end(), M_Rng);

        for(size_t i = 0; i < directions.size(); ++i) {
            int nx = cx + std::get<0>(directions[i]);
            int ny = cy + std::get<1>(directions[i]);

            if (nx >= 0 && nx < M_Width && ny >= 0 && ny < M_Height && !M_Grid[ny][nx].visited) {
                neighbors.push_back({nx, ny});
                neighbor_indices_in_directions.push_back({(int)i,0}); // Store the original index from directions
            }
        }

        if (!neighbors.empty()) {
            // Choose a random unvisited neighbor
            std::uniform_int_distribution<> distrib(0, neighbors.size() - 1);
            int choice = distrib(M_Rng);
            
            std::pair<int, int> nextCoords = neighbors[choice];
            int nx = nextCoords.first;
            int ny = nextCoords.second;
            
            int direction_tuple_idx = neighbor_indices_in_directions[choice].first;

            // Remove walls
            char wall_current = std::get<2>(directions[direction_tuple_idx]);
            char wall_neighbor = std::get<3>(directions[direction_tuple_idx]);

            if (wall_current == 'T') M_Grid[cy][cx].wallTop = false;
            else if (wall_current == 'B') M_Grid[cy][cx].wallBottom = false;
            else if (wall_current == 'L') M_Grid[cy][cx].wallLeft = false;
            else if (wall_current == 'R') M_Grid[cy][cx].wallRight = false;

            if (wall_neighbor == 'T') M_Grid[ny][nx].wallTop = false;
            else if (wall_neighbor == 'B') M_Grid[ny][nx].wallBottom = false;
            else if (wall_neighbor == 'L') M_Grid[ny][nx].wallLeft = false;
            else if (wall_neighbor == 'R') M_Grid[ny][nx].wallRight = false;

            M_Grid[ny][nx].visited = true;
            stack.push({nx, ny}); // Push chosen neighbor

        } else {
            stack.pop(); // Backtrack
        }
    }

    // Optional: A simple way to ensure the start and end are open if they are on the edge.
    // This is a basic approach; more robust handling might be needed for complex edge cases or specific designs.
    if (M_Grid[startY][startX].isStart) {
        if (startY == 0) M_Grid[startY][startX].wallTop = false; // Open to top if start is on top edge
        if (startY == M_Height - 1) M_Grid[startY][startX].wallBottom = false; // Open to bottom
        if (startX == 0) M_Grid[startY][startX].wallLeft = false; // Open to left
        if (startX == M_Width - 1) M_Grid[startY][startX].wallRight = false; // Open to right
    }
    if (M_Grid[endY][endX].isEnd) {
         // Open the end cell more deliberately - e.g., remove one wall leading "out"
        // For now, let's try to remove a wall that isn't an outer boundary if possible,
        // or just one if it's a corner.
        bool opened = false;
        if (endY > 0 && !M_Grid[endY-1][endX].wallBottom) { M_Grid[endY][endX].wallTop = false; opened = true; } // Path from above
        else if (endY < M_Height - 1 && !M_Grid[endY+1][endX].wallTop) { M_Grid[endY][endX].wallBottom = false; opened = true; } // Path from below
        else if (endX > 0 && !M_Grid[endY][endX-1].wallRight) { M_Grid[endY][endX].wallLeft = false; opened = true; } // Path from left
        else if (endX < M_Width - 1 && !M_Grid[endY][endX+1].wallLeft) { M_Grid[endY][endX].wallRight = false; opened = true; } // Path from right
        
        // Fallback if it's a corner or enclosed space, just open one outer wall
        if(!opened){
            if (endY == M_Height - 1) M_Grid[endY][endX].wallBottom = false;
            else if (endX == M_Width - 1) M_Grid[endY][endX].wallRight = false;
            else if (endY == 0) M_Grid[endY][endX].wallTop = false;
            else if (endX == 0) M_Grid[endY][endX].wallLeft = false;
        }
    }


    std::cout << "Maze generation complete." << std::endl;
}

const Cell& Maze::GetCell(int x, int y) const {
    if (x < 0 || x >= M_Width || y < 0 || y >= M_Height) {
        // Consider throwing an exception or returning a dummy 'wall' cell
        // For now, logging and returning a cell from 0,0 if out of bounds
        std::cerr << "Error: GetCell coordinates (" << x << "," << y << ") out of bounds." << std::endl;
        // This is not ideal, as it might return a valid traversable cell when it should be a wall.
        // A better approach for out-of-bounds could be to return a static 'boundary' cell
        // that has all walls set to true.
        static Cell errorCell; // Default cell (all walls true)
        errorCell.wallTop = errorCell.wallBottom = errorCell.wallLeft = errorCell.wallRight = true;
        return errorCell;
    }
    return M_Grid[y][x];
}

void Maze::PrintToConsole() const {
    for (int y = 0; y < M_Height; ++y) {
        // Print top walls of cells in this row
        for (int x = 0; x < M_Width; ++x) {
            std::cout << "+";
            std::cout << (M_Grid[y][x].wallTop ? "---" : "   ");
        }
        std::cout << "+" << std::endl;

        // Print left walls and cell contents
        for (int x = 0; x < M_Width; ++x) {
            std::cout << (M_Grid[y][x].wallLeft ? "|" : " ");
            char content = ' ';
            if (M_Grid[y][x].isStart) content = 'S';
            else if (M_Grid[y][x].isEnd) content = 'E';
            std::cout << " " << content << " ";
        }
        std::cout << (M_Grid[y][M_Width-1].wallRight ? "|" : " ") << std::endl; // Right wall of last cell in row
    }
    // Print bottom walls of the last row
    for (int x = 0; x < M_Width; ++x) {
        std::cout << "+";
        std::cout << (M_Grid[M_Height-1][x].wallBottom ? "---" : "   ");
    }
    std::cout << "+" << std::endl;
}

int Maze::GetWidth() const {
    return M_Width;
}

int Maze::GetHeight() const {
    return M_Height;
}

void Maze::SetStartCell(int x, int y) {
    if (x < 0 || x >= M_Width || y < 0 || y >= M_Height) {
        std::cerr << "Error: SetStartCell coordinates (" << x << "," << y << ") out of bounds." << std::endl;
        return;
    }
    // Reset previous start cell
    for (int i = 0; i < M_Height; ++i) {
        for (int j = 0; j < M_Width; ++j) {
            if (M_Grid[i][j].isStart) {
                M_Grid[i][j].isStart = false;
                break;
            }
        }
    }
    M_Grid[y][x].isStart = true;
}

void Maze::SetEndCell(int x, int y) {
    if (x < 0 || x >= M_Width || y < 0 || y >= M_Height) {
        std::cerr << "Error: SetEndCell coordinates (" << x << "," << y << ") out of bounds." << std::endl;
        return;
    }
    // Reset previous end cell
    for (int i = 0; i < M_Height; ++i) {
        for (int j = 0; j < M_Width; ++j) {
            if (M_Grid[i][j].isEnd) {
                M_Grid[i][j].isEnd = false;
                break;
            }
        }
    }
    M_Grid[y][x].isEnd = true;
}

glm::ivec2 Maze::GetStartCellCoords() const {
    for (int i = 0; i < M_Height; ++i) {
        for (int j = 0; j < M_Width; ++j) {
            if (M_Grid[i][j].isStart) {
                return glm::ivec2(j, i);
            }
        }
    }
    return glm::ivec2(-1, -1); // Invalid coordinates
}

glm::ivec2 Maze::GetEndCellCoords() const {
    for (int i = 0; i < M_Height; ++i) {
        for (int j = 0; j < M_Width; ++j) {
            if (M_Grid[i][j].isEnd) {
                return glm::ivec2(j, i);
            }
        }
    }
    return glm::ivec2(-1, -1); // Invalid coordinates
} 















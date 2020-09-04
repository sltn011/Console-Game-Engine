#include "Console Game Engine.hpp"
#include <stack>
#include <random>

// Generates random maze using Recursive Backtracker algorithm
class MazeGenerator : public CGE::BaseGameEngine {

    enum MazeCellState {
        NotVisited = 0,
        Path_N = 1,
        Path_W = 1 << 1,
        Path_S = 1 << 2,
        Path_E = 1 << 3,
        Visited = 1 << 4
    };

    enum Direction {
        North,
        West,
        South,
        East
    };
    
    bool userCreate() override {
        m_appName = L"Maze Generator";

        // Maze size
        m_mazeWidth = 80;
        m_mazeHeight = 50;

        // Defines how many pixels maze path occupies
        m_pathWidth = 1;

        // Creating map of the maze and mark all cells as "Not visited"
        m_mazeMap = std::make_unique<MazeCellState[]>(m_mazeWidth * m_mazeHeight);
        std::memset(m_mazeMap.get(), MazeCellState::NotVisited, sizeof(MazeCellState));

        // Starting point
        COORD start{ 0, 0 };
        m_stack.push(start);

        // We are in a starting point - so it's visited
        m_mazeMap[start.Y * m_mazeWidth + start.X] = MazeCellState::Visited;
        ++m_cellsVisited;

        // Init RNG
        std::random_device rd;
        m_rng.seed(rd());

        return true;
    }

    bool userUpdate(float elapsedTime) override {

        // Create maze if not all cells visited
        if (m_cellsVisited != m_mazeWidth * m_mazeHeight) {

            // 1: Get unvisited neighbours

            COORD &current = m_stack.top();
            std::vector<Direction> neighbours;

            auto currentCellOffset = [&](int offsetX, int offsetY) -> int {
                return (current.Y + offsetY) * m_mazeWidth + (current.X + offsetX);
            };

            // North
            if (current.Y > 0) {
                if (!(m_mazeMap[currentCellOffset(0, -1)] & MazeCellState::Visited)) {
                    neighbours.push_back(Direction::North);
                }
            }

            // West
            if (current.X > 0) {
                if (!(m_mazeMap[currentCellOffset(-1, 0)] & MazeCellState::Visited)) {
                    neighbours.push_back(Direction::West);
                }
            }
            
            // South
            if (current.Y < m_mazeHeight - 1) {
                if (!(m_mazeMap[currentCellOffset(0, 1)] & MazeCellState::Visited)) {
                    neighbours.push_back(Direction::South);
                }
            }

            // East
            if (current.X < m_mazeWidth - 1) {
                if (!(m_mazeMap[currentCellOffset(1, 0)] & MazeCellState::Visited)) {
                    neighbours.push_back(Direction::East);
                }
            }

            // 2: Check if have available neighbours
            if (!neighbours.empty()) {
                // Choose random neighbour
                Direction next = neighbours[m_rng() % neighbours.size()];

                // Create path between neighbour and current cell
                switch (next) {
                case Direction::North:
                    // Write into current cell that it has a path to neighbour
                    m_mazeMap[currentCellOffset(0, 0)] = static_cast<MazeCellState>(m_mazeMap[currentCellOffset(0, 0)] | MazeCellState::Path_N);

                    // Write into neighbour cell that it has a path to current cell
                    m_mazeMap[currentCellOffset(0, -1)] = static_cast<MazeCellState>(MazeCellState::Visited | MazeCellState::Path_S);

                    // Now we are moving to neighbour cell
                    // Pushing it on stack to make it current
                    m_stack.push({ current.X, current.Y - 1 });
                    break;

                case Direction::West:
                    // Write into current cell that it has a path to neighbour
                    m_mazeMap[currentCellOffset(0, 0)] = static_cast<MazeCellState>(m_mazeMap[currentCellOffset(0, 0)] | MazeCellState::Path_W);

                    // Write into neighbour cell that it has a path to current cell
                    m_mazeMap[currentCellOffset(-1, 0)] = static_cast<MazeCellState>(MazeCellState::Visited | MazeCellState::Path_E);

                    // Now we are moving to neighbour cell
                    // Pushing it on stack to make it current
                    m_stack.push({ current.X - 1, current.Y });
                    break;

                case Direction::South:
                    // Write into current cell that it has a path to neighbour
                    m_mazeMap[currentCellOffset(0, 0)] = static_cast<MazeCellState>(m_mazeMap[currentCellOffset(0, 0)] | MazeCellState::Path_S);

                    // Write into neighbour cell that it has a path to current cell
                    m_mazeMap[currentCellOffset(0, 1)] = static_cast<MazeCellState>(MazeCellState::Visited | MazeCellState::Path_N);

                    // Now we are moving to neighbour cell
                    // Pushing it on stack to make it current
                    m_stack.push({ current.X, current.Y + 1 });
                    break;

                case Direction::East:
                    // Write into current cell that it has a path to neighbour
                    m_mazeMap[currentCellOffset(0, 0)] = static_cast<MazeCellState>(m_mazeMap[currentCellOffset(0, 0)] | MazeCellState::Path_E);

                    // Write into neighbour cell that it has a path to current cell
                    m_mazeMap[currentCellOffset(1, 0)] = static_cast<MazeCellState>(MazeCellState::Visited | MazeCellState::Path_W);

                    // Now we are moving to neighbour cell
                    // Pushing it on stack to make it current
                    m_stack.push({ current.X + 1, current.Y });
                    break;
                }

                // We visited new cell - increment number of visited cells
                ++m_cellsVisited;
            }
            else {
                // Nowhere to go -> backtrack
                m_stack.pop();
            }

        }

        // Clear screen
        fill(0, 0, m_screenWidth, m_screenHeight, CGE::Pixel::Empty);

        // Draw maze
        for (short x = 0; x < m_mazeWidth; ++x) {
            for (short y = 0; y < m_mazeHeight; ++y) {
                
                // Draw cell - each is inflated by m_pathWidth
                for (short px = 0; px < m_pathWidth; ++px) {
                    for (short py = 0; py < m_pathWidth; ++py) {
                        if (m_mazeMap[y * m_mazeWidth + x] & MazeCellState::Visited) {
                            draw(x * (m_pathWidth + 1) + px, y * (m_pathWidth + 1) + py, CGE::Pixel::Solid, CGE::Color::FG_White);
                        }
                        else {
                            draw(x * (m_pathWidth + 1) + px, y * (m_pathWidth + 1) + py, CGE::Pixel::Solid, CGE::Color::FG_DarkBlue);
                        }
                    }
                }

                // Draw paths
                for (short p = 0; p < m_pathWidth; ++p) {
                    if (m_mazeMap[y * m_mazeWidth + x] & MazeCellState::Path_S) {
                        draw(x * (m_pathWidth + 1) + p, y * (m_pathWidth + 1) + m_pathWidth, CGE::Pixel::Solid, CGE::Color::FG_White);
                    }

                    if (m_mazeMap[y * m_mazeWidth + x] & MazeCellState::Path_E) {
                        draw(x * (m_pathWidth + 1) + m_pathWidth, y * (m_pathWidth + 1) + p, CGE::Pixel::Solid, CGE::Color::FG_White);
                    }
                }

            }
        }

        // Draw current cell with different color
        for (short px = 0; px < m_pathWidth; ++px) {
            for (short py = 0; py < m_pathWidth; ++py) {
                draw(m_stack.top().X * (m_pathWidth + 1) + px, m_stack.top().Y * (m_pathWidth + 1) + py, CGE::Pixel::Solid, CGE::Color::FG_DarkYellow);
            }
        }

        return true;
    }

    // Maze size
    short m_mazeWidth;
    short m_mazeHeight;

    std::unique_ptr<MazeCellState[]> m_mazeMap;

    // All cells must be visited by algorithm
    // thats how we know when maze fully generated
    size_t m_cellsVisited = 0;

    // Maze generation algorithm is based on
    // Recursive Backtracker method
    // We will need stack for this
    std::stack<COORD> m_stack;

    // How many pixels path occupies
    short m_pathWidth;

    // RNG
    std::mt19937 m_rng;

};

int main() {
    MazeGenerator maze;
    if (maze.createConsole(160, 100, 8, 8)) {
        maze.start();
    }
    return 0;
}
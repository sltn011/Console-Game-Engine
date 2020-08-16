#include "Console Game Engine.hpp"
#include <vector>
#include <algorithm>
#include <list>

class ConsoleFPS : public CGE::BaseGameEngine {
public:

    ConsoleFPS() {
        m_brickWall.readFromFile("Console FPS Sprites/BrickWall.sprite");
        m_lampPost.readFromFile("Console FPS Sprites/LampPost.sprite");
        m_bullet.readFromFile("Console FPS Sprites/BulletShot.sprite");
        m_appName = L"Console FPS";
    }

    bool userCreate() override {
                                                      // Y
        m_map += L"################################"; // 0
        m_map += L"#................#.............#"; // 1
        m_map += L"#................#.............#"; // 2
        m_map += L"#..........#######.............#"; // 3
        m_map += L"#...............##.............#"; // 4
        m_map += L"#......##.......##.............#"; // 5
        m_map += L"#......##......................#"; // 6
        m_map += L"#.......#..............##......#"; // 7
        m_map += L"#......................####..###"; // 8
        m_map += L"#...................#####......#"; // 9
        m_map += L"#......................##......#"; // 10
        m_map += L"#########....####..............#"; // 11
        m_map += L"#..............................#"; // 12
        m_map += L"###...........##...............#"; // 13
        m_map += L"#.............########....######"; // 14
        m_map += L"#.............##...............#"; // 15
        m_map += L"#......###..####...............#"; // 16
        m_map += L"#.............###..#############"; // 17
        m_map += L"#.............##...............#"; // 18
        m_map += L"#########...####...............#"; // 19
        m_map += L"#.........................#....#"; // 20
        m_map += L"#.........................#....#"; // 21
        m_map += L"#.........#########.......#....#"; // 22
        m_map += L"#.............##........###....#"; // 23
        m_map += L"#..............................#"; // 24
        m_map += L"#.............###..............#"; // 25
        m_map += L"#...............#..............#"; // 26
        m_map += L"#...............####...........#"; // 27
        m_map += L"#.........#######..............#"; // 28
        m_map += L"#...............#..............#"; // 29
        m_map += L"#..............................#"; // 30
        m_map += L"################################"; // 31

        m_objects = {
            {0.0f, {2.5f, 2.5f, 0.0f, 0.0f, false, m_lampPost}},
            {0.0f, {9.5f, 9.5f, 0.0f, 0.0f, false, m_lampPost}},
            {0.0f, {2.5f, 17.5f, 0.0f, 0.0f, false, m_lampPost}},
            {0.0f, {24.5f, 3.5f, 0.0f, 0.0f, false, m_lampPost}},
            {0.0f, {5.5f, 25.5f, 0.0f, 0.0f, false, m_lampPost}},
            {0.0f, {26.5f, 28.5f, 0.0f, 0.0f, false, m_lampPost}},
            {0.0f, {22.5f, 19.5f, 0.0f, 0.0f, false, m_lampPost}},
            {0.0f, {24.5f, 26.5f, 0.0f, 0.0f, false, m_lampPost}}
        };

        m_fDepthBuffer = std::make_unique<float[]>(m_screenWidth);

        return true;

    }

    bool userUpdate(float fElapsedTime) override {
        // Keyboard controls - WASD
        if (getKey(L'W').isHeld) {
            m_fPlayerX += m_fPlayerMoveSpeed * std::cosf(m_fPlayerA) * fElapsedTime;
            m_fPlayerY += m_fPlayerMoveSpeed * std::sinf(m_fPlayerA) * fElapsedTime;
            if (m_map[static_cast<int>(m_fPlayerY) * m_nMapWidth + static_cast<int>(m_fPlayerX)] == L'#') {
                m_fPlayerX -= m_fPlayerMoveSpeed * std::cosf(m_fPlayerA) * fElapsedTime;
                m_fPlayerY -= m_fPlayerMoveSpeed * std::sinf(m_fPlayerA) * fElapsedTime;
            }
        }
        if (getKey(L'S').isHeld) {
            m_fPlayerX -= m_fPlayerMoveSpeed * std::cosf(m_fPlayerA) * fElapsedTime;
            m_fPlayerY -= m_fPlayerMoveSpeed * std::sinf(m_fPlayerA) * fElapsedTime;
            if (m_map[static_cast<int>(m_fPlayerY) * m_nMapWidth + static_cast<int>(m_fPlayerX)] == L'#') {
                m_fPlayerX += m_fPlayerMoveSpeed * std::cosf(m_fPlayerA) * fElapsedTime;
                m_fPlayerY += m_fPlayerMoveSpeed * std::sinf(m_fPlayerA) * fElapsedTime;
            }
        }
        // Strafe left
        if (getKey(L'Q').isHeld) {
            m_fPlayerX += m_fPlayerRotationSpeed * std::sinf(m_fPlayerA) * fElapsedTime;
            m_fPlayerY -= m_fPlayerRotationSpeed * std::cosf(m_fPlayerA) * fElapsedTime;
            if (m_map[static_cast<int>(m_fPlayerY) * m_nMapWidth + static_cast<int>(m_fPlayerX)] == L'#') {
                m_fPlayerX -= m_fPlayerRotationSpeed * std::sinf(m_fPlayerA) * fElapsedTime;
                m_fPlayerY += m_fPlayerRotationSpeed * std::cosf(m_fPlayerA) * fElapsedTime;
            }
        }
        // Strafe right
        if (getKey(L'E').isHeld) {
            m_fPlayerX -= m_fPlayerRotationSpeed * std::sinf(m_fPlayerA) * fElapsedTime;
            m_fPlayerY += m_fPlayerRotationSpeed * std::cosf(m_fPlayerA) * fElapsedTime;
            if (m_map[static_cast<int>(m_fPlayerY) * m_nMapWidth + static_cast<int>(m_fPlayerX)] == L'#') {
                m_fPlayerX += m_fPlayerRotationSpeed * std::sinf(m_fPlayerA) * fElapsedTime;
                m_fPlayerY -= m_fPlayerRotationSpeed * std::cosf(m_fPlayerA) * fElapsedTime;
            }
        }
        if (getKey(L'A').isHeld) {
            m_fPlayerA -= m_fPlayerRotationSpeed * fElapsedTime;
        }
        if (getKey(L'D').isHeld) {
            m_fPlayerA += m_fPlayerRotationSpeed * fElapsedTime;
        }

        // Shooting
        if (getKey(VK_SPACE).isReleased) {
            gameObj bullet = { m_fPlayerX, m_fPlayerY, 0.0f, 0.0f, false, m_bullet };

            float fNoise = ((static_cast<float>(std::rand()) / RAND_MAX) - 0.5f) * 0.1f;
            bullet.vx = std::cosf(m_fPlayerA + fNoise) * m_bulletSpeed;
            bullet.vy = std::sinf(m_fPlayerA + fNoise) * m_bulletSpeed;

            m_objects.emplace_back(0.0f, std::move(bullet));
        }

        // For each column on screen we do calculations
        for (int x = 0; x < m_screenWidth; ++x) {
            // Calculating angle at which player looks at "pixel column"
            float fRayAngle = (m_fPlayerA - (m_fPlayerFOV / 2.0f)) + (static_cast<float>(x) * (m_fPlayerFOV / static_cast<float>(m_screenWidth)));

            // Flag for drawing wall
            bool bHitWall = false;

            // Variables for sampling wall texture
            float fSampleX = 0.0f;
            float fSampleY = 0.0f;

            // Unit vector to the current pixel column
            float fEyeX = std::cosf(fRayAngle);
            float fEyeY = std::sinf(fRayAngle);

            // Variable for wall searching
            float fDistanceToWall = 0.0f;

            // Cycle for searching walls in player's FOV
            // Continues untill we hit a wall or reach render depth
            while (!bHitWall && fDistanceToWall < m_fRenderDepth) {
                // Each time increase variable 
                fDistanceToWall += 0.005f;

                // Coordinates of a point on map in which our "wall-searching vector" currently is
                int nTestX = static_cast<int>(m_fPlayerX + fEyeX * fDistanceToWall);
                int nTestY = static_cast<int>(m_fPlayerY + fEyeY * fDistanceToWall);

                // Some error happened - we broke map boundary somehow
                // Treat this case as "hitting wall" and breaking cycle
                if (nTestX < 0 || nTestX >= m_nMapWidth || nTestY < 0 || nTestY >= m_nMapHeight) {
                    bHitWall = true;
                    fDistanceToWall = m_fRenderDepth;
                }
                else {
                    // Our vector found wall
                    if (m_map[nTestY * m_nMapWidth + nTestX] == L'#') {
                        bHitWall = true;

                        // Saving distance to wall for this column in depth buffer(used in rendering objects)
                        m_fDepthBuffer[x] = fDistanceToWall;

                        // Center of block that we hit
                        float fBlockMidX = float(nTestX) + 0.5f;
                        float fBlockMidY = float(nTestY) + 0.5f;

                        // Coordinates of a collision point of searching vector and wall 
                        float fTestPointX = m_fPlayerX + fEyeX * fDistanceToWall;
                        float fTestPointY = m_fPlayerY + fEyeY * fDistanceToWall;

                        // Angle between block center and collision point
                        float fTestAngle = std::atan2f((fTestPointY - fBlockMidY), (fTestPointX - fBlockMidX));

                        // Using angle we can get quadrant in which we hit the wall
                        // and find out which value use for horizontal sampling
                        // Quadrants are made by rotating XOY by PI/4 so axes create an X with center in block center
                        //
                        //         <- (-x)
                        //      | [OUR WALL] ^        Explanation
                        // (-y) v [ BLOCK  ] | (+y) 
                        //          (+x) ->
                        //
                        if (fTestAngle >= -pi * 0.25f && fTestAngle < pi * 0.25f) {
                            fSampleX = fTestPointY;
                        }
                        else if (fTestAngle >= pi * 0.25f && fTestAngle < pi * 0.75f) {
                            fSampleX = -fTestPointX;
                        }
                        else if (fTestAngle >= -pi * 0.75f && fTestAngle < -pi * 0.25f) {
                            fSampleX = fTestPointX;
                        }
                        else  {
                            fSampleX = -fTestPointY;
                        }
                    }
                }
            }

            // Distance between floor and ceiling
            // The bigger the distance to wall, the bigger floor and ceiling appear
            int nCeiling = static_cast<int>(static_cast<float>(m_screenHeight / 2.0f) - m_screenHeight / static_cast<float>(fDistanceToWall));
            int nFloor = m_screenHeight - nCeiling;

            // Cycle for rendering column
            for (int y = 0; y < m_screenHeight; ++y) {
                // If ceiling we just leave it black
                if (y < nCeiling) {
                    draw(x, y, CGE::Pixel::Empty, CGE::Color::FG_Black);
                }
                // If drawing the wall
                else if (y >= nCeiling && y <= nFloor) {
                    // Wall is in render distance
                    if (fDistanceToWall < m_fRenderDepth) {
                        fSampleY = (static_cast<float>(y) - static_cast<float>(nCeiling)) / (static_cast<float>(nFloor) - static_cast<float>(nCeiling));
                        // Using normalized coordinates for sampling and getting wall texture pixel
                        draw(x, y, CGE::Pixel::Solid, m_brickWall.sampleColor(fSampleX, fSampleY));
                    }
                    else {
                        // Wall is not in render distance - we paint it black
                        draw(x, y, CGE::Pixel::Empty, CGE::Color::FG_Black);
                    }
                }
                else {
                    // Drawing the floor
                    // The more distance to floor, the darker it appears
                    float fFloorShadeCoef = 1.0f - (static_cast<float>(y) - m_screenHeight / 2.0f) / (static_cast<float>(m_screenHeight) / 2.0f);
                    short nShade = getFloorShade(fFloorShadeCoef);
                    draw(x, y, nShade, CGE::Color::FG_DarkGreen);
                }
            }
        }
        
        // Sorting list of objects by distance to player
        // This ordering is used in rendering - we need to draw closes object last
        // so textures don't overlap
        auto sortByDistance = [](auto const &p1, auto const &p2) {
            return p1.first > p2.first;
        };
        m_objects.sort(sortByDistance);

        // Drawing objects
        for (auto &obj : m_objects) {

            // Objects that have moving speed travel some distance
            obj.second.x += obj.second.vx * fElapsedTime;
            obj.second.y += obj.second.vy * fElapsedTime;

            // If moving object hit the wall we mark it as "object that should be removed"
            if (m_map[static_cast<int>(obj.second.y) * m_nMapWidth + static_cast<int>(obj.second.x)] == L'#') {
                obj.second.doRemove = true;
            }

            // Checking if player can see object in his FOV
            // Calculating vector from player to object
            float fVecX = obj.second.x - m_fPlayerX;
            float fVecY = obj.second.y - m_fPlayerY;
            float distanceToPlayer = std::sqrtf(fVecX * fVecX + fVecY * fVecY);

            // If object is too far away - we just skip it
            if (distanceToPlayer > m_fRenderDepth || distanceToPlayer <= 0.5f) {
                continue;
            }

            // Checking if object is in FOV using angles
            float fEyeX = std::cosf(m_fPlayerA);
            float fEyeY = std::sinf(m_fPlayerA);

            float fObjAngle = std::atan2f(fVecY, fVecX) - std::atan2f(fEyeY, fEyeX);
            if (fObjAngle < -pi) {
                fObjAngle += 2.0f * pi;
            }
            else if (fObjAngle > pi) {
                fObjAngle -= 2.0f * pi;
            }

            bool isInPlayerFOV = std::fabs(fObjAngle) < m_fPlayerFOV / 2.0f;

            // If player can see object we render it
            if (isInPlayerFOV) {
                // Calculating size of ceiling and floor above and below object by distance to object
                float fObjectCeiling = (m_screenHeight / 2.0f) - (m_screenHeight / distanceToPlayer);
                float fObjectFloor = m_screenHeight - fObjectCeiling;
                float fObjectHeight = fObjectFloor - fObjectCeiling;

                // Calculating dimensions of object - the farer it is the smaller it appears
                float fObjectAspectRatio = static_cast<float>(obj.second.s.getHeight()) / static_cast<float>(obj.second.s.getWidth());
                float fObjectWidth = fObjectHeight / fObjectAspectRatio;
                float fObjectMiddle = (0.5f * (fObjAngle / (m_fPlayerFOV / 2.0f)) + 0.5f) * static_cast<float>(m_screenWidth);

                // Rendering object using sampling
                for (float lx = 0.0f; lx < fObjectWidth; ++lx) {
                    for (float ly = 0.0f; ly < fObjectHeight; ++ly) {
                        float fSampleX = lx / fObjectWidth;
                        float fSampleY = ly / fObjectHeight;
                        CGE::basePixelType c = obj.second.s.samplePixel(fSampleX, fSampleY);
                        int nObjColumn = static_cast<int>(fObjectMiddle - (fObjectWidth / 2.0f) + lx);
                        if (nObjColumn >= 0.0f && nObjColumn < m_screenWidth) {
                            // Drawing object taking into consideration transparency of its parts
                            if (c != CGE::Pixel::Empty && m_fDepthBuffer[nObjColumn] >= distanceToPlayer) {
                                draw(static_cast<short>(nObjColumn), static_cast<short>(fObjectCeiling + ly), c, obj.second.s.sampleColor(fSampleX, fSampleY));
                            }
                        }
                    }
                }
            }
            // Saving distance from object to player for next time calculations
            obj.first = distanceToPlayer;
        }

        // Deleting "marked" objects
        m_objects.remove_if([](auto const &p) { return p.second.doRemove == true; });

        // Printing level map at the top left corner
        for (int nx = 0; nx < m_nMapWidth; ++nx) {
            for (int ny = 0; ny < m_nMapHeight; ++ny) {
                draw(nx, ny, static_cast<CGE::Pixel>(m_map[ny * m_nMapWidth + nx]), CGE::Color::FG_White);
            }
        }
        draw(static_cast<int>(m_fPlayerX), static_cast<int>(m_fPlayerY), static_cast<CGE::Pixel>(L'P'), CGE::Color::FG_White);

        return true;
    }

private:
    static constexpr float pi = 3.141592f;

    // Player data - coordinates, speed, FOV
    float m_fPlayerX = 1.0f;
    float m_fPlayerY = 22.0f;
    float m_fPlayerA = 0.0f;
    float m_fPlayerMoveSpeed = 4.0f;
    float m_fPlayerRotationSpeed = 2.0f;
    float m_fPlayerFOV = pi / 4.0f;

    float m_bulletSpeed = 2 * m_fPlayerMoveSpeed;

    float m_fRenderDepth = 16.0f;

    short m_nMapWidth = 32;
    short m_nMapHeight = 32;

    std::wstring m_map;

    CGE::Sprite m_brickWall;
    CGE::Sprite m_lampPost;
    CGE::Sprite m_bullet;

    struct gameObj {
        float        x;
        float        y;
        float       vx;
        float       vy;
        bool  doRemove;
        CGE::Sprite &s;
    };

    // List of pairs (distance to object, object data)
    // distance is used in rendering
    std::list<std::pair<float, gameObj>> m_objects;

    // Depth buffer used for rendering - we dont want objects that are behind the wall
    // to be rendered in front of it and overlap with wall texture
    std::unique_ptr<float[]> m_fDepthBuffer;

    short getFloorShade(float const floorShadeCoef) {
        if (floorShadeCoef < 0.25f)     return '#'; // Part of floor is close to us
        else if (floorShadeCoef < 0.5f) return 'x';
        else if (floorShadeCoef < 0.75) return '-';
        else if (floorShadeCoef < 0.9)  return '.';
        else                            return ' '; // Part of floor is very far
    }
};

int main() {
    ConsoleFPS engine;

    // Creating console - you can tweak these numbers but be careful
    // Some changes can drop framerate very low or even fail constructing console
    if (engine.createConsole(320, 240, 3, 3)) {

        // Launching the game if console successfully created
        engine.start();
    }
}
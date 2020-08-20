#pragma once

/*
My rewrite of olcGameEngine by Javidx9: https://github.com/OneLoneCoder
These files are made only for myself for educational purposes.

Original files can be found here: https://github.com/OneLoneCoder/videos
*/

#ifdef _WIN32
#include <Windows.h>
#else
#error This code only works on Windows!!!
#endif

#ifndef UNICODE
#error Please, turn on Unicode support:\
VS: Project Properties -> General -> Character Set -> Use Unicode
#endif

#include <iostream>
#include <string>
#include <cstdio>
#include <cmath>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace CGE {

    // Type that can fit console color - uint16_t alias
    using baseColorType = uint16_t;

    // Type that can fit console pixel - uint16_t alias
    using basePixelType = uint16_t;

    // Enum with colors that can be used with console graphic
    // FG colors are used for symbols
    // BG colors are used for background
    enum Color : baseColorType {
        FG_Black =          0x0000,
        FG_DarkBlue =       0x0001,
        FG_DarkGreen =      0x0002,
        FG_DarkCyan =       0x0003,
        FG_DarkRed =        0x0004,
        FG_DarkMagenta =    0x0005,
        FG_DarkYellow =     0x0006,
        FG_Grey =           0x0007,
        FG_DarkGrey =       0x0008,
        FG_Blue =           0x0009,
        FG_Green =          0x000A,
        FG_Cyan =           0x000B,
        FG_Red =            0x000C,
        FG_Magenta =        0x000D,
        FG_Yellow =         0x000E,
        FG_White =          0x000F,

        BG_Black =          0x0000,
        BG_DarkBlue =       0x0010,
        BG_DarkGreen =      0x0020,
        BG_DarkCyan =       0x0030,
        BG_DarkRed =        0x0040,
        BG_DarkMagenta =    0x0050,
        BG_DarkYellow =     0x0060,
        BG_Grey =           0x0070,
        BG_DarkGrey =       0x0080,
        BG_Blue =           0x0090,
        BG_Green =          0x00A0,
        BG_Cyan =           0x00B0,
        BG_Red =            0x00C0,
        BG_Magenta =        0x00D0,
        BG_Yellow =         0x00E0,
        BG_White =          0x00F0,
    };

    // Enum with different types of pixels you can use to draw
    enum Pixel : basePixelType {
        Solid =         0x2588,
        ThreeQuarters = 0x2593,
        Half =          0x2592,
        Quarter =       0x2591,
        Empty   =       0x0020
    };

    // Class for in-game images
    class Sprite {
    public:

        Sprite
        (
        ) {
            m_width = 8;
            m_height = 8;
            m_pixels = std::make_unique<basePixelType[]>(m_width * m_height);
            m_colors = std::make_unique<baseColorType[]>(m_width * m_height);
            for (short i = 0; i < m_width * m_height; ++i) {
                m_pixels[i] = Pixel::Empty;
                m_colors[i] = Color::FG_Black;
            }
        }

        // Blank Sprite instance
        Sprite
        ( short width
        , short height
        ) {
            m_width = width;
            m_height = height;
            m_pixels = std::make_unique<basePixelType[]>(m_width * m_height);
            m_colors = std::make_unique<baseColorType[]>(m_width * m_height);
            for (short i = 0; i < m_width * m_height; ++i) {
                m_pixels[i] = Pixel::Empty;
                m_colors[i] = Color::FG_Black;
            }
        }

        // Reading Sprite data from binary file
        Sprite
        ( std::string const &filePath
        ) {
            readFromFile(filePath);
        }

        Sprite
        ( Sprite const &s
        ) = delete;

        Sprite
        ( Sprite &&s
        ) noexcept = default;

        short getWidth
        (
        ) const {
            return m_width;
        }

        short getHeight
        (
        ) const {
            return m_height;
        }

        void setPixel
        ( short x
        , short y
        , basePixelType p
        ) {
            if (x >= m_width || y >= m_height) {
                return;
            }
            else {
                m_pixels[y * m_width + x] = p;
            }
        }

        void setColor
        ( short x
        , short y
        , baseColorType c
        ) {
            if (x >= m_width || y >= m_height) {
                return;
            }
            else {
                m_colors[y * m_width + x] = c;
            }
        }

        basePixelType getPixel
        ( short x
        , short y
        ) const {
            if (x >= m_width || y >= m_height) {
                return Pixel::Empty;
            }
            else {
                return m_pixels[y * m_width + x];
            }
        }

        baseColorType getColor
        ( short x
        , short y
        ) const {
            if (x >= m_width || y >= m_height) {
                return Color::FG_Black;
            }
            else {
                return m_colors[y * m_width + x];
            }
        }

        // Can be used to get pixel data from sprite with normalized coordinates
        // where coordinates are between 0 and 1
        basePixelType samplePixel
        ( float nx
        , float ny
        ) const {
            float sx = sampleCoord(nx);
            float sy = sampleCoord(ny);
            int x = (static_cast<int>(std::roundf(sx * m_width)) % m_width);
            int y = (static_cast<int>(std::roundf(sy * m_height)) % m_height);
            return getPixel(x, y);
        }

        // Can be used to get color data from sprite with normalized coordinates
        // where coordinates are between 0 and 1
        baseColorType sampleColor
        ( float nx
        , float ny
        ) const {
            float sx = sampleCoord(nx);
            float sy = sampleCoord(ny);
            int x = (static_cast<int>(std::roundf(sx * m_width)) % m_width);
            int y = (static_cast<int>(std::roundf(sy * m_height)) % m_height);
            return getColor(x, y);
        }

        std::wstring pixelsToWString
        (
        ) const {
            return std::wstring(reinterpret_cast<wchar_t*>(m_pixels.get()), reinterpret_cast<wchar_t*>(m_pixels.get() + m_width * m_height));
        }

        std::wstring colorsToWString
        (
        ) const {
            return std::wstring(reinterpret_cast<wchar_t*>(m_colors.get()), reinterpret_cast<wchar_t*>(m_colors.get() + m_width * m_height));
        }

        // Writes sprite to binary file
        bool writeToFile
        ( std::string const &filePath
        ) const {
            std::FILE *f;
            fopen_s(&f, filePath.c_str(), "wb");
            if (!f) {
                return false;
            }
            std::fwrite(&m_width, sizeof(m_width), 1, f);
            std::fwrite(&m_height, sizeof(m_height), 1, f);
            std::fwrite(m_pixels.get(), sizeof(m_pixels[0]), m_width * m_height, f);
            std::fwrite(m_colors.get(), sizeof(m_colors[0]), m_width * m_height, f);
            std::fclose(f);
            return true;
        }

        // Reads sprite from binary file
        // If fails - doesnt mess with old instance
        bool readFromFile
        ( std::string const &filePath
        ) {
            std::FILE *f;
            fopen_s(&f, filePath.c_str(), "rb");
            if (!f) {
                return false;
            }
            std::fread(&m_width, sizeof(m_width), 1, f);
            std::fread(&m_height, sizeof(m_height), 1, f);
            m_pixels = std::make_unique<basePixelType[]>(m_width * m_height);
            m_colors = std::make_unique<baseColorType[]>(m_width * m_height);
            std::fread(m_pixels.get(), sizeof(m_pixels[0]), m_width * m_height, f);
            std::fread(m_colors.get(), sizeof(m_colors[0]), m_width * m_height, f);
            std::fclose(f);
            return true;
        }

    private:
        // Sprite size
        short m_width = 0;
        short m_height = 0;

        // Arrays that represent how sprite looks like
        std::unique_ptr<basePixelType[]> m_pixels;
        std::unique_ptr<basePixelType[]> m_colors;

        static float sampleCoord
        ( float c
        ) {
            float frac = std::modf(c, &c);
            return (std::signbit(frac) ? (1.0f + frac) : frac);
        }
    };

    class BaseGameEngine {
    public:
        BaseGameEngine
        (
        ) {
            m_screenHandler = GetStdHandle(STD_OUTPUT_HANDLE);
            m_inputHandler = GetStdHandle(STD_INPUT_HANDLE);
            m_appName = L"Default";
        }

        virtual ~BaseGameEngine
        (
        ) {
            SetConsoleActiveScreenBuffer(m_originalScreenHandler);
        }

        static BOOL CloseHandler(DWORD evt)
        {
            // Note this gets called in a seperate OS thread, so it must
            // only exit when the game has finished cleaning up, or else
            // the process will be killed before OnUserDestroy() has finished
            if (evt == CTRL_CLOSE_EVENT)
            {
                m_atomActive = false;

                // Wait for thread to be exited
                std::unique_lock<std::mutex> ul(m_muxGame);
                m_gameFinished.wait(ul);
            }
            return true;
        }

        bool createConsole
        ( short screenWidth
        , short screenHeight
        , short fontWidth
        , short fontHeight
        ) {
            if (m_screenHandler == INVALID_HANDLE_VALUE || m_inputHandler == INVALID_HANDLE_VALUE) {
                reportError(L"Bad handle recieved!");
                return false;
            }

            m_screenWidth = screenWidth;
            m_screenHeight = screenHeight;

            // Console can behave differently on some systems
            // and there's no info why in MSDN
            // Partial solution for this is taken from original code
            // by Javidx9 - https://github.com/OneLoneCoder/videos/blob/master/olcConsoleGameEngine.h

            // Change console visual size to a minimum so ScreenBuffer can shrink
            // below the actual visual size
            m_rectWindow = { 0, 0, 1, 1 };
            if (!SetConsoleWindowInfo(m_screenHandler, TRUE, &m_rectWindow)) {
                reportError(L"SetConsoleWindowInfo failed!");
                return false;
            }

            // Set the size of the screen buffer
            COORD coord{ m_screenWidth, m_screenHeight };
            if (!SetConsoleScreenBufferSize(m_screenHandler, coord)) {
                reportError(L"SetConsoleScreenBufferSize failed!");
                return false;
            }

            // Assign screen buffer to the console
            if (!SetConsoleActiveScreenBuffer(m_screenHandler)) {
                reportError(L"SetConsoleActiveScreenBuffer failed!");
                return false;
            }

            // Set the font size now that the screen buffer has been assigned to the console
            CONSOLE_FONT_INFOEX fontInfo{};
            fontInfo.cbSize = sizeof(fontInfo);
            fontInfo.nFont = 0;
            fontInfo.dwFontSize.X = fontWidth;
            fontInfo.dwFontSize.Y = fontHeight;
            fontInfo.FontFamily = FF_DONTCARE;
            fontInfo.FontWeight = FW_NORMAL;
            wcscpy_s(fontInfo.FaceName, L"Consolas");
            if (!SetCurrentConsoleFontEx(m_screenHandler, FALSE, &fontInfo)) {
                reportError(L"SetCurrentConsoleFontEx failed!");
                return false;
            }

            // Get screen buffer info and check the maximum allowed window size. 
            // Return error if exceeded, so user knows their dimensions/fontsize are too large
            CONSOLE_SCREEN_BUFFER_INFO scrInfo{};
            if (!GetConsoleScreenBufferInfo(m_screenHandler, &scrInfo)) {
                reportError(L"GetConsoleScreenBufferInfo failed!");
                return false;
            }
            if (m_screenWidth > scrInfo.dwMaximumWindowSize.X) {
                reportError(L"Requested screen width was too big, failed to create such screen!");
                return false;
            }
            if (m_screenHeight > scrInfo.dwMaximumWindowSize.Y) {
                reportError(L"Requested screen height was too big, failed to create such screen!");
                return false;
            }
         
            // Set Physical Console Window Size
            m_rectWindow = { 0, 0, m_screenWidth - 1, m_screenHeight - 1 };
            if (!SetConsoleWindowInfo(m_screenHandler, TRUE, &m_rectWindow)) {
                reportError(L"SetConsoleWindowInfo failed!");
                return false;
            }

            m_screenBuf = std::make_unique<CHAR_INFO[]>(m_screenWidth * m_screenHeight);
            std::memset(m_screenBuf.get(), 0, m_screenWidth * m_screenHeight * sizeof(CHAR_INFO));

            SetConsoleCtrlHandler(reinterpret_cast<PHANDLER_ROUTINE>(CloseHandler), TRUE);

            SetConsoleTitleW(m_appName.c_str());

            return true;
        }

        void start
        (
        ) {
            m_atomActive = true;
            std::thread gameThread(&BaseGameEngine::gameThread, this);
            gameThread.join();
        }

        void draw
        ( short x
        , short y
        , basePixelType pix = Pixel::Solid
        , baseColorType col = Color::FG_White
        ) {
            if (x >= 0 && x < m_screenWidth && y >= 0 && y < m_screenHeight) {
                m_screenBuf.get()[y * m_screenWidth + x].Char.UnicodeChar = pix;
                m_screenBuf.get()[y * m_screenWidth + x].Attributes = col;
            }
        }

        void fill
        ( short fromX
        , short fromY
        , short toX
        , short toY
        , basePixelType pix = Pixel::Solid
        , baseColorType col = Color::FG_White
        ) {
            clipCoords(fromX, fromY);
            clipCoords(toX, toY);
            for (short x = fromX; x < toX; ++x) {
                for (short y = fromY; y < toY; ++y) {
                    draw(x, y, pix, col);
                }
            }
        }

        void drawString
        ( short x
        , short y
        , std::wstring const &str
        , baseColorType col = Color::FG_White
        ) {
            for (size_t i = 0; i < str.size(); ++i) {
                // Not calling draw method to allow line breaks
                m_screenBuf.get()[y * m_screenWidth + x + i].Char.UnicodeChar = str.at(i);
                m_screenBuf.get()[y * m_screenWidth + x + i].Attributes = col;
            }
        }

        // Skips whitespace character instead of printing it
        void drawStringAlpha
        ( short x
        , short y
        , std::wstring const &str
        , baseColorType col = Color::FG_White
        ) {
            for (size_t i = 0; i < str.size(); ++i) {
                if (str.at(i) != L' ') {
                    // Not calling draw method to allow line breaks
                    m_screenBuf.get()[y * m_screenWidth + x + i].Char.UnicodeChar = str.at(i);
                    m_screenBuf.get()[y * m_screenWidth + x + i].Attributes = col;
                }
            }
        }

        // Bresenham’s Line Generation algorithm
        // https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
        void drawLine
        ( short x1
        , short y1
        , short x2
        , short y2
        , basePixelType pix = Pixel::Solid
        , baseColorType col = Color::FG_White
        ) {
            int dx = x2 - x1;
            int dy = y2 - y1;
            int dxAbs = std::abs(dx);
            int dyAbs = std::abs(dy);
            int px = 2 * dyAbs - dxAbs;
            int py = 2 * dxAbs - dyAbs;
            int xBegin, xEnd, yBegin, yEnd;
            if (dxAbs > dyAbs) {
                if (dx >= 0) {
                    xBegin = x1;
                    xEnd = x2;
                    yBegin = y1;
                }
                else {
                    xBegin = x2;
                    xEnd = x1;
                    yBegin = y2;
                }
                for (; xBegin < xEnd; ++xBegin) {
                    if (px < 0) {
                        px += 2 * dyAbs;
                    }
                    else {
                        if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
                            ++yBegin;
                        }
                        else {
                            --yBegin;
                        }
                        px += 2 * (dyAbs - dxAbs);
                    }
                    draw(xBegin, yBegin, pix, col);
                }
            }
            else{
                if (dy >= 0) {
                    yBegin = y1;
                    yEnd = y2;
                    xBegin = x1;
                }
                else {
                    yBegin = y2;
                    yEnd = y1;
                    xBegin = x2;
                }
                for (; yBegin < yEnd; ++yBegin) {
                    if (py < 0) {
                        py += 2 * dxAbs;
                    }
                    else {
                        if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
                            ++xBegin;
                        }
                        else {
                            --xBegin;
                        }
                        py += 2 * (dxAbs - dyAbs);
                    }
                    draw(xBegin, yBegin, pix, col);
                }
            }
        }

        void drawTriangle
        ( short x1
        , short y1
        , short x2
        , short y2
        , short x3
        , short y3
        , basePixelType pix = Pixel::Solid
        , baseColorType col = Color::FG_White
        ) {
            drawLine(x1, y1, x2, y2, pix, col);
            drawLine(x2, y2, x3, y3, pix, col);
            drawLine(x1, y1, x3, y3, pix, col);
        }

        // https://www.avrfreaks.net/sites/default/files/triangles.c - Bresenham method
        void fillTriangle
        ( short x1
        , short y1
        , short x2
        , short y2
        , short x3
        , short y3
        , basePixelType pix = Pixel::Solid
        , baseColorType col = Color::FG_White
        ) {
            if (y1 > y2) { std::swap(y1, y2); std::swap(x1, x2); }
            if (y1 > y3) { std::swap(y1, y3); std::swap(x1, x3); }
            if (y2 > y3) { std::swap(y2, y3); std::swap(x2, x3); }

            auto fastDrawScanLine = [&](short startX, short endX, short y) {
                for (short i = startX; i <= endX; ++i) {
                    this->draw(i, y, pix, col);
                }
            };

            int t1x, t2x, y, minx, maxx, t1xp, t2xp;
            bool changed1 = false;
            bool changed2 = false;
            int signx1, signx2, dx1, dy1, dx2, dy2;
            int e1, e2;

            t1x = t2x = x1; y = y1; // Starting points
            dx1 = x2 - x1; 
            if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
            else { signx1 = 1; }
            dy1 = y2 - y1;

            dx2 = (int)(x3 - x1); if (dx2 < 0) { dx2 = -dx2; signx2 = -1; }
            else signx2 = 1;
            dy2 = (int)(y3 - y1);

            if (dy1 > dx1) {   // swap values
                std::swap(dx1, dy1);
                changed1 = true;
            }
            if (dy2 > dx2) {   // swap values
                std::swap(dy2, dx2);
                changed2 = true;
            }

            e2 = (int)(dx2 >> 1);
            // Flat top, just process the second half
            if (y1 == y2) goto next;
            e1 = (int)(dx1 >> 1);

            for (int i = 0; i < dx1;) {
                t1xp = 0; t2xp = 0;
                if (t1x < t2x) { minx = t1x; maxx = t2x; }
                else { minx = t2x; maxx = t1x; }
                // process first line until y value is about to change
                while (i < dx1) {
                    i++;
                    e1 += dy1;
                    while (e1 >= dx1) {
                        e1 -= dx1;
                        if (changed1) t1xp = signx1;//t1x += signx1;
                        else          goto next1;
                    }
                    if (changed1) break;
                    else t1x += signx1;
                }
                // Move line
            next1:
                // process second line until y value is about to change
                while (1) {
                    e2 += dy2;
                    while (e2 >= dx2) {
                        e2 -= dx2;
                        if (changed2) t2xp = signx2;//t2x += signx2;
                        else          goto next2;
                    }
                    if (changed2)     break;
                    else              t2x += signx2;
                }
            next2:
                if (minx > t1x) minx = t1x; if (minx > t2x) minx = t2x;
                if (maxx < t1x) maxx = t1x; if (maxx < t2x) maxx = t2x;
                fastDrawScanLine(minx, maxx, y);    // Draw line from min to max points found on the y
                                                   // Now increase y
                if (!changed1) t1x += signx1;
                t1x += t1xp;
                if (!changed2) t2x += signx2;
                t2x += t2xp;
                y += 1;
                if (y == y2) break;

            }
        next:
            // Second half
            dx1 = (int)(x3 - x2); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
            else signx1 = 1;
            dy1 = (int)(y3 - y2);
            t1x = x2;

            if (dy1 > dx1) {   // swap values
                std::swap(dy1, dx1);
                changed1 = true;
            }
            else changed1 = false;

            e1 = (int)(dx1 >> 1);

            for (int i = 0; i <= dx1; i++) {
                t1xp = 0; t2xp = 0;
                if (t1x < t2x) { minx = t1x; maxx = t2x; }
                else { minx = t2x; maxx = t1x; }
                // process first line until y value is about to change
                while (i < dx1) {
                    e1 += dy1;
                    while (e1 >= dx1) {
                        e1 -= dx1;
                        if (changed1) { t1xp = signx1; break; }//t1x += signx1;
                        else          goto next3;
                    }
                    if (changed1) break;
                    else   	   	  t1x += signx1;
                    if (i < dx1) i++;
                }
            next3:
                // process second line until y value is about to change
                while (t2x != x3) {
                    e2 += dy2;
                    while (e2 >= dx2) {
                        e2 -= dx2;
                        if (changed2) t2xp = signx2;
                        else          goto next4;
                    }
                    if (changed2)     break;
                    else              t2x += signx2;
                }
            next4:

                if (minx > t1x) minx = t1x; if (minx > t2x) minx = t2x;
                if (maxx < t1x) maxx = t1x; if (maxx < t2x) maxx = t2x;
                fastDrawScanLine(minx, maxx, y);
                if (!changed1) t1x += signx1;
                t1x += t1xp;
                if (!changed2) t2x += signx2;
                t2x += t2xp;
                y += 1;
                if (y > y3) return;
            }
        }

        // Bresenham’s circle drawing algorithm
        // https://www.geeksforgeeks.org/bresenhams-circle-drawing-algorithm/
        void drawCircle
        ( short xc
        , short yc
        , short radius
        , basePixelType pix = Pixel::Solid
        , baseColorType col = Color::FG_White
        ) {
            int x = 0;
            int y = radius;
            int p = 3 - 2 * radius;
            if (radius > 0) {
                while (y >= x) // only formulate 1/8 of circle
                {
                    draw(xc - x, yc - y, pix, col); 
                    draw(xc - y, yc - x, pix, col); 
                    draw(xc + y, yc - x, pix, col); 
                    draw(xc + x, yc - y, pix, col); 
                    draw(xc - x, yc + y, pix, col); 
                    draw(xc - y, yc + x, pix, col); 
                    draw(xc + y, yc + x, pix, col); 
                    draw(xc + x, yc + y, pix, col); 
                    if (p < 0) {
                        p += 4 * (x++) + 6;
                    }
                    else {
                        p += 4 * ((x++) - (y--)) + 10;
                    }
                }
            }
        }

        void fillCircle
        ( short xc
        , short yc
        , short radius
        , basePixelType pix = Pixel::Solid
        , baseColorType col = Color::FG_White
        ) {
            int x = 0;
            int y = radius;
            int p = 3 - 2 * radius;
            if (radius > 0) {
                auto fastDrawScanLine = [&](short startX, short endX, short y) {
                    for (short i = startX; i <= endX; ++i) {
                        this->draw(i, y, pix, col);
                    }
                };
                while (y >= x) // only formulate 1/8 of circle
                {
                    fastDrawScanLine(xc - x, xc + x, yc - y);
                    fastDrawScanLine(xc - y, xc + y, yc - x);
                    fastDrawScanLine(xc - x, xc + x, yc + y);
                    fastDrawScanLine(xc - y, xc + y, yc + x);
                    if (p < 0) {
                        p += 4 * (x++) + 6;
                    }
                    else {
                        p += 4 * ((x++) - (y--)) + 10;
                    }
                }
            }
        }

        void drawSprite
        ( short x
        , short y
        , Sprite const &sprite
        ) {
            for (short i = 0; i < sprite.getWidth(); ++i) {
                for (short j = 0; j < sprite.getHeight(); ++j) {
                    if (sprite.getPixel(i, j) != L' ') {
                        draw(x + i, y + j, sprite.getPixel(i, j), sprite.getColor(i, j));
                    }
                }
            }
        }

        void drawSpritePartial
        ( short xScreen
        , short yScreen
        , Sprite const &sprite
        , short xBegin
        , short yBegin
        , short width
        , short height
        ) {
            for (short i = 0; i < width; ++i) {
                for (short j = 0; j < height; ++j) {
                    if (sprite.getPixel(i + xBegin, j + yBegin) != L' ') {
                        draw(xScreen + i, yScreen + j, sprite.getPixel(i + xBegin, j + yBegin), sprite.getColor(i + xBegin, j + yBegin));
                    }
                }
            }
        }

        // Changes x and y coords so they fit to screen
        void clipCoords
        ( short &x
        , short &y
        ) {
            if (x < 0) {
                x = 0;
            }
            else if (x >= m_screenWidth) {
                x = m_screenWidth;
            }
            if (y < 0) {
                y = 0;
            }
            else if (y >= m_screenHeight) {
                y = m_screenHeight;
            }
        }

        short getScreenWidth
        (
        ) const {
            return m_screenWidth;
        }

        short getScreenHeight
        (
        ) const {
            return m_screenHeight;
        }

        void setAppName
        ( std::wstring const &appName
        ) {
            m_appName = appName;
        }

        std::wstring getAppName
        (
        ) const {
            return m_appName;
        }

        // Must be overriden by user class
        // Used to initialize user resources
        virtual bool userCreate
        (
        ) = 0; 

        // Must be overriden by user class
        // Used to update game values and render image inside screen buffer
        // Elapsed time is time between two calls to this function
        virtual bool userUpdate
        ( float elapsedTime
        ) = 0;

        // Can be overriden to clean resources initialized by user class
        virtual bool userDestroy
        (
        ) { 
            return true; 
        }

    protected:

        void gameThread
        (
        ) {
            if (!userCreate()) {
                m_atomActive = false;
            }
            auto tp1 = std::chrono::steady_clock::now();
            auto tp2 = std::chrono::steady_clock::now();
            while (m_atomActive) {
                while (m_atomActive) {
                    // Timing
                    tp2 = std::chrono::steady_clock::now();
                    std::chrono::duration<float> cycleDuration = tp2 - tp1;
                    tp1 = tp2;
                    float elapsedTime = cycleDuration.count();

                    // Keyboard input
                    for (int i = 0; i < numKeyboardKeys; ++i) {
                        m_keyNewState[i] = GetAsyncKeyState(i);
                        m_keys[i].isPressed = false;
                        m_keys[i].isReleased = false;
                        if (m_keyNewState[i] != m_keyOldState[i])
                        {
                            if (m_keyNewState[i] & 0x8000)
                            {
                                m_keys[i].isPressed = !m_keys[i].isHeld;
                                m_keys[i].isHeld = true;
                            }
                            else
                            {
                                m_keys[i].isReleased = true;
                                m_keys[i].isHeld = false;
                            }
                            m_keyOldState[i] = m_keyNewState[i];
                        }
                    }

                    // Mouse input - check window events
                    INPUT_RECORD inBuf[32] = { 0 };
                    DWORD events = 0;
                    GetNumberOfConsoleInputEvents(m_inputHandler, &events);
                    if (events > 0) {
                        ReadConsoleInput(m_inputHandler, inBuf, events, &events);
                    }

                    // Handle events - only mouse clicks and movement
                    for (DWORD i = 0; i < events; ++i) {
                        switch (inBuf[i].EventType) {
                        case FOCUS_EVENT:
                            m_consoleInFocus = inBuf[i].Event.FocusEvent.bSetFocus;
                            break;

                        case MOUSE_EVENT:
                            switch (inBuf[i].Event.MouseEvent.dwEventFlags) {
                            case MOUSE_MOVED:
                                m_mousePosX = inBuf[i].Event.MouseEvent.dwMousePosition.X;
                                m_mousePosY = inBuf[i].Event.MouseEvent.dwMousePosition.Y;
                                break;

                            case 0:
                                for (int m = 0; m < numMouseButtons; ++m)
                                    m_mouseNewState[m] = (inBuf[i].Event.MouseEvent.dwButtonState & (1 << m)) > 0;
                                break;

                            default:
                                break;
                            }
                            break;

                        default:
                            break; // Don't care
                        }
                    }

                    for (int i = 0; i < numMouseButtons; ++i) {
                        m_mouse[i].isPressed = false;
                        m_mouse[i].isReleased = false;
                        if (m_mouseNewState[i] != m_mouseOldState[i]) {
                            if (m_mouseNewState[i]) {
                                m_mouse[i].isPressed = true;
                                m_mouse[i].isHeld = true;
                            }
                            else {
                                m_mouse[i].isReleased = true;
                                m_mouse[i].isHeld = false;
                            }
                        }
                        m_mouseOldState[i] = m_mouseNewState[i];
                    }

                    // Handle frame update by user
                    if (!userUpdate(elapsedTime)) {
                        m_atomActive = false;
                    }

                    // Title update
                    wchar_t buf[256];
                    swprintf_s(buf, 256, L"%ls - FPS: %3.2f", m_appName.c_str(), 1.0f / elapsedTime);
                    SetConsoleTitle(buf);
                    WriteConsoleOutput(m_screenHandler, m_screenBuf.get(), { m_screenWidth, m_screenHeight }, { 0,0 }, &m_rectWindow);
                }
                if (userDestroy()) { // User allowed to finish
                    SetConsoleActiveScreenBuffer(m_originalScreenHandler);
                    m_gameFinished.notify_one();
                }
                else {
                    // User denied destroy for some reason, continue running
                    m_atomActive = true;
                }
            }
        }

        // Prints out error message
        void reportError
        ( std::wstring const &errorMsg
        ) {
            SetConsoleActiveScreenBuffer(m_originalScreenHandler);
            wprintf(L"\n\rERROR: %ls\n", errorMsg.c_str());
        }

        // Screen size
        short m_screenWidth = 0;
        short m_screenHeight = 0;
 
        HANDLE m_screenHandler;
        HANDLE m_inputHandler;
        HANDLE m_originalScreenHandler;
        CONSOLE_SCREEN_BUFFER_INFO m_OriginalScreenInfo;

        // RAII array that stores characters and their colors
        // and is used to create image on screen
        // All that you draw on screen goes here
        std::unique_ptr<CHAR_INFO[]> m_screenBuf;

        SMALL_RECT m_rectWindow;

        // Application name shown in title
        std::wstring m_appName;

        // Keyboard key state
        struct keyState {
            bool isPressed;
            bool isReleased;
            bool isHeld;
        };

        static inline int const numKeyboardKeys = 256;
        static inline int const numMouseButtons = 5;

        keyState m_keys[numKeyboardKeys]          = { 0 };
        short    m_keyOldState[numKeyboardKeys]   = { 0 };
        short    m_keyNewState[numKeyboardKeys]   = { 0 };

        keyState m_mouse[numMouseButtons]         = { 0 };
        short    m_mousePosX                      =   0;
        short    m_mousePosY                      =   0;
        bool     m_mouseOldState[numMouseButtons] = { 0 };
        bool     m_mouseNewState[numMouseButtons] = { 0 };

        bool     m_consoleInFocus = true;

    public:

        keyState getKey
        ( int keyID
        ) const { 
            return m_keys[keyID]; 
        }

        short getMousePosX
        (
        ) const { 
            return m_mousePosX; 
        }

        short getMousePosY
        (
        ) const { 
            return m_mousePosY; 
        }

        keyState getMouse
        ( int buttonID
        ) const { 
            return m_mouse[buttonID]; 
        }

        bool isFocused
        (
        ) const {
            return m_consoleInFocus;
        }

    protected:

        // These need to be static because of the OnDestroy call the OS may make. The OS
        // spawns a special thread just for that
        inline static std::atomic_bool m_atomActive{ false };
        inline static std::condition_variable m_gameFinished;
        inline static std::mutex m_muxGame;
    };

}; // CGE
# Console-Game-Engine
My rewrite of olcGameEngine by Javidx9: https://github.com/OneLoneCoder  
These files are made only for myself for educational purposes.

Original files can be found here: https://github.com/OneLoneCoder/videos

1) Console Game Engine.h is a header only library for code written in C++17 for OS Windows  
Usage:  
```c++
#include "Console Game Engine.h"

// Make sure to enable Unicode!

class GameEngine : public CGE::BaseGameEngine {
public:
    // MUST be overriden!
    // Used to initialize user resources
    bool userCreate() override { ... initializes ... return true; } // true if all OK, else return false to stop execution
    
    // MUST be overriden!
    // Used to update game values and render image inside screen buffer(m_screenBuf)
    // Elapsed time is time between two calls to this function - can be used for synchronization
    bool userUpdate(float elapsedTime) override { ... return true; } // true if all OK, else return false to stop execution
    
    // Can be overriden to clean resources initialized by user class
    bool userDestroy() override { ... cleanup ... return true; } // true if all OK(will stop game loop)
                                                                 // else return false to continue execution of game loop
};

int main() {
GameEngine ge;
if(ge.createConsole(screenWidth, screenHeight, fontWidth, fontHeight)) // If successfull returns true
    ge.start();
return 0;
}
```  
Game engine can draw sprites that are written inside binary file with special format that consists of 4 parts:
```
(short)width (short)height (uint16_t[width*height])pixelType (uint16_t[width*height])colorType
```  

#! All files below use Console Game Engine
  
# Console FPS
Console FPS is a prototype of a game similar to Wolfenstein 3D.   
It uses Console Game Engine to create pseudo-3D image using raycasting method.  
Game Engine renders 3D walls and 2D objects.  
```
Controls:

    WASD - Walking    
    
    Q/E - Strafe left/right  
    
    Space - Shoot
```  
    
```
Installation:

    Download "Console Game Engine.hpp", "Console FPS.cpp" and "Console FPS Sprites/" folder  
    
    Put downloaded files in one folder and compile  
    
    For best FPS set compiler optimization to maximum (and set configuration to Release in VS)
    You can also try changing parameters of createConsole function in main function at "Console FPS.cpp"
```
Original code used as an example:  
    https://github.com/OneLoneCoder/videos
    
Videos used as tutorial for learning:  
    Part 1: https://youtu.be/xW8skO7MFYw  
    Part 2: https://youtu.be/HEb2akswCcw

# 3D Graphics Engine
File 1  
First file contains program that renders rotating 3D mesh of a cube  
Program uses matrix multiplication to rotate vertices around axises and to project 3D image to 2D screen  

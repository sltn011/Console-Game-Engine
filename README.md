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

# ! All files below use Console Game Engine
  
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

    Download "Console FPS" folder  
    
    In command line:
    ...\Console FPS> cmake CMakeLists.txt
    
    After building project if you are using VS
    Open project folder
    Open .sln file
    After VS loaded your project select App
        Turn Unicode ON
        Set it as StartUp Project
        
    You can now launch application
    
    For best FPS set compiler optimization to maximum (and set configuration to Release in VS)
    You can also try changing parameters of createConsole function in main function at "Console FPS.cpp"
```  
![Console FPS preview](https://github.com/sltn011/Console-Game-Engine/blob/master/ReadmeImages/consoleFPS1.png)
  
Original code used as an example:  
    https://github.com/OneLoneCoder/videos
    
Videos used as tutorial for learning:  
    Part 1: https://youtu.be/xW8skO7MFYw  
    Part 2: https://youtu.be/HEb2akswCcw

# 3D Graphics Engine
File contains program that renders rotating 3D mesh  
Program can load meshes from triangulated .obj file   
Project contains 3DTools library with classes used for rendering  
Project can be built with CMake. For this:  
```
In command line:
    ...\3D Graphics Engine> cmake CMakeLists.txt
    
After building project if you are using VS
    Open project folder
    Open .sln file
    After VS loaded your project select App
        Turn Unicode ON
        Set it as StartUp Project
        
You can now launch application
    
```
Program uses matrix multiplication to rotate vertices around axises and to project 3D image to 2D screen  
Sides that are visible to camera are calculated using dot product  
Model is illuminated by "light source", sides are shaded according to amount of "light" they catch  
  
1) Model renderer renders model from .obj file and shows it on screen  
Object is rotated using matrixes  
![Model Renderer preview](https://github.com/sltn011/Console-Game-Engine/blob/master/ReadmeImages/3DGraphics_ModelRenderer.png)  
  
2) Camera example renders big mountains objects and allows user to fly around  
Code shows how to create cameras for 3D worlds, how to render objects around it  
and how to handle camera movement  
Code also shows simple clipping algorithm to greatly boost perfomance by discarding  
triangles that user doesn't see in FoV  
```
Camera controls:
    WASD - moving in XZ plane
    Space/LeftShift - move up and down in Y axis
```
![Camera Example preview](https://github.com/sltn011/Console-Game-Engine/blob/master/ReadmeImages/3DGraphics_CameraExample.png)  
  
3) Texturing example loads cube mesh from .obj file and it's texture from .spr file  
and renders cube with texture applied  
This example adds support of texture vertexes for meshes and shows how to render  
textures on polygons  
This example also provides free camera for user with same controls as Camera example file  
and uses depth buffer instead of sorting polygons based on distance to camera  
![Texturing Example preview](https://github.com/sltn011/Console-Game-Engine/blob/master/ReadmeImages/3DGraphics_TexturingExample.png)  
  
Original code used as an example:  
    https://github.com/OneLoneCoder/videos
    
Videos used as tutorial for learning:  
    Part 1: https://youtu.be/ih20l3pJoeU  
    Part 2: https://youtu.be/XgMWc6LumG4  
    Part 3: https://youtu.be/HXSuNxpCzdM  
    Part 4: https://youtu.be/nBzCS-Y0FcY  
  
# Maze Generator  
Programm generates random maze using Recursive Backtracker algorithm, std::stack structure and Console Game Engine to render  
process of maze creation step by step  
Algorithm uses data from current maze cell(which is on top of the stack), gets random not yet visited neighbour cell and creates path  
between two. Then neighbour is put on stack and becomes current. If all neighbours were already visited - backtrack by popping top cell from stack  
After each step maze is rendered to create animation  
Algorithm works until all cells are visited  
1) Maze Generator creating new maze  
![MazeGeneratorWorking](https://github.com/sltn011/Console-Game-Engine/blob/master/ReadmeImages/MazeGeneratorWorking.png)  
  
2) One of mazes created by Generator  
![MazeGeneratorResult](https://github.com/sltn011/Console-Game-Engine/blob/master/ReadmeImages/MazeGeneratorResult.png)  
  
Video used as tutorial for learning:  
    https://youtu.be/Y37-gB83HKE  
  
# Perlin Noise Generator  
Programm generates random seed and uses it to produce 1D or 2D Perlin-like noise  
Perlin noise is created by summing waves with different amplitudes and wave lenghts  
Generated noise is than rendered in the console using Console Game Engine  
```
Controls:
    1, 2 - 1D or 2D mode
    Space - changing number of octaves used in noise generation
    Z - Creating new noise seed
    I - Switching interpoltion methods (Linear and Cosine)
    Up/Down - Increase/Decrease value used to decrease amplitude of each consecutive octave  
```  
1) 1D Perlin noise example  
![1D Perlin Noise](https://github.com/sltn011/Console-Game-Engine/blob/master/ReadmeImages/PerlinNoiseGenerator1D.png)  
  
2) 2D Perlin noise example(different values are painted differently)  
![2D Perlin Noise](https://github.com/sltn011/Console-Game-Engine/blob/master/ReadmeImages/PerlinNoiseGenerator2D.png)  
  
Video used as tutorial for learning:  
    https://youtu.be/6-0UaeJBumA

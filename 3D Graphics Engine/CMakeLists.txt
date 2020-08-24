# Minimum CMake Version
cmake_minimum_required(VERSION 3.10.2)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Project Name
project("3D Graphics Engine")

# Need Threads for Console Game Engine to work
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Creating static library with tools for rendering 3D Image
add_library(3DTools STATIC src/Vec3D.cpp src/Triangle.cpp src/Matrix4x4.cpp src/Mesh.cpp)

# Folder with header files
target_include_directories(3DTools PUBLIC include)

# What will be compiled as an executable
add_executable(ModelRenderer "example/Model Renderer.cpp")
add_executable(CameraExample "example/Camera Example.cpp")

# Linking libraries with executable
target_link_libraries(ModelRenderer 3DTools Threads::Threads)
target_link_libraries(CameraExample 3DTools Threads::Threads)

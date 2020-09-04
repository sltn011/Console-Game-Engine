# Minimum CMake Version
cmake_minimum_required(VERSION 3.10.2)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Project Name
project("Console FPS")

# Set Unicode on
add_definitions(-D_UNICODE -DUNICODE)

# Need Threads for Console Game Engine to work
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Where header files are
include_directories(include)

# What will be compiled as an executable
add_executable(App "Console FPS.cpp")

# Linking libraries with executable
target_link_libraries(App Threads::Threads)
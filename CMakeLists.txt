cmake_minimum_required(VERSION 3.13)

# set the project name and version
project(MyRayTracer)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# specify 
set(CMAKE_CXX_FLAGS "-O3 -lm -fopenmp")


add_executable(MyRayTracer Main.cpp)

# Link libs
target_link_libraries(MyRayTracer png)

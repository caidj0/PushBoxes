cmake_minimum_required(VERSION 3.5.0)
project(PushBoxes VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

add_subdirectory(Blocks)

add_executable(PushBoxes main.cpp Map.cpp)
target_link_libraries(PushBoxes PUBLIC Blocks)
cmake_minimum_required(VERSION 3.5.0)
project(PushBoxes VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

find_library(ncurses_path NAME ncurses REQUIRED)

add_executable(PushBoxes main.cpp Map.cpp Block.cpp Blocks.cpp Screen.cpp MapUtils.cpp)
target_link_libraries(PushBoxes PUBLIC ${ncurses_path})
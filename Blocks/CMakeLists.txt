add_library(Blocks Block.cpp Box.cpp Player.cpp Wall.cpp)

target_include_directories(Blocks INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
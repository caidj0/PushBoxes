#pragma once

#include <string_view>

namespace PushBoxes {
enum Direction{
    UP, DOWN, LEFT, RIGHT
};

struct Block {
    std::string_view name;
    char viewChar;
    bool isReplaceable;
    bool isMoveable;
    Block(const char* name, char viewChar, bool isReplaceable, bool isMoveable);
};

enum POIType {
    NEEDBLOCK, NEEDPLAYER
};

}
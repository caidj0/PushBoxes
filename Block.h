#pragma once

#include <functional>
#include <string>

namespace PushBoxes {
enum Direction { NODIRECTION, UP, DOWN, LEFT, RIGHT };

Direction inverseDirection(Direction direction);

class Block;

class BlockType {
   public:
    const std::string name;
    const std::function<char(const Block&)> viewChar;
    const bool isReplaceable;
    const bool isMoveable;
    const bool isAccessible;
    bool operator==(const BlockType& Right) const;
    bool operator!=(const BlockType& Right) const;

    BlockType(std::string name, char viewChar, bool isReplaceable,
              bool isMoveable, bool isAccessible);
    BlockType(std::string name,
              std::function<char(const Block& block)> viewChar,
              bool isReplaceable, bool isMoveable, bool isAccessible);
    BlockType(const BlockType& Right) = delete;
};

class Block {
   private:
    const BlockType* typePtr;
   public:
    std::string inner_map_id;
    
    Direction moving_trend;

    Block(const BlockType& type);
    Block(const BlockType& type, std::string inner_map_id);
    char getViewChar() const;
    const BlockType& getBlockType() const;
    void setBlockType(const BlockType& type);
};

enum POIType { NEEDBLOCK, NEEDPLAYER };

}  // namespace PushBoxes
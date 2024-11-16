#pragma once

#include <cstddef>
#include <functional>
#include <string>

namespace PushBoxes {
enum Direction { NODIRECTION, UP, DOWN, LEFT, RIGHT };

Direction inverseDirection(Direction direction);

class Block;

struct VisualMode {
    bool isHighlight;
};

class BlockType {
   public:
    const std::string name;
    const std::function<char(const Block&, size_t)> viewChar;
    const bool isReplaceable;
    const bool isMoveable;
    const bool isAccessible;
    const VisualMode visualMode;
    bool operator==(const BlockType& Right) const;
    bool operator!=(const BlockType& Right) const;

    BlockType(std::string name, char viewChar, bool isReplaceable,
              bool isMoveable, bool isAccessible, VisualMode visualMode);
    BlockType(std::string name,
              std::function<char(const Block& block, size_t time)> viewChar,
              bool isReplaceable, bool isMoveable, bool isAccessible,
              VisualMode visualMode);
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
    VisualMode getVisualMode() const;
    char getViewChar(size_t time) const;
    const BlockType& getBlockType() const;
    void setBlockType(const BlockType& type);
};

enum POIType { NEEDBLOCK, NEEDPLAYER };

}  // namespace PushBoxes
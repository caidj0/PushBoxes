#pragma once

#include <cstddef>
#include <functional>
#include <string_view>

namespace PushBoxes {
enum Direction { UP, DOWN, LEFT, RIGHT };

class Block;

class BlockType {
   public:
    const std::string_view name;
    const std::function<char(const Block&)> viewChar;
    const bool isReplaceable;
    const bool isMoveable;
    const bool isAccessible;
    bool operator==(const BlockType& Right) const;
    bool operator!=(const BlockType& Right) const;

    BlockType(std::string_view name, char viewChar, bool isReplaceable,
              bool isMoveable, bool isAccessible);
    BlockType(std::string_view name,
              std::function<char(const Block& block)> viewChar,
              bool isReplaceable, bool isMoveable, bool isAccessible);
    BlockType(const BlockType& Right) = delete;
};

class Block {
   private:
    const BlockType* typePtr;
   public:
    Block(const BlockType& type, size_t locate_map_id);
    Block(const BlockType& type, size_t locate_map_id, size_t inner_map_id);
    size_t locate_map_id;
    size_t inner_map_id;
    char getViewChar() const;
    const BlockType& getBlockType() const;
    void setBlockType(const BlockType& type);
};

enum POIType { NEEDBLOCK, NEEDPLAYER };

}  // namespace PushBoxes
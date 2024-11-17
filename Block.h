#pragma once

#include <cstddef>
#include <functional>
#include <string>

namespace PushBoxes {
enum Direction { NODIRECTION, UP, DOWN, LEFT, RIGHT };

Direction inverseDirection(Direction direction);
Direction flipDirection(Direction direction);

class Block;

struct VisualMode {
    bool isHighlight;
    bool canBeCovered;
};

class BlockType {
   public:
    const std::string name;
    const std::function<char(const Block& block, size_t time)> viewChar;
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

    // 0 ~ 3 代表逆时针旋转了 0, 90, 180, 270 度.
    int rotate;

    // 是否左右翻转
    bool isFliped;

    Direction moving_trend;

    // 0 代表不能包含玩家, 1 代表可以包含玩家, 2 代表已经包含了玩家.
    int playerStatus;
    size_t playerIndex;

    Block(const BlockType& type);
    Block(const BlockType& type, std::string inner_map_id);
    VisualMode getVisualMode() const;
    char getViewChar(size_t time) const;
    const BlockType& getBlockType() const;
    void setBlockType(const BlockType& type);
};

enum POIType { POINONE, NEEDBLOCK, NEEDPLAYER };

bool meetPOIDemand(POIType type, const Block& block);

}  // namespace PushBoxes
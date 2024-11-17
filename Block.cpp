#include "Block.h"

#include <cstddef>
#include <string>

#include "Blocks.h"

namespace PushBoxes {
Direction inverseDirection(Direction direction) {
    if (direction == UP) return DOWN;
    if (direction == DOWN) return UP;
    if (direction == LEFT) return RIGHT;
    return LEFT;
}

Direction flipDirection(Direction direction) {
    if (direction == LEFT) return RIGHT;
    if (direction == RIGHT) return LEFT;
    return direction;
}

BlockType::BlockType(std::string name, char viewChar, bool isReplaceable,
                     bool isMoveable, bool isAccessible, VisualMode visualMode)
    : name(name),
      viewChar([viewChar](const Block& block, size_t time) {
          if (block.playerStatus == 2 && block.getBlockType() == BOX_BLOCK)
              return 'p';
          if (block.playerStatus && ((time / 30 & 3) != 3))
              return block.playerStatus == 2 ? 'p' : ':';
          return viewChar;
      }),
      isReplaceable(isReplaceable),
      isMoveable(isMoveable),
      isAccessible(isAccessible),
      visualMode(visualMode){};

BlockType::BlockType(
    std::string name,
    std::function<char(const Block& block, size_t time)> viewChar,
    bool isReplaceable, bool isMoveable, bool isAccessible,
    VisualMode visualMode)
    : name(name),
      viewChar([viewChar](const Block& block, size_t time) {
          if (block.playerStatus && (((time / 30) & 3) != 3))
              return block.playerStatus == 2 ? 'p' : ':';
          return viewChar(block, time);
      }),
      isReplaceable(isReplaceable),
      isMoveable(isMoveable),
      isAccessible(isAccessible),
      visualMode(visualMode){};

bool BlockType::operator==(const BlockType& right) const {
    return right.name == name;
}
bool BlockType::operator!=(const BlockType& right) const {
    return right.name != name;
}

Block::Block(const BlockType& type)
    : typePtr(&type),
      inner_map_id(),
      moving_trend(NODIRECTION),
      rotate(0),
      isFliped(0),
      playerStatus(0),
      playerIndex(0) {}

Block::Block(const BlockType& type, std::string inner_map_id)
    : typePtr(&type),
      inner_map_id(inner_map_id),
      moving_trend(NODIRECTION),
      rotate(0),
      isFliped(0),
      playerIndex(0),
      playerStatus(0) {}

VisualMode Block::getVisualMode() const { return typePtr->visualMode; }

char Block::getViewChar(size_t time) const {
    return typePtr->viewChar(*this, time);
}

const BlockType& Block::getBlockType() const { return *typePtr; }

void Block::setBlockType(const BlockType& type) { typePtr = &type; }

bool meetPOIDemand(POIType type, const Block& block) {
    return (type == NEEDBLOCK && block.getBlockType().isMoveable && block.playerStatus != 2) ||
           (type == NEEDPLAYER && block.playerStatus == 2);
}

}  // namespace PushBoxes
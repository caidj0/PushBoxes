#include "Block.h"

#include <string>

namespace PushBoxes {
Direction inverseDirection(Direction direction) {
    if(direction == UP) return DOWN;
    if(direction == DOWN) return UP;
    if(direction == LEFT) return RIGHT;
    return LEFT;
}

BlockType::BlockType(std::string name, char viewChar, bool isReplaceable,
                     bool isMoveable, bool isAccessible)
    : name(name),
      viewChar([viewChar](const Block& block) { return viewChar; }),
      isReplaceable(isReplaceable),
      isMoveable(isMoveable),
      isAccessible(isAccessible){};

BlockType::BlockType(std::string name,
                     std::function<char(const Block& block)> viewChar,
                     bool isReplaceable, bool isMoveable, bool isAccessible)
    : name(name),
      viewChar(viewChar),
      isReplaceable(isReplaceable),
      isMoveable(isMoveable),
      isAccessible(isAccessible){};

bool BlockType::operator==(const BlockType& right) const {
    return right.name == name;
}
bool BlockType::operator!=(const BlockType& right) const {
    return right.name != name;
}

Block::Block(const BlockType& type)
    : typePtr(&type), inner_map_id(), moving_trend(NODIRECTION) {}

Block::Block(const BlockType& type, std::string inner_map_id)
    : typePtr(&type), inner_map_id(inner_map_id), moving_trend(NODIRECTION) {}

char Block::getViewChar() const { return typePtr->viewChar(*this); }

const BlockType& Block::getBlockType() const { return *typePtr; }

void Block::setBlockType(const BlockType& type) { typePtr = &type; }
}  // namespace PushBoxes
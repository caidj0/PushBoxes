#include "Block.h"

#include <cstddef>


namespace PushBoxes {
BlockType::BlockType(std::string_view name, char viewChar, bool isReplaceable,
                     bool isMoveable, bool isAccessible)
    : name(name),
      viewChar([viewChar](const Block& block) { return viewChar; }),
      isReplaceable(isReplaceable),
      isMoveable(isMoveable),
      isAccessible(isAccessible){};

BlockType::BlockType(std::string_view name,
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

Block::Block(const BlockType& type, size_t locate_map_id)
    : typePtr(&type), locate_map_id(locate_map_id), inner_map_id(0) {}

Block::Block(const BlockType& type, size_t locate_map_id, size_t inner_map_id)
    : typePtr(&type),
      locate_map_id(locate_map_id),
      inner_map_id(inner_map_id) {}

char Block::getViewChar() const { return typePtr->viewChar(*this); }

const BlockType& Block::getBlockType() const { return *typePtr; }

void Block::setBlockType(const BlockType& type) { typePtr = &type; }
}  // namespace PushBoxes
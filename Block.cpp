#include "Block.h"

#include "Blocks.h"

namespace PushBoxes {
Block::Block(const char* name, char viewChar, bool isReplaceable,
             bool isMoveable, bool isAccessible)
    : name(name),
      viewChar(viewChar),
      isReplaceable(isReplaceable),
      isMoveable(isMoveable),
      isAccessible(isAccessible){};

Block::Block(const Block& Right, size_t inner_map_id)
    : name(Right.name),
      viewChar(Right.viewChar),
      isReplaceable(Right.isReplaceable),
      isMoveable(Right.isMoveable),
      isAccessible(Right.isAccessible),
      inner_map_id(inner_map_id){};

bool Block::operator==(const Block& right) const { return right.name == name; }
bool Block::operator!=(const Block& right) const { return right.name != name; }

char Block::getViewChar() const {
    if (*this != MAP_BLOCK) return viewChar;
    if (inner_map_id < 10)
        return '0' + inner_map_id;
    else
        return 'a' + inner_map_id - 10;
}

}  // namespace PushBoxes
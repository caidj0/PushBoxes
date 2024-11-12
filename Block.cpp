#include "Block.h"

namespace PushBoxes {
Block::Block(const char* name, char viewChar, bool isReplaceable,
             bool isMoveable)
    : name(name),
      viewChar(viewChar),
      isReplaceable(isReplaceable),
      isMoveable(isMoveable) {};

}
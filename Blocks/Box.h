#pragma once
#include "Block.h"

namespace PushBoxes {
class Box : public Block {
   public:
    bool isMoveable() override;
    char blockChar() override;
};
}  // namespace PushBoxes
#pragma once
#include "Block.h"

namespace PushBoxes {
class Wall : public Block {
   public:
    char blockChar() override;
};
}  // namespace PushBoxes
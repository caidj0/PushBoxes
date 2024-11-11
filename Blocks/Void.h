#pragma once
#include "Block.h"

namespace PushBoxes {
class Void: public Block {
    public:
        bool isReplaceable() override;
        char blockChar() override;
};

}
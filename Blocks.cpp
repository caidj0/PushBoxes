#include "Blocks.h"

#include <stdexcept>

namespace PushBoxes {
const BlockType& getBlockByName(std::string name) {
    for (auto x : ALL_BLOCKS)
        if (x->name == name) return *x;
    throw std::out_of_range(
        std::string("Can't find block with name: ").append(name));
}
}  // namespace PushBoxes
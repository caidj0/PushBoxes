#include "Blocks.h"

#include <stdexcept>

namespace PushBoxes {
Block getBlockByName(std::string_view name) {
    for (auto x : BLOCKS)
        if (x.name == name) return x;
    throw std::out_of_range(
        std::string("Can't find block with name: ").append(name));
}
}  // namespace PushBoxes
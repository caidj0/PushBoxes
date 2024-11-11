#pragma once
#include <cstddef>
#include <vector>

#include "Block.h"

namespace PushBoxes {
class Map {
   public:
    std::vector<std::vector<PushBoxes::Block>> blocks;
    Map(size_t row, size_t column);
};

class MapManager {
    public:
    std::vector<Map> maps;
};

}  // namespace PushBoxes
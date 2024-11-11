#include "Map.h"
#include <cstddef>
#include <vector>

#include "Void.h"

namespace PushBoxes {
Map::Map(size_t row, size_t column) {
    blocks.resize(row, std::vector<Block>(column, Void()));
}
}
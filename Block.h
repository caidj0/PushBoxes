#pragma once

#include <cstddef>
#include <string_view>

namespace PushBoxes {
enum Direction { UP, DOWN, LEFT, RIGHT };

class Block {
   private:
    char viewChar;

   public:
    std::string_view name;
    bool isReplaceable;
    bool isMoveable;
    bool isAccessible;
    size_t inner_map_id;
    Block(const char* name, char viewChar, bool isReplaceable, bool isMoveable,
          bool isAccessible);
    Block(const Block& Right, size_t inner_map_id);
    bool operator==(const Block& Right) const;
    bool operator!=(const Block& Right) const;
    char getViewChar() const;
};

enum POIType { NEEDBLOCK, NEEDPLAYER };

}  // namespace PushBoxes
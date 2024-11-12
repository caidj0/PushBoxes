#pragma once
#include <string_view>

#include "Block.h"

namespace PushBoxes {
const Block VOID_BLOCK("void", '.', 1, 0, 0);
const Block WALL_BLOCK("wall", 'W', 0, 0, 0);
const Block BOX_BLOCK("box", 'B', 0, 1, 0);
const Block PLAYER_BLOCK("player", '@', 0, 1, 0);
const Block MAP_BLOCK("map", 0, 0, 1, 1);

const Block BLOCKS[] = {VOID_BLOCK, WALL_BLOCK, BOX_BLOCK, PLAYER_BLOCK, MAP_BLOCK};

Block getBlockByName(std::string_view name);

}  // namespace PushBoxes

#pragma once

#include <string_view>
#include "Block.h"

namespace PushBoxes {
const BlockType VOID_BLOCK("void", '.', 1, 0, 0);
const BlockType WALL_BLOCK("wall", 'W', 0, 0, 0);
const BlockType BOX_BLOCK("box", 'B', 0, 1, 0);
const BlockType PLAYER_BLOCK("player", '@', 0, 1, 0);
const BlockType MAP_BLOCK(
    "map",
    [](const Block& block) {
        return block.inner_map_id < 10 ? block.inner_map_id + '0'
                                       : block.inner_map_id + 'a';
    },
    0, 1, 1);

const BlockType* const ALL_BLOCKS[] = {
    &VOID_BLOCK, &WALL_BLOCK, &BOX_BLOCK, &PLAYER_BLOCK, &MAP_BLOCK,
};

const BlockType& getBlockByName(std::string_view name);

}  // namespace PushBoxes

#pragma once

#include <cstddef>
#include <string>

#include "Block.h"

namespace PushBoxes {
const BlockType VOID_BLOCK("void", '.', 1, 0, 0, {0, 1});
const BlockType WALL_BLOCK("wall", 'W', 0, 0, 0, {0, 0});
const BlockType BOX_BLOCK("box", 'B', 0, 1, 0, {0, 0});
const BlockType MAP_BLOCK("map",
                          [](const Block& block, size_t time) {
                              if (block.isFliped && ((time / 60) & 1))
                                  return '/';
                              return block.inner_map_id[0];
                          },
                          0, 1, 1, {0, 0});
const BlockType CLONE_BLOCK("clone",
                            [](const Block& block, size_t time) {
                                if (block.isFliped && ((time / 60) & 1))
                                    return '/';
                                return block.inner_map_id[0];
                            },
                            0, 1, 1, {1, 0});
const BlockType INF_BLOCK("inf",
                          [](const Block& block, size_t time) {
                            if (block.isFliped && ((time / 30) % 4 == 0))
                                    return '/';
                              if ((time / 30) % 4 == 1)
                                  return 'i';
                              return block.inner_map_id[0];
                          },
                          0, 1, 0, {1, 0});

const BlockType* const ALL_BLOCKS[] = {&VOID_BLOCK, &WALL_BLOCK, &BOX_BLOCK,
                                       &MAP_BLOCK, &CLONE_BLOCK, &INF_BLOCK};

const BlockType& getBlockByName(std::string name);

}  // namespace PushBoxes

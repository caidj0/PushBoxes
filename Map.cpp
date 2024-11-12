#include "Map.h"

#include <cstddef>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <vector>

#include "Block.h"
#include "Blocks.h"

namespace PushBoxes {
Map::Map(size_t row, size_t column)
    : row(row),
      column(column),
      blocks(row, std::vector<PushBoxes::Block>(column, VOID_BLOCK)) {}

Map& MapManager::Shot::findMapById(size_t id) {
    for (auto& x : maps)
        if (x.id == id) return x;
    throw std::out_of_range(
        std::string("Can't find map with id: ").append(std::to_string(id)));
}

Block& MapManager::Shot::getBlockByPos(BlockPosition pos) {
    return pos.map->blocks[pos.x][pos.y];
}

BlockPosition MapManager::Shot::getNearByBlock(BlockPosition pos, Direction md) {
    BlockPosition targetPos = pos;
    if (md == UP) {
        if (targetPos.x > 0) {
            targetPos.x--;
        } else {
            throw std::out_of_range("Pos extends map size.");
        }
    } else if (md == DOWN) {
        if (targetPos.x + 1 < pos.map->row) {
            targetPos.x++;
        } else {
            throw std::out_of_range("Pos extends map size.");
        }
    } else if (md == LEFT) {
        if (targetPos.y > 0) {
            targetPos.y--;
        } else {
            throw std::out_of_range("Pos extends map size.");
        }
    } else {
        if (targetPos.y + 1 < pos.map->column) {
            targetPos.y++;
        } else {
            throw std::out_of_range("Pos extends map size.");
        }
    }

    return targetPos;
}

bool MapManager::_move(BlockPosition &pos, Direction md, bool changePos) {
    BlockPosition targetPos = getNearByBlock(pos, md);
    Block& targetBlock = getBlockByPos(targetPos);
    Block& block = getBlockByPos(pos);
    if (targetBlock.isReplaceable ||
        (targetBlock.isMoveable && _move(targetPos, md, false))) {
        targetBlock = block;
        block = VOID_BLOCK;
        if(changePos)
            pos = targetPos;
        return true;
    }
    return false;
}

Map& MapManager::addNewMap(size_t row, size_t column) {
    return maps.emplace_back(row, column);
}

void MapManager::setPlayerPos(size_t x, size_t y, Map* map) {
    playerPos.x = x;
    playerPos.y = y;
    playerPos.map = map;
    map->blocks[x][y] = PLAYER_BLOCK;
}

void MapManager::addBox(size_t x, size_t y, Map* map) {
    map->blocks[x][y] = BOX_BLOCK;
}

bool MapManager::movePlayer(Direction md) {
    return _move(playerPos, md, true);
}

}  // namespace PushBoxes
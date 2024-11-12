#include "Map.h"

#include <cstddef>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <vector>

#include "Block.h"
#include "Blocks.h"

namespace PushBoxes {
Map::Map(size_t row, size_t column, size_t id)
    : row(row),
      column(column),
      id(id),
      blocks(row, std::vector<PushBoxes::Block>(column, VOID_BLOCK)) {}

Map& MapManager::Shot::getMapById(size_t id) {
    for (auto& x : maps)
        if (x.id == id) return x;
    throw std::out_of_range(
        std::string("Can't find map with id: ").append(std::to_string(id)));
}

const Map& MapManager::Shot::getMapById(size_t id) const {
    for (auto& x : maps)
        if (x.id == id) return x;
    throw std::out_of_range(
        std::string("Can't find map with id: ").append(std::to_string(id)));
}

Block& MapManager::Shot::getBlockByPos(BlockPosition pos) {
    return getMapById(pos.map_id).blocks[pos.x][pos.y];
}

BlockPosition MapManager::Shot::getNearByBlock(BlockPosition pos,
                                               Direction md) {
    BlockPosition targetPos = pos;
    Map& map = getMapById(pos.map_id);
    if (md == UP) {
        if (targetPos.x > 0) {
            targetPos.x--;
        } else {
            throw std::out_of_range("Pos extends map size.");
        }
    } else if (md == DOWN) {
        if (targetPos.x + 1 < map.row) {
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
        if (targetPos.y + 1 < map.column) {
            targetPos.y++;
        } else {
            throw std::out_of_range("Pos extends map size.");
        }
    }

    return targetPos;
}

bool MapManager::Shot::_move(BlockPosition& pos, Direction md, bool changePos) {
    BlockPosition targetPos = getNearByBlock(pos, md);
    Block& targetBlock = getBlockByPos(targetPos);
    Block& block = getBlockByPos(pos);
    if (targetBlock.isReplaceable ||
        (targetBlock.isMoveable && _move(targetPos, md, false))) {
        targetBlock = block;
        block = VOID_BLOCK;
        if (changePos) pos = targetPos;
        return true;
    } else if(targetBlock.isAccessible) {
        
    }
    return false;
}

size_t MapManager::Shot::addNewMap(size_t row, size_t column) {
    return maps.emplace_back(row, column, id_counter++).id;
}

void MapManager::setPlayerPos(BlockPosition pos) {
    _shots.top().playerPos = pos;
    _shots.top().getMapById(pos.map_id).blocks[pos.x][pos.y] = PLAYER_BLOCK;
}

void MapManager::setBlock(BlockPosition pos, Block type) {
    if (type != PLAYER_BLOCK) {
        _shots.top().getMapById(pos.map_id).blocks[pos.x][pos.y] = type;
    }
}

bool MapManager::movePlayer(Direction md) {
    Shot temp_shot = _shots.top();
    bool flag = temp_shot._move(temp_shot.playerPos, md, true);
    if (flag) {
        _push_shot(temp_shot);
        return true;
    }
    return false;
}

MapManager::MapManager() : _shots() { _shots.emplace(); }

const std::vector<Map>& MapManager::getMaps() const {
    return _shots.top().maps;
}

const std::vector<POI>& MapManager::getPois() const {
    return _shots.top().pois;
}
BlockPosition MapManager::getPlayerPos() const {
    return _shots.top().playerPos;
}

size_t MapManager::addNewMap(size_t row, size_t column) {
    return _shots.top().addNewMap(row, column);
}

void MapManager::_push_shot(const Shot& shot) { _shots.push(shot); }

bool MapManager::cancel() {
    if (_shots.size() > 1) {
        _shots.pop();
        return true;
    }
    return false;
}

void MapManager::reset() { _push_shot(_oriShot); }

MapManager::MapManager(const Shot& oriShot) : _shots(), _oriShot(oriShot) {
    _shots.push(oriShot);
}

void MapManager::changeOriShot() { _oriShot = _shots.top(); }

const Map& MapManager::getMapById(size_t id) const {
    return _shots.top().getMapById(id);
}

}  // namespace PushBoxes
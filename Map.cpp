#include "Map.h"

#include <cstddef>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <vector>

#include "Block.h"
#include "Blocks.h"
#include "ShotFile.h"

namespace PushBoxes {
Map::Map(size_t row, size_t column, std::string id)
    : row(row),
      column(column),
      id(id),
      blocks(row, std::vector<PushBoxes::Block>(column, {VOID_BLOCK, id})) {}

Map& MapManager::Shot::getMapById(std::string id) {
    for (auto& x : maps)
        if (x.id == id) return x;
    throw std::out_of_range(std::string("Can't find map with id: ").append(id));
}

const Map& MapManager::Shot::getMapById(std::string id) const {
    for (auto& x : maps)
        if (x.id == id) return x;
    throw std::out_of_range(std::string("Can't find map with id: ").append(id));
}

Block& MapManager::Shot::getBlockByPos(BlockPosition pos) {
    return getMapById(pos.map_id).blocks[pos.x][pos.y];
}

std::string MapManager::Shot::containMapWithVoid(std::string map_id) {
    std::string meta_map_id = "Void_" + map_id;
    addNewMap(7, 7, meta_map_id);
    Map &meta_map = getMapById(meta_map_id);
    for(size_t i = 0;i < 6;i++) {
        meta_map.blocks[0][i].setBlockType(WALL_BLOCK);
        meta_map.blocks[6][i].setBlockType(WALL_BLOCK);
        meta_map.blocks[i][0].setBlockType(WALL_BLOCK);
        meta_map.blocks[i][6].setBlockType(WALL_BLOCK);
    }
    Map &map = getMapById(map_id);
    map.pos = {2,2, meta_map_id};
    map.isInMap = 1;
    meta_map.blocks[2][2].setBlockType(MAP_BLOCK);
    meta_map.blocks[2][2].inner_map_id = map_id;
    return meta_map_id;
}

BlockPosition MapManager::Shot::getBlockOutside(std::string map_id, Direction direction) {
    Map& map = getMapById(map_id);
    if(!map.isInMap)
        containMapWithVoid(map_id);
    return getNearbyBlock(map.pos, direction);
}

BlockPosition MapManager::Shot::getNearbyBlock(BlockPosition pos,
                                               Direction md) {
    BlockPosition targetPos = pos;
    Map& map = getMapById(pos.map_id);
    if (md == UP) {
        if (targetPos.x > 0) {
            targetPos.x--;
        } else {
            targetPos = getBlockOutside(pos.map_id, md);
        }
    } else if (md == DOWN) {
        if (targetPos.x + 1 < map.row) {
            targetPos.x++;
        } else {
            targetPos = getBlockOutside(pos.map_id, md);
        }
    } else if (md == LEFT) {
        if (targetPos.y > 0) {
            targetPos.y--;
        } else {
            targetPos = getBlockOutside(pos.map_id, md);
        }
    } else {
        if (targetPos.y + 1 < map.column) {
            targetPos.y++;
        } else {
            targetPos = getBlockOutside(pos.map_id, md);
        }
    }

    return targetPos;
}

BlockPosition MapManager::Shot::getAccessPosition(Block* targetBlock,
                                                  Direction direction) {
    Map& map = getMapById(targetBlock->inner_map_id);
    BlockPosition pos;
    pos.map_id = targetBlock->inner_map_id;
    if (direction == DOWN) {
        pos.x = 0;
        pos.y = map.column / 2;
    } else if (direction == UP) {
        pos.x = map.row - 1;
        pos.y = map.column / 2;
    } else if (direction == RIGHT) {
        pos.x = map.row / 2;
        pos.y = 0;
    } else {
        pos.x = map.row / 2;
        pos.y = map.column - 1;
    }
    return pos;
}


bool MapManager::Shot::_move(BlockPosition& pos, Direction md) {
    BlockPosition targetPos = getNearbyBlock(pos, md);
    Block& block = getBlockByPos(pos);
    Block* targetBlock = &getBlockByPos(targetPos);
    while (targetBlock) {
        if (targetBlock->getBlockType().isReplaceable ||
            (targetBlock->getBlockType().isMoveable &&
             _move(targetPos, md))) {
            *targetBlock = block;
            if(block.getBlockType() == PLAYER_BLOCK) {
                playerPos = targetPos;
            } else if(block.getBlockType() == MAP_BLOCK) {
                getMapById(block.inner_map_id).pos = targetPos;
            }
            block.setBlockType(VOID_BLOCK);
            return true;
        } else if (targetBlock->getBlockType().isAccessible) {
            targetPos = getAccessPosition(targetBlock, md);
            targetBlock = &getBlockByPos(targetPos);
        } else {
            targetBlock = nullptr;
        }
    }
    return false;
}

std::string MapManager::Shot::addNewMap(size_t row, size_t column) {
    while (!addNewMap(row, column, std::to_string(id_counter))) {
        id_counter++;
    }
    return getMapById(std::to_string(id_counter)).id;
}

bool MapManager::Shot::addNewMap(size_t row, size_t column, std::string id) {
    for (const Map& x : maps) {
        if (x.id == id) return false;
    }
    maps.emplace_back(row, column, id);
    return true;
}

void MapManager::setPlayerPos(BlockPosition pos) {
    _shots.top().playerPos = pos;
    _shots.top()
        .getMapById(pos.map_id)
        .blocks[pos.x][pos.y]
        .setBlockType(PLAYER_BLOCK);
}

void MapManager::setBlock(BlockPosition pos, Block block) {
    if (block.getBlockType() != PLAYER_BLOCK) {
        _shots.top().getMapById(pos.map_id).blocks[pos.x][pos.y] = block;
    }
}

void MapManager::setBlock(BlockPosition pos, const BlockType& blockType) {
    if (blockType != PLAYER_BLOCK && blockType != MAP_BLOCK) {
        _shots.top()
            .getMapById(pos.map_id)
            .blocks[pos.x][pos.y]
            .setBlockType(blockType);
    }
}

void MapManager::setMapBlockPos(BlockPosition pos, std::string map_id) {
    Map& map = getMapById(map_id);
    if (map.isInMap) {
        setBlock(map.pos, VOID_BLOCK);
    }
    map.pos = pos;
    map.isInMap = 1;
    setBlock(pos, PushBoxes::Block(PushBoxes::MAP_BLOCK, pos.map_id, map_id));
}

bool MapManager::movePlayer(Direction md) {
    Shot temp_shot = _shots.top();
    bool flag = temp_shot._move(temp_shot.playerPos, md);
    if (flag) {
        _push_shot(temp_shot);
        return true;
    }
    return false;
}

MapManager::MapManager() : _shots() { _shots.emplace(); }

MapManager::MapManager(std::string shotPath) : _shots() {
    _oriShot = ShotFile::read(shotPath);
    _shots.push(_oriShot);
}

const std::vector<Map>& MapManager::getMaps() const {
    return _shots.top().maps;
}

const std::vector<POI>& MapManager::getPois() const {
    return _shots.top().pois;
}
BlockPosition MapManager::getPlayerPos() const {
    return _shots.top().playerPos;
}

std::string MapManager::addNewMap(size_t row, size_t column) {
    return _shots.top().addNewMap(row, column);
}

bool MapManager::addNewMap(size_t row, size_t column, std::string id) {
    return _shots.top().addNewMap(row, column, id);
}

void MapManager::_push_shot(const Shot& shot) { _shots.push(shot); }

bool MapManager::undo() {
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

const Map& MapManager::getMapById(std::string id) const {
    return _shots.top().getMapById(id);
}

Map& MapManager::getMapById(std::string id) {
    return _shots.top().getMapById(id);
}

bool MapManager::saveShot(std::string path) {
    return ShotFile::write(path, _shots.top());
}

void MapManager::readShot(std::string shotPath) {
    Shot shot = ShotFile::read(shotPath);
    while(!_shots.empty())
        _shots.pop();
    _oriShot = shot;
    _shots.push(shot);
}

}  // namespace PushBoxes
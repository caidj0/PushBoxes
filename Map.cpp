#include "Map.h"

#include <cassert>
#include <cstddef>
#include <cstdio>
#include <stack>
#include <stdexcept>
#include <string>
#include <utility>
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
    addNewMap(9, 9, meta_map_id);
    Map& meta_map = getMapById(meta_map_id);
    for (size_t i = 0; i < 9; i++) {
        meta_map.blocks[0][i].setBlockType(WALL_BLOCK);
        meta_map.blocks[8][i].setBlockType(WALL_BLOCK);
        meta_map.blocks[i][0].setBlockType(WALL_BLOCK);
        meta_map.blocks[i][8].setBlockType(WALL_BLOCK);
    }
    Map& map = getMapById(map_id);
    map.pos = {4, 4, meta_map_id};
    map.isInMap = 1;
    meta_map.blocks[4][4].setBlockType(MAP_BLOCK);
    meta_map.blocks[4][4].inner_map_id = map_id;
    return meta_map_id;
}

BlockPosition MapManager::Shot::getBlockOutside(std::string map_id,
                                                Direction direction) {
    Map& map = getMapById(map_id);
    if (!map.isInMap) containMapWithVoid(map_id);
    return getNearbyBlock(map.pos, direction);
}

BlockPosition MapManager::Shot::getNearbyBlock(BlockPosition pos,
                                               Direction direction) {
    BlockPosition targetPos = pos;
    Map& map = getMapById(pos.map_id);
    if (direction == UP) {
        if (targetPos.x > 0) {
            targetPos.x--;
        } else {
            targetPos = getBlockOutside(pos.map_id, direction);
        }
    } else if (direction == DOWN) {
        if (targetPos.x + 1 < map.row) {
            targetPos.x++;
        } else {
            targetPos = getBlockOutside(pos.map_id, direction);
        }
    } else if (direction == LEFT) {
        if (targetPos.y > 0) {
            targetPos.y--;
        } else {
            targetPos = getBlockOutside(pos.map_id, direction);
        }
    } else {
        if (targetPos.y + 1 < map.column) {
            targetPos.y++;
        } else {
            targetPos = getBlockOutside(pos.map_id, direction);
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

int MapManager::Shot::moveBlock(BlockPosition targetPos, BlockPosition fromPos, Block fromBlock) {
    Block &targetRef = getBlockByPos(targetPos);
    Block &fromRef = getBlockByPos(fromPos);
    
    targetRef = fromBlock;
    targetRef.moving_trend = NODIRECTION;

    if(fromBlock.getBlockType() == PLAYER_BLOCK) {
        playerPos = targetPos;
    } else if(fromBlock.getBlockType() == MAP_BLOCK) {
        getMapById(fromBlock.inner_map_id).pos = targetPos;
    }

    if(fromRef.moving_trend == NODIRECTION) {
        return 2;
    } else {
        fromRef.setBlockType(VOID_BLOCK);
        fromRef.moving_trend = NODIRECTION;
        return 3;
    }
}

// 返回值: 3 成功腾出这个位置; 2 inBlock 未能成功腾出这个位置,
// 但是有其他变化; 1 未能成功腾出这个位置, 且没有其他变化, 但是有备选位置;
// 0 未能成功腾出这个位置, 且没有其他变化
std::pair<int, BlockPosition> MapManager::Shot::move(BlockPosition pos,
                                                     Direction direction) {
    Block& ref = getBlockByPos(pos);
    Block blockBackup = ref;

    if (ref.getBlockType().isReplaceable) return {3, BlockPosition()};

    if (!ref.getBlockType().isMoveable) return {0, BlockPosition()};

    if (ref.moving_trend != NODIRECTION && ref.moving_trend != direction)
        return {0, BlockPosition()};

    if (ref.moving_trend == direction) {
        return {3, BlockPosition()};
    }

    ref.moving_trend = direction;

    int flag = 1;

    std::stack<BlockPosition> targetPoses; 
    targetPoses.push(getNearbyBlock(pos, direction));

    while (flag == 1) {
        auto ret = move(targetPoses.top(), direction);
        if (ret.first == 1)
            targetPoses.top() = ret.second;
        else
            flag = ret.first;
    }

    if (flag == 3) {
        return {moveBlock(targetPoses.top(), pos, blockBackup), BlockPosition()};
    }

    if(flag == 2) {
        return {2, BlockPosition()};
    }

    assert(ref.moving_trend == direction);
    assert(flag == 0);

    while(!targetPoses.empty() && !getBlockByPos(targetPoses.top()).getBlockType().isMoveable)
        targetPoses.pop();

    if(!targetPoses.empty()) {
        if (getBlockByPos(targetPoses.top()).getBlockType().isMoveable && ref.getBlockType().isAccessible) {
            BlockPosition innerPos =
                getAccessPosition(&ref, inverseDirection(direction));
            flag = 1;
            while (flag == 1) {
                auto ret = move(innerPos, inverseDirection(direction));
                if (ret.first == 1)
                    innerPos = ret.second;
                else
                    flag = ret.first;
            }
            if (flag == 3) {
                moveBlock(innerPos, targetPoses.top(), getBlockByPos(targetPoses.top()));
                return {moveBlock(targetPoses.top(), pos, blockBackup), BlockPosition()};
            }
        }

    }

    assert(flag == 2 || flag == 0);

    ref.moving_trend = NODIRECTION;

    if (flag == 0 && ref.getBlockType().isAccessible)
        return {1, getAccessPosition(&ref, direction)};

    return {flag, BlockPosition()};
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
    setBlock(pos, PushBoxes::Block(PushBoxes::MAP_BLOCK, map_id));
}

bool MapManager::movePlayer(Direction direction) {
    Shot temp_shot = _shots.top();
    int flag = temp_shot.move(temp_shot.playerPos, direction).first;
    if (flag == 2 || flag == 3) {
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

const std::list<Map>& MapManager::getMaps() const { return _shots.top().maps; }

const std::list<POI>& MapManager::getPois() const { return _shots.top().pois; }
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
    while (!_shots.empty()) _shots.pop();
    _oriShot = shot;
    _shots.push(shot);
}

}  // namespace PushBoxes
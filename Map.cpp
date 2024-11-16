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

EnterPosition::EnterPosition() : vaild(0), pos(), ratio() {}

EnterPosition::EnterPosition(BlockPosition pos)
    : vaild(1), pos(pos), ratio(0.5) {}

EnterPosition::EnterPosition(BlockPosition pos, double ratio)
    : vaild(1), pos(pos), ratio(ratio){};

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

EnterPosition MapManager::Shot::getBlockOutside(EnterPosition enter_pos,
                                                Direction direction) {
    Map& map = getMapById(enter_pos.pos.map_id);
    if (!map.isInMap) containMapWithVoid(enter_pos.pos.map_id);
    if (direction == UP || direction == DOWN) {
        enter_pos.ratio =
            1.0 / map.column * (enter_pos.pos.y + enter_pos.ratio);
    } else {
        enter_pos.ratio = 1.0 / map.row * (enter_pos.pos.x + enter_pos.ratio);
    }
    enter_pos.pos = map.pos;
    return getNearbyBlock(enter_pos, direction);
}

EnterPosition MapManager::Shot::getNearbyBlock(EnterPosition enter_pos,
                                               Direction direction) {
    EnterPosition targetPos = enter_pos;
    Map& map = getMapById(enter_pos.pos.map_id);
    if (direction == UP) {
        if (targetPos.pos.x > 0) {
            targetPos.pos.x--;
        } else {
            targetPos = getBlockOutside(enter_pos, direction);
        }
    } else if (direction == DOWN) {
        if (targetPos.pos.x + 1 < map.row) {
            targetPos.pos.x++;
        } else {
            targetPos = getBlockOutside(enter_pos, direction);
        }
    } else if (direction == LEFT) {
        if (targetPos.pos.y > 0) {
            targetPos.pos.y--;
        } else {
            targetPos = getBlockOutside(enter_pos, direction);
        }
    } else {
        if (targetPos.pos.y + 1 < map.column) {
            targetPos.pos.y++;
        } else {
            targetPos = getBlockOutside(enter_pos, direction);
        }
    }

    return targetPos;
}

EnterPosition MapManager::Shot::getAccessPosition(Block* targetBlock,
                                                  Direction direction,
                                                  double ratio) {
    Map& map = getMapById(targetBlock->inner_map_id);
    EnterPosition enter_pos;
    enter_pos.vaild = 1;
    enter_pos.pos.map_id = targetBlock->inner_map_id;
    if (direction == DOWN) {
        enter_pos.pos.x = 0;
        enter_pos.pos.y = map.column * ratio;
        enter_pos.ratio = ratio * map.column - enter_pos.pos.y;
    } else if (direction == UP) {
        enter_pos.pos.x = map.row - 1;
        enter_pos.pos.y = map.column * ratio;
        enter_pos.ratio = ratio * map.column - enter_pos.pos.y;
    } else if (direction == RIGHT) {
        enter_pos.pos.x = map.row * ratio;
        enter_pos.pos.y = 0;
        enter_pos.ratio = ratio * map.row - enter_pos.pos.x;
    } else {
        enter_pos.pos.x = map.row * ratio;
        enter_pos.pos.y = map.column - 1;
        enter_pos.ratio = ratio * map.row - enter_pos.pos.x;
    }
    return enter_pos;
}

bool MapManager::Shot::isWin() const {
    size_t poi_num = 0, achieved_num = 0;
    for (const auto& map : maps) {
        for (const auto& poi : map.pois) {
            poi_num++;

            achieved_num +=
                (poi.second == NEEDPLAYER &&
                 map.blocks[poi.first.first][poi.first.second].getBlockType() ==
                     PLAYER_BLOCK) ||
                (poi.second == NEEDBLOCK &&
                 map.blocks[poi.first.first][poi.first.second]
                     .getBlockType()
                     .isMoveable);
        }
    }
    return (poi_num != 0 && poi_num == achieved_num);
}

int MapManager::Shot::moveBlock(BlockPosition targetPos, BlockPosition fromPos,
                                Block fromBlock) {
    Block& targetRef = getBlockByPos(targetPos);
    Block& fromRef = getBlockByPos(fromPos);

    targetRef = fromBlock;
    targetRef.moving_trend = NODIRECTION;

    if (fromBlock.getBlockType() == PLAYER_BLOCK) {
        playerPos = targetPos;
    } else if (fromBlock.getBlockType() == MAP_BLOCK) {
        getMapById(fromBlock.inner_map_id).pos = targetPos;
    }

    if (fromRef.moving_trend == NODIRECTION) {
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
std::pair<int, EnterPosition> MapManager::Shot::move(EnterPosition enter_pos,
                                                     Direction direction) {
    Block& ref = getBlockByPos(enter_pos.pos);
    Block blockBackup = ref;

    if (ref.getBlockType().isReplaceable) return {3, EnterPosition()};

    if (!ref.getBlockType().isMoveable) return {0, EnterPosition()};

    if (ref.moving_trend != NODIRECTION && ref.moving_trend != direction)
        return {0, EnterPosition()};

    if (ref.moving_trend == direction) {
        return {3, EnterPosition()};
    }

    ref.moving_trend = direction;

    int flag = 1;

    std::stack<EnterPosition> targetPoses;
    targetPoses.push(getNearbyBlock(EnterPosition(enter_pos.pos), direction));

    while (flag == 1) {
        auto ret = move(targetPoses.top(), direction);
        if (ret.first == 1)
            targetPoses.top() = ret.second;
        else
            flag = ret.first;
    }

    if (flag == 3) {
        return {moveBlock(targetPoses.top().pos, enter_pos.pos, blockBackup),
                EnterPosition()};
    }

    if (flag == 2) {
        return {2, EnterPosition()};
    }

    assert(ref.moving_trend == direction);
    assert(flag == 0);

    while (!targetPoses.empty() &&
           !getBlockByPos(targetPoses.top().pos).getBlockType().isMoveable)
        targetPoses.pop();

    if (!targetPoses.empty()) {
        if (getBlockByPos(targetPoses.top().pos).getBlockType().isMoveable &&
            ref.getBlockType().isAccessible) {
            EnterPosition innerPos =
                getAccessPosition(&ref, inverseDirection(direction), 0.5);
            flag = 1;
            while (flag == 1) {
                auto ret = move(innerPos, inverseDirection(direction));
                if (ret.first == 1)
                    innerPos = ret.second;
                else
                    flag = ret.first;
            }
            if (flag == 3) {
                moveBlock(innerPos.pos, targetPoses.top().pos,
                          getBlockByPos(targetPoses.top().pos));
                return {moveBlock(targetPoses.top().pos, enter_pos.pos,
                                  blockBackup),
                        EnterPosition()};
            }
        }
    }

    assert(flag == 2 || flag == 0);

    ref.moving_trend = NODIRECTION;

    if (flag == 0 && ref.getBlockType().isAccessible)
        return {1, getAccessPosition(&ref, direction, enter_pos.ratio)};

    return {flag, EnterPosition()};
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
    if (block.getBlockType() != PLAYER_BLOCK &&
        block.getBlockType() != MAP_BLOCK) {
        _shots.top().getMapById(pos.map_id).blocks[pos.x][pos.y] = block;
    }
}

void MapManager::setBlock(BlockPosition pos, const BlockType& blockType) {
    if (blockType != PLAYER_BLOCK && blockType != MAP_BLOCK &&
        blockType != CLONE_BLOCK) {
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
    int flag =
        temp_shot.move(EnterPosition(temp_shot.playerPos), direction).first;
    if (flag == 2 || flag == 3) {
        isWin = temp_shot.isWin();
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
        isWin = _shots.top().isWin();
        return true;
    }
    return false;
}

void MapManager::reset() {
    _push_shot(_oriShot);
    isWin = 0;
}

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
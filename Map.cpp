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

EnterPosition::EnterPosition(BlockPosition pos, Direction direction)
    : vaild(1), pos(pos), ratio(0.5), direction(direction), isFilped(0) {}

EnterPosition::EnterPosition(BlockPosition pos, Direction direction,
                             double ratio)
    : vaild(1), pos(pos), ratio(ratio), direction(direction), isFilped(0){};

EnterPosition::EnterPosition(BlockPosition pos, Direction direction,
                             double ratio, bool isFliped)
    : vaild(1),
      pos(pos),
      ratio(ratio),
      direction(direction),
      isFilped(isFliped){};

Direction EnterPosition::getDirection() const {
    if (isFilped) {
        if (direction == LEFT) return RIGHT;
        if (direction == RIGHT) return LEFT;
    }
    return direction;
}

double EnterPosition::getRatio() const {
    if (isFilped && (direction == UP || direction == DOWN)) return 1 - ratio;
    return ratio;
}

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

EnterPosition MapManager::Shot::getBlockOutside(EnterPosition enter_pos) {
    Map& map = getMapById(enter_pos.pos.map_id);
    if (!map.isInMap) containMapWithVoid(enter_pos.pos.map_id);

    enter_pos.isFilped ^= getBlockByPos(map.pos).isFliped;

    if (enter_pos.getDirection() == UP || enter_pos.getDirection() == DOWN) {
        enter_pos.ratio =
            1.0 / map.column * (enter_pos.pos.y + enter_pos.getRatio());
    } else {
        enter_pos.ratio =
            1.0 / map.row * (enter_pos.pos.x + enter_pos.getRatio());
    }
    enter_pos.pos = map.pos;
    return getNearbyBlock(enter_pos);
}

EnterPosition MapManager::Shot::getNearbyBlock(EnterPosition enter_pos) {
    EnterPosition targetPos = enter_pos;
    Map& map = getMapById(enter_pos.pos.map_id);
    if (enter_pos.getDirection() == UP) {
        if (targetPos.pos.x > 0) {
            targetPos.pos.x--;
        } else {
            targetPos = getBlockOutside(enter_pos);
        }
    } else if (enter_pos.getDirection() == DOWN) {
        if (targetPos.pos.x + 1 < map.row) {
            targetPos.pos.x++;
        } else {
            targetPos = getBlockOutside(enter_pos);
        }
    } else if (enter_pos.getDirection() == LEFT) {
        if (targetPos.pos.y > 0) {
            targetPos.pos.y--;
        } else {
            targetPos = getBlockOutside(enter_pos);
        }
    } else {
        if (targetPos.pos.y + 1 < map.column) {
            targetPos.pos.y++;
        } else {
            targetPos = getBlockOutside(enter_pos);
        }
    }

    return targetPos;
}

EnterPosition MapManager::Shot::getAccessPosition(EnterPosition old_enter_pos) {
    Block& targetBlock = getBlockByPos(old_enter_pos.pos);
    Map& map = getMapById(targetBlock.inner_map_id);
    EnterPosition enter_pos({0, 0, targetBlock.inner_map_id},
                            old_enter_pos.direction, old_enter_pos.ratio,
                            targetBlock.isFliped ^ old_enter_pos.isFilped);

    if (enter_pos.getDirection() == DOWN) {
        enter_pos.pos.x = 0;
        enter_pos.pos.y = map.column * enter_pos.getRatio();
        enter_pos.ratio = enter_pos.getRatio() * map.column - enter_pos.pos.y;
    } else if (enter_pos.getDirection() == UP) {
        enter_pos.pos.x = map.row - 1;
        enter_pos.pos.y = map.column * enter_pos.getRatio();
        enter_pos.ratio = enter_pos.getRatio() * map.column - enter_pos.pos.y;
    } else if (enter_pos.getDirection() == RIGHT) {
        enter_pos.pos.x = map.row * enter_pos.getRatio();
        enter_pos.pos.y = 0;
        enter_pos.ratio = enter_pos.getRatio() * map.row - enter_pos.pos.x;
    } else {
        enter_pos.pos.x = map.row * enter_pos.getRatio();
        enter_pos.pos.y = map.column - 1;
        enter_pos.ratio = enter_pos.getRatio() * map.row - enter_pos.pos.x;
    }
    return enter_pos;
}

bool MapManager::Shot::isWin() const {
    size_t poi_num = 0, achieved_num = 0;
    for (const auto& map : maps) {
        for (const auto& poi : map.pois) {
            poi_num++;

            achieved_num += meetPOIDemand(
                poi.second, map.blocks[poi.first.first][poi.first.second]);
        }
    }
    return (poi_num != 0 && poi_num == achieved_num);
}

int MapManager::Shot::moveBlock(EnterPosition targetPos, EnterPosition fromPos,
                                Block fromBlock) {
    Block& targetRef = getBlockByPos(targetPos.pos);
    Block& fromRef = getBlockByPos(fromPos.pos);

    targetRef = fromBlock;
    targetRef.moving_trend = NODIRECTION;

    targetRef.isFliped = fromRef.isFliped ^ targetPos.isFilped;

    if (fromBlock.playerStatus == 2) {
        playerPoses.at(fromBlock.playerIndex) = targetPos.pos;
    }
    if (fromBlock.getBlockType() == MAP_BLOCK) {
        getMapById(fromBlock.inner_map_id).pos = targetPos.pos;
    }

    if (fromRef.moving_trend == NODIRECTION) {
        return 2;
    } else {
        fromRef = VOID_BLOCK;
        return 3;
    }
}

void MapManager::Shot::transferPlayer(EnterPosition targetPos, EnterPosition fromPos) {
    Block& targetRef = getBlockByPos(targetPos.pos);
    Block& fromRef = getBlockByPos(fromPos.pos);

    targetRef.playerStatus = 2;
    fromRef.playerStatus = 1;
    targetRef.playerIndex = fromRef.playerIndex;
    playerPoses.at(fromRef.playerIndex) = targetPos.pos;
}

// 返回值: 3 成功腾出这个位置; 2 inBlock 未能成功腾出这个位置,
// 但是有其他变化; 1 未能成功腾出这个位置, 且没有其他变化, 但是有备选位置;
// 0 未能成功腾出这个位置, 且没有其他变化
std::pair<int, EnterPosition> MapManager::Shot::move(EnterPosition enter_pos, bool force_move) {
    assert(enter_pos.direction != NODIRECTION);
    Block& ref = getBlockByPos(enter_pos.pos);
    Block blockBackup = ref;

    if (ref.getBlockType().isReplaceable) return {3, EnterPosition()};

    if (!force_move && !ref.getBlockType().isMoveable) return {0, EnterPosition()};

    if (ref.moving_trend != NODIRECTION &&
        ref.moving_trend != enter_pos.getDirection()) {
        if (ref.getBlockType().isAccessible)
            return {1, getAccessPosition(enter_pos)};
        return {0, EnterPosition()};
    }

    if (ref.moving_trend == enter_pos.getDirection()) {
        return {3, EnterPosition()};
    }

    ref.moving_trend = enter_pos.getDirection();

    int flag = 1;

    std::stack<EnterPosition> targetPoses;
    targetPoses.push(
        getNearbyBlock(EnterPosition(enter_pos.pos, enter_pos.getDirection())));

    while (flag == 1) {
        auto ret = move(targetPoses.top());
        if (ret.first == 1)
            targetPoses.push(ret.second);
        else
            flag = ret.first;
    }

    if (flag == 3) {
        return {moveBlock(targetPoses.top(), enter_pos, blockBackup),
                EnterPosition()};
    }

    if (flag == 2) {
        ref.moving_trend = NODIRECTION;
        return {2, EnterPosition()};
    }

    assert(flag == 0);

    EnterPosition accessPos, takeupPos;
    while (!targetPoses.empty()) {
        if (!accessPos.vaild &&
            getBlockByPos(targetPoses.top().pos).getBlockType().isMoveable)
            accessPos = targetPoses.top();
        if (!takeupPos.vaild &&
            getBlockByPos(targetPoses.top().pos).playerStatus == 1)
            takeupPos = targetPoses.top();
        targetPoses.pop();
    }

    if (flag == 0 && accessPos.vaild) {
        Block& targetRef = getBlockByPos(accessPos.pos);
        if (targetRef.getBlockType().isMoveable &&
            ref.getBlockType().isAccessible) {
            Block targetBlock = targetRef;
            targetRef.moving_trend = inverseDirection(enter_pos.getDirection());
            EnterPosition innerPos = getAccessPosition(
                EnterPosition(enter_pos.pos,
                              inverseDirection(enter_pos.getDirection()), 0.5));
            flag = 1;
            while (flag == 1) {
                auto ret = move(innerPos);
                if (ret.first == 1)
                    innerPos = ret.second;
                else
                    flag = ret.first;
            }
            if (flag == 3) {
                flag = moveBlock(innerPos, accessPos, targetBlock);
                if (flag == 3) {
                    return {
                        moveBlock(accessPos, enter_pos, blockBackup),
                        EnterPosition()};
                }
            }
            targetRef.moving_trend = NODIRECTION;
        }
    }

    if(flag == 0 && ref.playerStatus == 2 && takeupPos.vaild) {
        transferPlayer(takeupPos, enter_pos);
        flag = 2;
    }

    assert(flag == 2 || flag == 0);

    ref.moving_trend = NODIRECTION;

    if (flag == 0 && ref.getBlockType().isAccessible)
        return {1, getAccessPosition(enter_pos)};

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

void MapManager::addPlayer(BlockPosition pos) {
    Block& block = _shots.top().getMapById(pos.map_id).blocks[pos.x][pos.y];
    if (block.getBlockType() != VOID_BLOCK) {
        _shots.top().playerPoses.push_back(pos);
        block.playerStatus = 2;
        block.playerIndex = _shots.top().playerPoses.size() - 1;
    }
}

void MapManager::setBlock(BlockPosition pos, Block block) {
    if (block.getBlockType() != MAP_BLOCK) {
        _shots.top().getMapById(pos.map_id).blocks[pos.x][pos.y] = block;
    }
}

void MapManager::setBlock(BlockPosition pos, const BlockType& blockType) {
    if (blockType != MAP_BLOCK && blockType != CLONE_BLOCK) {
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
    int flag = 0;
    for (auto x : temp_shot.playerPoses) {
        flag |= temp_shot.move(EnterPosition(x, direction), 1).first;
    }
    if (flag & 2) {
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

const std::vector<BlockPosition>& MapManager::getPlayerPoses() const {
    return _shots.top().playerPoses;
}

std::string MapManager::addNewMap(size_t row, size_t column) {
    return _shots.top().addNewMap(row, column);
}

bool MapManager::addNewMap(size_t row, size_t column, std::string id) {
    return _shots.top().addNewMap(row, column, id);
}

bool MapManager::addPoi(std::string map_id, size_t row, size_t column,
                        POIType type) {
    return getMapById(map_id).pois.insert({{row, column}, type}).second;
}

bool MapManager::deletePoi(std::string map_id, size_t row, size_t column) {
    Map& map = getMapById(map_id);
    return map.pois.erase({row, column});
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
    isWin = _shots.top().isWin();
}

}  // namespace PushBoxes
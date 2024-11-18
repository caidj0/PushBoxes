#include "Map.h"

#include <cassert>
#include <cstddef>
#include <cstdio>
#include <set>
#include <stack>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "Block.h"
#include "Blocks.h"
#include "Exceptions.h"
#include "ShotFile.h"

namespace PushBoxes {

bool BlockPosition::operator<(const BlockPosition& Right) const {
    return (map_id < Right.map_id) ||
           (map_id == Right.map_id &&
            (x < Right.x || (x == Right.x && y < Right.y)));
}

bool BlockPosition::operator==(const BlockPosition& Right) const {
    return map_id == Right.map_id && x == Right.x && y == Right.y;
}

FixedPosition::FixedPosition() : vaild(0), pos(), ratio() {}

FixedPosition::FixedPosition(BlockPosition pos, Direction direction)
    : vaild(1), pos(pos), ratio(0.5), direction(direction), isFilped(0) {}

FixedPosition::FixedPosition(BlockPosition pos, Direction direction,
                             double ratio)
    : vaild(1), pos(pos), ratio(ratio), direction(direction), isFilped(0){};

FixedPosition::FixedPosition(BlockPosition pos, Direction direction,
                             double ratio, bool isFliped)
    : vaild(1),
      pos(pos),
      ratio(ratio),
      direction(direction),
      isFilped(isFliped){};

bool FixedPosition::operator<(const FixedPosition& Right) const {
    return pos < Right.pos || (pos == Right.pos && ratio < Right.ratio);
}

bool FixedPosition::operator==(const FixedPosition& Right) const {
    return pos == Right.pos && ratio == Right.ratio;
}

Direction FixedPosition::getDirection() const {
    if (isFilped) {
        if (direction == LEFT) return RIGHT;
        if (direction == RIGHT) return LEFT;
    }
    return direction;
}

double FixedPosition::getRatio() const {
    if (isFilped && (direction == UP || direction == DOWN)) return 1 - ratio;
    return ratio;
}

Map::Map(size_t row, size_t column, std::string id)
    : row(row),
      column(column),
      id(id),
      blocks(row, std::vector<PushBoxes::Block>(column, {VOID_BLOCK, id})),
      pois(),
      isInMap(0),
      isMetaMap(0),
      pos(),
      infPoses(),
      epsPoses() {}

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

const Block& MapManager::Shot::getBlockByPos(BlockPosition pos) const {
    return getMapById(pos.map_id).blocks[pos.x][pos.y];
}

std::string MapManager::Shot::containBlockWithMeta(Block ref) {
    std::string meta_map_id;
    if (ref.getBlockType() == MAP_BLOCK) {
        meta_map_id = "Meta_" + ref.inner_map_id;
    } else if (ref.getBlockType() == INF_BLOCK) {
        meta_map_id =
            "Meta_" + ref.inner_map_id + "^" + std::to_string(ref.inf_level);
    }
    addNewMap(9, 9, meta_map_id);
    Map& meta_map = getMapById(meta_map_id);
    meta_map.isMetaMap = 1;
    for (size_t i = 0; i < 9; i++) {
        meta_map.blocks[0][i].setBlockType(WALL_BLOCK);
        meta_map.blocks[8][i].setBlockType(WALL_BLOCK);
        meta_map.blocks[i][0].setBlockType(WALL_BLOCK);
        meta_map.blocks[i][8].setBlockType(WALL_BLOCK);
    }
    Map& map = getMapById(ref.inner_map_id);
    if (ref.getBlockType() == MAP_BLOCK) {
        map.pos = {4, 4, meta_map_id};
        map.isInMap = 1;
    } else if (ref.getBlockType() == INF_BLOCK) {
        map.infPoses[ref.inf_level - 1] = {4, 4, meta_map_id};
    }
    meta_map.blocks[4][4] = ref;

    return meta_map_id;
}

std::string MapManager::Shot::containMapWithMeta(std::string map_id) {
    return containBlockWithMeta(Block(MAP_BLOCK, map_id));
}

FixedPosition MapManager::Shot::getPosOutside(FixedPosition enter_pos) {
    Map& map = getMapById(enter_pos.pos.map_id);
    if (!map.isInMap) containMapWithMeta(enter_pos.pos.map_id);

    enter_pos.isFilped ^= getBlockByPos(map.pos).isFliped;

    if (enter_pos.getDirection() == UP || enter_pos.getDirection() == DOWN) {
        enter_pos.ratio =
            1.0 / map.column * (enter_pos.pos.y + enter_pos.getRatio());
    } else {
        enter_pos.ratio =
            1.0 / map.row * (enter_pos.pos.x + enter_pos.getRatio());
    }
    enter_pos.pos = map.pos;
    return enter_pos;
}

FixedPosition MapManager::Shot::getNearbyBlock(FixedPosition enter_pos) {
    auto is_out = [&](FixedPosition pos) {
        Map& map = getMapById(pos.pos.map_id);
        if (pos.getDirection() == UP) {
            return pos.pos.x == 0;
        } else if (pos.getDirection() == DOWN) {
            return pos.pos.x + 1 == map.row;
        } else if (pos.getDirection() == LEFT) {
            return pos.pos.y == 0;
        } else {
            return pos.pos.y + 1 == map.column;
        }
    };

    std::set<FixedPosition> set;
    FixedPosition targetPos = enter_pos;

    while (is_out(targetPos)) {
        if (!set.insert(targetPos).second)
            targetPos = getInfinityBlock(targetPos);
        else
            targetPos = getPosOutside(targetPos);
    }

    if (targetPos.getDirection() == UP) {
        targetPos.pos.x--;
    } else if (targetPos.getDirection() == DOWN) {
        targetPos.pos.x++;
    } else if (targetPos.getDirection() == LEFT) {
        targetPos.pos.y--;
    } else {
        targetPos.pos.y++;
    }

    return targetPos;
}

FixedPosition MapManager::Shot::getAccessPosition(FixedPosition old_enter_pos) {
    Block& targetBlock = getBlockByPos(old_enter_pos.pos);
    Map& map = getMapById(targetBlock.inner_map_id);
    FixedPosition enter_pos({0, 0, targetBlock.inner_map_id},
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

int MapManager::Shot::moveBlock(FixedPosition targetPos, FixedPosition fromPos,
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
    } else if (fromBlock.getBlockType() == INF_BLOCK) {
        getMapById(fromBlock.inner_map_id).infPoses[fromBlock.inf_level - 1] =
            targetPos.pos;
    }

    if (fromRef.moving_trend == NODIRECTION) {
        return 2;
    } else {
        fromRef = VOID_BLOCK;
        return 3;
    }
}

void MapManager::Shot::transferPlayer(FixedPosition targetPos,
                                      FixedPosition fromPos) {
    Block& targetRef = getBlockByPos(targetPos.pos);
    Block& fromRef = getBlockByPos(fromPos.pos);

    targetRef.playerStatus = 2;
    fromRef.playerStatus = 1;
    targetRef.playerIndex = fromRef.playerIndex;
    playerPoses.at(fromRef.playerIndex) = targetPos.pos;
}

FixedPosition MapManager::Shot::getInfinityBlock(FixedPosition fixed_pos) {
    Block& ref = getBlockByPos(fixed_pos.pos);
    Map& map = getMapById(ref.inner_map_id);
    long long inf_level;
    if (ref.getBlockType() == MAP_BLOCK) {
        inf_level = 1;
    } else if (ref.getBlockType() == INF_BLOCK) {
        inf_level = ref.inf_level + 1;
    }

    assert(inf_level > 0);

    if (map.infPoses.size() < inf_level) {
        map.infPoses.emplace_back();
        containBlockWithMeta(Block(INF_BLOCK, ref.inner_map_id, inf_level));
    }

    fixed_pos.pos = map.infPoses[inf_level - 1];
    fixed_pos.isFilped ^= getBlockByPos(fixed_pos.pos).isFliped;
    return fixed_pos;
}

bool MapManager::Shot::isAccessible(BlockPosition pos) const {
    const Map& map = getMapById(pos.map_id);
    const Block& ref = getBlockByPos(pos);
    return ref.getBlockType().isAccessible && !map.isMetaMap;
}

// 返回值: 3 成功腾出这个位置; 2 inBlock 未能成功腾出这个位置,
// 但是有其他变化; 1 未能成功腾出这个位置, 且没有其他变化, 但是有备选位置;
// 0 未能成功腾出这个位置, 且没有其他变化
std::pair<int, FixedPosition> MapManager::Shot::move(FixedPosition enter_pos,
                                                     bool force_move) {
    assert(enter_pos.direction != NODIRECTION);
    Block& ref = getBlockByPos(enter_pos.pos);
    Block blockBackup = ref;

    if (ref.getBlockType().isReplaceable) return {3, FixedPosition()};

    if (!force_move && !ref.getBlockType().isMoveable)
        return {0, FixedPosition()};

    if (ref.moving_trend != NODIRECTION &&
        ref.moving_trend != enter_pos.getDirection()) {
        if (isAccessible(enter_pos.pos))
            return {1, getAccessPosition(enter_pos)};
        return {0, FixedPosition()};
    }

    if (ref.moving_trend == enter_pos.getDirection()) {
        return {3, FixedPosition()};
    }

    ref.moving_trend = enter_pos.getDirection();

    int flag = 1;

    std::stack<FixedPosition> targetPoses;
    targetPoses.push(
        getNearbyBlock(FixedPosition(enter_pos.pos, enter_pos.getDirection())));

    while (flag == 1) {
        auto ret = move(targetPoses.top());
        if (ret.first == 1)
            targetPoses.push(ret.second);
        else
            flag = ret.first;
    }

    if (flag == 3) {
        return {moveBlock(targetPoses.top(), enter_pos, blockBackup),
                FixedPosition()};
    }

    if (flag == 2) {
        ref.moving_trend = NODIRECTION;
        return {2, FixedPosition()};
    }

    assert(flag == 0);

    FixedPosition swallowPos, takeupPos;
    while (!targetPoses.empty()) {
        Block& tempRef = getBlockByPos(targetPoses.top().pos);
        if (!swallowPos.vaild && tempRef.getBlockType().isMoveable &&
            tempRef.moving_trend == NODIRECTION)
            swallowPos = targetPoses.top();
        if (!takeupPos.vaild && tempRef.playerStatus == 1)
            takeupPos = targetPoses.top();
        targetPoses.pop();
    }

    if (flag == 0 && swallowPos.vaild) {
        Block& targetRef = getBlockByPos(swallowPos.pos);
        if (targetRef.getBlockType().isMoveable &&
            isAccessible(enter_pos.pos)) {
            Block targetBlock = targetRef;
            targetRef.moving_trend = inverseDirection(enter_pos.getDirection());
            FixedPosition innerPos = getAccessPosition(
                FixedPosition(enter_pos.pos,
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
                flag = moveBlock(innerPos, swallowPos, targetBlock);
                if (flag == 3) {
                    return {moveBlock(swallowPos, enter_pos, blockBackup),
                            FixedPosition()};
                }
            }
            targetRef.moving_trend = NODIRECTION;
        }
    }

    if (flag == 0 && ref.playerStatus == 2 && takeupPos.vaild) {
        transferPlayer(takeupPos, enter_pos);
        flag = 2;
    }

    assert(flag == 2 || flag == 0);

    ref.moving_trend = NODIRECTION;

    if (flag == 0 && isAccessible(enter_pos.pos))
        return {1, getAccessPosition(enter_pos)};

    return {flag, FixedPosition()};
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
        flag |= temp_shot.move(FixedPosition(x, direction), 1).first;
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
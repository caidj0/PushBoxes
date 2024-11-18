#pragma once
#include <cstddef>
#include <list>
#include <map>
#include <stack>
#include <string>
#include <utility>
#include <vector>

#include "Block.h"

namespace PushBoxes {

struct BlockPosition {
    size_t x, y;
    std::string map_id;
    bool operator<(const BlockPosition& Right) const;
    bool operator==(const BlockPosition& Right) const;
};

struct FixedPosition {
    bool vaild;
    BlockPosition pos;
    double ratio;
    Direction direction;
    bool isFilped;
    FixedPosition();
    explicit FixedPosition(BlockPosition pos, Direction direction);
    FixedPosition(BlockPosition pos, Direction direction, double ratio);
    FixedPosition(BlockPosition pos, Direction direction, double ratio, bool isFilped);

    Direction getDirection() const;
    double getRatio() const;
    bool operator<(const FixedPosition& Right) const;
    bool operator==(const FixedPosition& Right) const;
};

class Map {
   public:
    size_t row, column;
    std::vector<std::vector<PushBoxes::Block>> blocks;
    std::map<std::pair<size_t, size_t>, POIType> pois;
    std::string id;
    bool isInMap;
    bool isMetaMap;
    BlockPosition pos;
    std::vector<BlockPosition> infPoses;
    std::vector<BlockPosition> epsPoses;
    Map(size_t row, size_t column, std::string id);
};

class MapManager {
   private:
    struct Shot {
        size_t id_counter;
        std::list<Map> maps;
        std::vector<PushBoxes::BlockPosition> playerPoses;

        int moveBlock(FixedPosition targetPos, FixedPosition fromPos,
                      Block fromBlock);
        void transferPlayer(FixedPosition targetPos, FixedPosition fromPos);
        std::pair<int, FixedPosition> move(FixedPosition enter_pos, bool force_move = false);
        Map& getMapById(std::string id);
        const Map& getMapById(std::string id) const;
        Block& getBlockByPos(BlockPosition pos);
        const Block& getBlockByPos(BlockPosition pos) const;
        FixedPosition getNearbyBlock(FixedPosition enter_pos);
        std::string addNewMap(size_t row, size_t column);
        bool addNewMap(size_t row, size_t column, std::string id);
        FixedPosition getAccessPosition(FixedPosition enter_pos);
        std::string containBlockWithMeta(Block ref);
        std::string containMapWithMeta(std::string map_id);
        FixedPosition getPosOutside(FixedPosition pos);
        bool isWin() const;
        FixedPosition getInfinityBlock(FixedPosition fixed_pos);
        bool isAccessible(BlockPosition pos) const;
    };
    std::stack<Shot> _shots;
    Shot _oriShot;
    void _push_shot(const Shot& shot);

   public:
    bool isWin;

    MapManager();
    MapManager(const Shot& oriShot);
    MapManager(std::string shotPath);

    const std::list<Map>& getMaps() const;
    const std::vector<BlockPosition>& getPlayerPoses() const;
    const Map& getMapById(std::string id) const;
    Map& getMapById(std::string id);

    std::string addNewMap(size_t row, size_t column);
    bool addNewMap(size_t row, size_t column, std::string id);
    bool addPoi(std::string map_id, size_t row, size_t column, POIType type);
    bool deletePoi(std::string map_id, size_t row, size_t column);
    void addPlayer(BlockPosition pos);
    void setBlock(BlockPosition pos, Block block);
    void setBlock(BlockPosition pos, const BlockType& blockType);
    void setMapBlockPos(BlockPosition pos, std::string map_id);

    bool movePlayer(Direction md);
    bool undo();
    void reset();
    void changeOriShot();
    bool saveShot(std::string path);
    void readShot(std::string shotPath);

    friend class ShotFile;
};

}  // namespace PushBoxes
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
};

class Map {
   public:
    size_t row, column;
    std::vector<std::vector<PushBoxes::Block>> blocks;
    std::string id;
    bool isInMap;
    BlockPosition pos;
    Map(size_t row, size_t column, std::string id);
};

struct POI {
    BlockPosition pos;
    POIType type;
};

class MapManager {
   private:
    struct Shot {
        size_t id_counter;
        std::list<Map> maps;
        std::list<POI> pois;
        PushBoxes::BlockPosition playerPos;

        int moveBlock(BlockPosition targetPos, BlockPosition fromPos, Block fromBlock);
        std::pair<int, BlockPosition> move(BlockPosition pos, Direction direction);
        Map& getMapById(std::string id);
        const Map& getMapById(std::string id) const;
        Block& getBlockByPos(BlockPosition pos);
        BlockPosition getNearbyBlock(BlockPosition pos, Direction direction);
        std::string addNewMap(size_t row, size_t column);
        bool addNewMap(size_t row, size_t column, std::string id);
        BlockPosition getAccessPosition(Block* targetBlock,
                                        Direction direction);
        std::string containMapWithVoid(std::string map_id);
        BlockPosition getBlockOutside(std::string map_id, Direction direction);
    };
    std::stack<Shot> _shots;
    Shot _oriShot;
    void _push_shot(const Shot& shot);

   public:
    MapManager();
    MapManager(const Shot& oriShot);
    MapManager(std::string shotPath);

    const std::list<Map>& getMaps() const;
    const std::list<POI>& getPois() const;
    BlockPosition getPlayerPos() const;
    const Map& getMapById(std::string id) const;
    Map& getMapById(std::string id);

    std::string addNewMap(size_t row, size_t column);
    bool addNewMap(size_t row, size_t column, std::string id);
    void setPlayerPos(BlockPosition pos);
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
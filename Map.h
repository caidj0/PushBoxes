#pragma once
#include <cstddef>
#include <stack>
#include <vector>

#include "Block.h"

namespace PushBoxes {

struct BlockPosition {
    size_t x, y, map_id;
};

class Map {
   public:
    size_t row, column;
    std::vector<std::vector<PushBoxes::Block>> blocks;
    size_t id;
    bool isInMap;
    BlockPosition pos;
    Map(size_t row, size_t column, size_t id);
};

struct POI {
    BlockPosition pos;
    POIType type;
};

class MapManager {
   private:
    struct Shot {
        size_t id_counter;
        std::vector<Map> maps;
        std::vector<POI> pois;
        PushBoxes::BlockPosition playerPos;

        bool _move(BlockPosition& pos, Direction md, bool changePos);
        Map& getMapById(size_t id);
        Block& getBlockByPos(BlockPosition pos);
        BlockPosition getNearbyBlock(BlockPosition pos, Direction md);
        size_t addNewMap(size_t row, size_t column);
        BlockPosition getAccessPosition(Block* targetBlock, Direction direction);

        const Map& getMapById(size_t id) const;
    };
    std::stack<Shot> _shots;
    Shot _oriShot;
    void _push_shot(const Shot& shot);


   public:
    MapManager();
    MapManager(const Shot& oriShot);

    const std::vector<Map>& getMaps() const;
    const std::vector<POI>& getPois() const;
    BlockPosition getPlayerPos() const;
    const Map& getMapById(size_t id) const;
    Map& getMapById(size_t id);

    size_t addNewMap(size_t row, size_t column);
    void setPlayerPos(BlockPosition pos);
    void setBlock(BlockPosition pos, Block block);
    void setBlock(BlockPosition pos, const BlockType& blockType);
    void setMapBlockPos(BlockPosition pos, size_t map_id);

    bool movePlayer(Direction md);
    bool cancel();
    void reset();
    void changeOriShot();
};

}  // namespace PushBoxes
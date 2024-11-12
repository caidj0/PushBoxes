#pragma once
#include <cstddef>
#include <stack>
#include <vector>

#include "Block.h"

namespace PushBoxes {

class Map {
   public:
    size_t row, column;
    std::vector<std::vector<PushBoxes::Block>> blocks;
    size_t id;
    Map(size_t row, size_t column);
};

struct BlockPosition {
    size_t x, y;
    Map* map;
};

struct POI {
    BlockPosition pos;
    POIType type;
};

class MapManager {
   private:
    bool _move(BlockPosition &pos, Direction md, bool changePos);
    struct Shot {
        std::vector<Map> maps;
        std::vector<POI> pois;
        PushBoxes::BlockPosition playerPos;
        Map& findMapById(size_t id);
        Block& getBlockByPos(BlockPosition pos);
        BlockPosition getNearByBlock(BlockPosition pos, Direction md);
    };

   public:
    std::stack<Shot> old_shots;
    std::vector<Map> maps;
    std::vector<POI> pois;
    PushBoxes::BlockPosition playerPos;

    Map& addNewMap(size_t row, size_t column);
    bool movePlayer(Direction md);
    void setPlayerPos(size_t x, size_t y, Map* map);
    void addBox(size_t x, size_t y, Map* map);
    bool cancel();
};


}  // namespace PushBoxes
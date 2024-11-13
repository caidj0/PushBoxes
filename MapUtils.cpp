#include "MapUtils.h"
#include <cstddef>
#include <string>
#include "Blocks.h"

namespace PushBoxes {

namespace Utils {

void CloseMap(MapManager& mm, std::string map_id) {
    const Map& map = mm.getMapById(map_id);
    for(size_t i = 0;i < map.column;i++) {
        mm.setBlock({0,i,map_id}, WALL_BLOCK);
        mm.setBlock({map.row - 1,i,map_id}, WALL_BLOCK);
    }
    for(size_t i = 0;i < map.row;i++) {
        mm.setBlock({i,0,map_id}, WALL_BLOCK);
        mm.setBlock({i,map.column - 1,map_id}, WALL_BLOCK);
    }
}

}

}
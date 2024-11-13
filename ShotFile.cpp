#include "ShotFile.h"

#include <cstddef>
#include <fstream>
#include <stdexcept>
#include <string>

#include "Block.h"
#include "Blocks.h"
#include "Exceptions.h"
#include "Map.h"

namespace PushBoxes {

MapManager::Shot ShotFile::read(std::string path) {
    std::ifstream f;
    f.open(path);
    if (!f.is_open()) throw FileExcption();
    MapManager::Shot shot;
    size_t map_num, poi_num;
    f >> map_num >> poi_num;
    for (size_t i = 0; i < map_num; i++) {
        size_t row, column;
        std::string id;
        f >> row >> column >> id;
        shot.addNewMap(row, column, id);
        Map& map = shot.maps.back();
        std::string str;
        for (size_t x = 0; x < row; x++) {
            for (size_t y = 0; y < column; y++) {
                f >> str;
                if (str[0] == '[' && str[str.length() - 1] == ']') {
                    map.blocks[x][y] =
                        Block(MAP_BLOCK, id,
                              str.substr(0, str.length() - 1).substr(1));
                } else {
                    map.blocks[x][y].setBlockType(getBlockByName(str));
                    if (map.blocks[x][y].getBlockType() == PLAYER_BLOCK) {
                        shot.playerPos.x = x;
                        shot.playerPos.y = y;
                        shot.playerPos.map_id = id;
                    }
                }
            }
        }
    }
    for (size_t i = 0; i < poi_num; i++) {
        std::string type;
        shot.pois.emplace_back();
        POI& poi = shot.pois.back();
        f >> type;
        if (type == "block") {
            poi.type = NEEDBLOCK;
        } else if (type == "player") {
            poi.type = NEEDPLAYER;
        }
        f >> poi.pos.x >> poi.pos.y >> poi.pos.map_id;
    }
    return shot;
}

bool ShotFile::write(std::string path, const MapManager::Shot& shot) {
    std::ofstream f;
    f.open(path);
    if (!f.is_open()) return false;
    f << shot.maps.size() << " " << shot.pois.size() << "\n";
    for (const auto& map : shot.maps) {
        f << map.row << " " << map.column << " " << map.id << "\n";
        for (size_t x = 0; x < map.row; x++) {
            for (size_t y = 0; y < map.column; y++) {
                if (map.blocks[x][y].getBlockType() == MAP_BLOCK) {
                    f << "[" << map.blocks[x][y].inner_map_id << "] ";
                } else {
                    f << map.blocks[x][y].getBlockType().name << " ";
                }
            }
            f << "\n";
        }
    }

    for (const auto& poi : shot.pois) {
        if (poi.type == NEEDBLOCK) {
            f << "block ";
        } else if (poi.type == NEEDPLAYER) {
            f << "player ";
        }
        f << poi.pos.x << " " << poi.pos.y << " " << poi.pos.map_id << "\n";
    }
    f.close();
    return true;
}

}  // namespace PushBoxes
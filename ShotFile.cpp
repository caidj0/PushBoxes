#include "ShotFile.h"

#include <cstddef>
#include <fstream>
#include <map>
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
    size_t map_num;
    f >> map_num;
    std::map<std::string, BlockPosition> map_pos;
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
                if ((str[0] == '[' && str[str.length() - 1] == ']') ||
                    (str[0] == ']' && str[str.length() - 1] == '[')) {
                    std::string map_block_id =
                        str.substr(0, str.length() - 1).substr(1);
                    map.blocks[x][y] = Block(MAP_BLOCK, map_block_id);
                    map_pos[map_block_id] = {x, y, id};
                    map.blocks[x][y].isFliped = (str[0] == ']');
                } else if ((str[0] == '(' && str[str.length() - 1] == ')') ||
                           (str[0] == ')' && str[str.length() - 1] == '(')) {
                    std::string map_block_id =
                        str.substr(0, str.length() - 1).substr(1);
                    map.blocks[x][y] = Block(CLONE_BLOCK, map_block_id);
                    map.blocks[x][y].isFliped = (str[0] == ')');
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
        size_t poi_num;
        f >> poi_num;
        for (size_t i = 0; i < poi_num; i++) {
            std::string type;
            size_t poi_x, poi_y;
            POIType poiType;
            f >> type;
            if (type == "block") {
                poiType = NEEDBLOCK;
            } else if (type == "player") {
                poiType = NEEDPLAYER;
            }
            f >> poi_x >> poi_y;
            map.pois.insert({{poi_x, poi_y}, poiType});
        }
    }

    for (auto& map : shot.maps) {
        auto t = map_pos.find(map.id);
        if (t != map_pos.end()) {
            map.isInMap = 1;
            map.pos = t->second;
        }
    }

    return shot;
}

bool ShotFile::write(std::string path, const MapManager::Shot& shot) {
    std::ofstream f;
    f.open(path);
    if (!f.is_open()) return false;
    f << shot.maps.size() << "\n";
    for (const auto& map : shot.maps) {
        f << map.row << " " << map.column << " " << map.id << "\n";
        for (size_t x = 0; x < map.row; x++) {
            for (size_t y = 0; y < map.column; y++) {
                if (map.blocks[x][y].getBlockType() == MAP_BLOCK) {
                    if (map.blocks[x][y].isFliped)
                        f << "]" << map.blocks[x][y].inner_map_id << "[ ";
                    else
                        f << "[" << map.blocks[x][y].inner_map_id << "] ";
                } else if (map.blocks[x][y].getBlockType() == CLONE_BLOCK) {
                    if (map.blocks[x][y].isFliped)
                        f << "(" << map.blocks[x][y].inner_map_id << ") ";
                    else
                        f << ")" << map.blocks[x][y].inner_map_id << "( ";
                } else {
                    f << map.blocks[x][y].getBlockType().name << " ";
                }
            }
            f << "\n";
        }
        f << map.pois.size() << "\n";
        for (const auto& poi : map.pois) {
            if (poi.second == NEEDBLOCK) {
                f << "block ";
            } else if (poi.second == NEEDPLAYER) {
                f << "player ";
            }
            f << poi.first.first << " " << poi.first.second << "\n";
        }
    }

    f.close();
    return true;
}

}  // namespace PushBoxes
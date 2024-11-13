#pragma once

#include <string>

#include "Map.h"
namespace PushBoxes {

/*
    The structure of .sf file:
    mapNum poiNum
    For everymap, row, column, id, blocks
    For everypoi, posm, type
 */

class ShotFile {
   private:
    static MapManager::Shot read(std::string path);
    static bool write(std::string path, const MapManager::Shot& shot);

    friend MapManager;
};

}  // namespace PushBoxes
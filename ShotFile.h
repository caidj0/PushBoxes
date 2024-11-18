#pragma once

#include <string>

#include "Map.h"
namespace PushBoxes {

class ShotFile {
   private:
    static MapManager::Shot read(std::string path);
    static bool write(std::string path, const MapManager::Shot& shot);

    friend MapManager;
};

}  // namespace PushBoxes
#include <string>
#include <unistd.h>

#include "Block.h"
#include "Blocks.h"
#include "Map.h"
#include "Screen.h"

PushBoxes::MapManager mapManager;

void init1() {
    std::string map_id1 = mapManager.addNewMap(10, 10);
    std::string map_id2 = mapManager.addNewMap(20, 40);
    // PushBoxes::Utils::CloseMap(mapManager, map_id1);

    mapManager.setPlayerPos({2, 2, map_id1});

    mapManager.setBlock({5, 5, map_id1}, PushBoxes::WALL_BLOCK);
    mapManager.setBlock({5, 6, map_id1}, PushBoxes::BOX_BLOCK);
    mapManager.setBlock({5, 7, map_id1}, PushBoxes::BOX_BLOCK);
    mapManager.setBlock({5, 8, map_id1}, PushBoxes::BOX_BLOCK);
    mapManager.setBlock({6, 6, map_id1}, PushBoxes::BOX_BLOCK);
    mapManager.setBlock({6, 7, map_id1}, PushBoxes::BOX_BLOCK);
    mapManager.setBlock({6, 8, map_id1}, PushBoxes::BOX_BLOCK);
    mapManager.setBlock({4, 6, map_id1}, PushBoxes::BOX_BLOCK);

    // PushBoxes::Utils::CloseMap(mapManager, map_id2);

    mapManager.setBlock({10, 10, map_id2}, PushBoxes::WALL_BLOCK);

    mapManager.setMapBlockPos({3, 3, map_id1}, map_id2);
    mapManager.setMapBlockPos({3, 3, map_id2}, map_id1);

    mapManager.changeOriShot();

    // spin();

    mapManager.saveShot("a.sf");
}

void spin() {
    while (1) {
        PushBoxes::printMaps(mapManager);

        PushBoxes::Key key = PushBoxes::getKey();
        if (key == PushBoxes::K_QUIT) return;

        if (key == PushBoxes::K_DOWN)
            mapManager.movePlayer(PushBoxes::DOWN);
        else if (key == PushBoxes::K_UP)
            mapManager.movePlayer(PushBoxes::UP);
        else if (key == PushBoxes::K_LEFT)
            mapManager.movePlayer(PushBoxes::LEFT);
        else if (key == PushBoxes::K_RIGHT)
            mapManager.movePlayer(PushBoxes::RIGHT);
        else if (key == PushBoxes::K_CANCEL)
            mapManager.undo();
        else if (key == PushBoxes::K_RESET)
            mapManager.reset();
        usleep(16666);
    }
}

int main(int, char**) {
    PushBoxes::screenInit();

    // init1();
    mapManager.readShot("c.sf");

    spin();

    PushBoxes::screenDestroy();
}

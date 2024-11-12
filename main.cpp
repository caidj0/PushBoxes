#include "Block.h"
#include "Blocks.h"
#include "Map.h"
#include "Screen.h"

PushBoxes::MapManager mapManager;

void spin() {
    while(1) {
        PushBoxes::printMaps(mapManager);

        PushBoxes::Key key = PushBoxes::getKey();
        if(key == PushBoxes::K_QUIT) 
            return;

        if(key == PushBoxes::K_DOWN)
            mapManager.movePlayer(PushBoxes::DOWN);
        else if(key == PushBoxes::K_UP)
            mapManager.movePlayer(PushBoxes::UP);
        else if(key == PushBoxes::K_LEFT)
            mapManager.movePlayer(PushBoxes::LEFT);
        else if(key == PushBoxes::K_RIGHT)
            mapManager.movePlayer(PushBoxes::RIGHT);
    }
}

int main(int, char**){
    PushBoxes::screenInit();

    PushBoxes::Map& m = mapManager.addNewMap(5, 5);
    for(int i = 0;i < 5;i++) {
        m.blocks[0][i] = PushBoxes::WALL_BLOCK;
        m.blocks[4][i] = PushBoxes::WALL_BLOCK;
        m.blocks[i][0] = PushBoxes::WALL_BLOCK;
        m.blocks[i][4] = PushBoxes::WALL_BLOCK;
    }

    mapManager.setPlayerPos(2, 2, &m);
    mapManager.addBox(1, 2, &m);
    spin();

    PushBoxes::screenDestroy();
}

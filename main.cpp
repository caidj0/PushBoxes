#include <unistd.h>

#include <string>

#include "Block.h"
#include "Blocks.h"
#include "Map.h"
#include "Screen.h"

PushBoxes::MapManager mapManager;

void spin() {
    while (1) {
        PushBoxes::printMaps(mapManager);

        PushBoxes::Key key = PushBoxes::getKey();
        if (key == PushBoxes::K_QUIT) return;

        if (key == PushBoxes::K_CANCEL)
            mapManager.undo();
        else if (key == PushBoxes::K_RESET)
            mapManager.reset();
        else if (!mapManager.isWin) {
            if (key == PushBoxes::K_DOWN)
                mapManager.movePlayer(PushBoxes::DOWN);
            else if (key == PushBoxes::K_UP)
                mapManager.movePlayer(PushBoxes::UP);
            else if (key == PushBoxes::K_LEFT)
                mapManager.movePlayer(PushBoxes::LEFT);
            else if (key == PushBoxes::K_RIGHT)
                mapManager.movePlayer(PushBoxes::RIGHT);
        }
        usleep(16666);
    }
}

int main(int, char**) {
    PushBoxes::screenInit();

    mapManager.readShot("Player_17.sf");

    spin();

    PushBoxes::screenDestroy();
}

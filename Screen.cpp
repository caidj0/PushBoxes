#include "Screen.h"

#include <curses.h>

#include <cstddef>
#include <list>
#include <string>

#include "Block.h"
#include "Map.h"

namespace PushBoxes {

void screenInit() {
    gametime = 0;
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, true);

    refresh();

    nodelay(stdscr, 1);
}

Key getKey() {
    gametime++;
    int key = getch();
    for (auto& x : KeyMap) {
        if (x.first == key) return x.second;
    }
    return K_NONE;
}

void screenDestroy() { endwin(); }

void printMaps(const MapManager& mm) {
    clear();

    size_t x = 0, y = 0;

    for (const auto& t : mm.getMaps()) {
        printMapAt(x, y, t);
        x += t.column + 3;
    }

    if(mm.isWin) {
        attron(A_STANDOUT);
        attron(A_BOLD);
        mvaddstr(0, 0, "You win!");
        attroff(A_BOLD);
        attroff(A_STANDOUT);
    }
}

void printMapAt(int x, int y, const Map& map) {
    for (int i = 0; i < map.row; i++) {
        for (int j = 0; j < map.column; j++) {
            if (map.blocks[i][j].getVisualMode().canBeCovered &&
                map.pois.count({i, j})) {
                if (map.pois.at({i, j}) == NEEDBLOCK) {
                    mvaddch(y + i, x + j, '_');
                } else {
                    mvaddch(y + i, x + j, '=');
                }
            } else {
                if (map.blocks[i][j].getVisualMode().isHighlight)
                    attron(A_STANDOUT);
                mvaddch(y + i, x + j, map.blocks[i][j].getViewChar(gametime));
                attroff(A_STANDOUT);
            }
        }
    }

    std::string str = "Map ";
    str.append(map.id);

    mvaddstr(y + map.row + 2, x + map.column / 2 - (str.length() + 1) / 2,
             str.c_str());
}

}  // namespace PushBoxes
#include "Screen.h"
#include <curses.h>
#include "Map.h"



namespace PushBoxes {

void screenInit() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, true);
    
    refresh();

}

Key getKey() {
    int key;
    while (true) {
        key = getch();
        for(auto &x : KeyMap) {
            if(x.first == key)
                return x.second;
        }
    }
}

void screenDestroy() {
    endwin();
}

void printMaps(const MapManager& mm) {
    for(const auto& x : mm.maps) {
        printMapAt(0, 0, x);
    }
    // refresh();
}

void printMapAt(int x, int y, const Map& map) {
    for(int i = 0;i < map.row;i++) {
        for(int j = 0;j < map.column;j++) {
            mvaddch(x + i, y + j, map.blocks[i][j].viewChar);
        }
    }
}

}
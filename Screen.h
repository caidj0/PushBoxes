#pragma once
#include <curses.h>

#include <utility>
#include "Map.h"

namespace PushBoxes {

enum Key { K_UP, K_DOWN, K_LEFT, K_RIGHT, K_QUIT, K_CANCEL, K_RESET };

const std::pair<int, Key> KeyMap[] = {
    {KEY_UP, K_UP},       {KEY_DOWN, K_DOWN}, {KEY_LEFT, K_LEFT},
    {KEY_RIGHT, K_RIGHT}, {'q', K_QUIT},      {'z', K_CANCEL},
    {'r', K_RESET}};

void screenInit();
Key getKey();
void screenDestroy();

void printMaps(const MapManager& mm);
void printMapAt(int x, int y, const Map& map);

}  // namespace PushBoxes
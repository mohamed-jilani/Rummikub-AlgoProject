// src/core/tile.c
#include <stdio.h>
#include "tile.h"

Tile createTile(int value, TileColor color) {
    Tile tile;
    tile.value = value;
    tile.color = color;
    tile.isJoker = (value == 0) ? 1 : 0;
    return tile;
}

Tile createJoker() {
    return createTile(0, NO_COLOR);
}

int isTileValid(Tile tile) {
    if (tile.isJoker) return 1;
    return (tile.value >= 1 && tile.value <= 13);
}

int isTileEqual(Tile t1, Tile t2) {
    if (t1.isJoker && t2.isJoker) return 1;
    if (t1.isJoker != t2.isJoker) return 0;
    return (t1.value == t2.value && t1.color == t2.color);
}

void printTile(Tile tile) {
    if (tile.isJoker) {
        printf("[JOKER]");
    } else {
        char colorChar;
        switch(tile.color) {
            case RED: colorChar = 'R'; break;
            case BLUE: colorChar = 'B'; break;
            case BLACK: colorChar = 'N'; break;
            case YELLOW: colorChar = 'J'; break;
            default: colorChar = '?';
        }
        printf("[%d%c]", tile.value, colorChar);
    }
}
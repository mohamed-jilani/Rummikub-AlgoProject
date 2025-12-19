// src/core/tile.c
#include <stdio.h>
#include "tile.h"

Tile createTile(int value, Color color) {
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
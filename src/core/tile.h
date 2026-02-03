// src/core/tile.h
#ifndef TILE_H
#define TILE_H

typedef enum {
    RED,
    BLUE,
    BLACK,
    YELLOW,
    NO_COLOR  // Pour les jokers
} TileColor;

typedef struct {
    int value;      // 1-13, 0 pour joker
    TileColor color;    // Couleur de la tuile
    int isJoker;    // 1 si c'est un joker, 0 sinon
} Tile;

// Fonctions de base
Tile createTile(int value, TileColor color);
Tile createJoker();
int isTileValid(Tile tile);
int isTileEqual(Tile t1, Tile t2);
void printTile(Tile tile);  // Pour debug en console

#endif
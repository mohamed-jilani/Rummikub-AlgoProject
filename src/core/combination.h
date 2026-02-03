// src/core/combination.h
#ifndef COMBINATION_H
#define COMBINATION_H

#include "tile.h"

#define MAX_COMBINATION_SIZE 20

typedef struct {
    Tile tiles[MAX_COMBINATION_SIZE];
    int size;
    int isValid;  // 1 si combinaison valide
} Combination;

// Fonctions combinaison
void initCombination(Combination* combo);
void addTileToCombination(Combination* combo, Tile tile);
void removeTileFromCombination(Combination* combo, int index);
void printCombination(Combination* combo);

#endif
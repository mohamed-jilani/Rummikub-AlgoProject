// src/core/combination.c
#include <stdio.h>
#include "combination.h"

void initCombination(Combination* combo) {
    combo->size = 0;
    combo->isValid = 0;
    
    // Initialiser toutes les tuiles à vide
    for(int i = 0; i < MAX_COMBINATION_SIZE; i++) {
        combo->tiles[i] = createTile(-1, NO_COLOR);
    }
}

void addTileToCombination(Combination* combo, Tile tile) {
    if(combo->size < MAX_COMBINATION_SIZE) {
        combo->tiles[combo->size] = tile;
        combo->size++;
    }
}

void removeTileFromCombination(Combination* combo, int index) {
    if(index < 0 || index >= combo->size) return;
    
    // Décaler les tuiles
    for(int i = index; i < combo->size - 1; i++) {
        combo->tiles[i] = combo->tiles[i + 1];
    }
    
    combo->size--;
}

void printCombination(Combination* combo) {
    printf("[");
    for(int i = 0; i < combo->size; i++) {
        printTile(combo->tiles[i]);
        if(i < combo->size - 1) printf(" ");
    }
    printf("]");
    
    if(combo->isValid) {
        printf(" (valide)");
    } else {
        printf(" (invalide)");
    }
    printf("\n");
}
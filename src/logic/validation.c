// src/logic/validation.c
#include <stdio.h>
#include "validation.h"

int isValidSeries(Tile tiles[], int size) {
    if(size < 3) return 0;
    
    // Vérifier que toutes les valeurs sont identiques
    int referenceValue = -1;
    for(int i = 0; i < size; i++) {
        if(tiles[i].isJoker) continue;
        if(referenceValue == -1) referenceValue = tiles[i].value;
        if(tiles[i].value != referenceValue) return 0;
    }
    
    // Vérifier que les couleurs sont différentes
    for(int i = 0; i < size; i++) {
        if(tiles[i].isJoker) continue;
        for(int j = i + 1; j < size; j++) {
            if(tiles[j].isJoker) continue;
            if(tiles[i].color == tiles[j].color) return 0;
        }
    }
    
    return 1;
}

int isValidRun(Tile tiles[], int size) {
    if(size < 3) return 0;
    
    // Trier les tuiles par valeur (simplifié)
    // Note: Pour V1, on suppose que les tuiles sont déjà triées
    Color runColor = NO_COLOR;
    
    for(int i = 0; i < size; i++) {
        if(tiles[i].isJoker) continue;
        
        if(runColor == NO_COLOR) {
            runColor = tiles[i].color;
        } else if(tiles[i].color != runColor) {
            return 0;
        }
    }
    
    // Vérifier séquence (simplifié)
    int hasJoker = 0;
    for(int i = 0; i < size; i++) {
        if(tiles[i].isJoker) hasJoker = 1;
    }
    
    if(!hasJoker) {
        for(int i = 1; i < size; i++) {
            if(tiles[i].value != tiles[i-1].value + 1) return 0;
        }
    }
    
    return 1;
}

int isValidCombination(Tile tiles[], int size) {
    return isValidSeries(tiles, size) || isValidRun(tiles, size);
}

int validateCombination(Combination* combo) {
    combo->isValid = isValidCombination(combo->tiles, combo->size);
    return combo->isValid;
}
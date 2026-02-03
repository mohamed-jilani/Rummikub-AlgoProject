// src/logic/validation.c
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "validation.h"

// Fonction de comparaison pour le tri
int compareTilesForSort(const void* a, const void* b) {
    Tile* tileA = (Tile*)a;
    Tile* tileB = (Tile*)b;
    
    // Les jokers en dernier pour faciliter certains algorithmes
    if(tileA->isJoker && !tileB->isJoker) return 1;
    if(!tileA->isJoker && tileB->isJoker) return -1;
    if(tileA->isJoker && tileB->isJoker) return 0;
    
    // Pour les suites : trier par couleur puis valeur
    if(tileA->color != tileB->color) {
        return tileA->color - tileB->color;
    }
    return tileA->value - tileB->value;
}

// Trier un tableau de tuiles
void sortTiles(Tile tiles[], int size) {
    qsort(tiles, size, sizeof(Tile), compareTilesForSort);
}

bool isValidGroup(Tile tiles[], int count) {
    if (count < 3 || count > 4) {
        printf("DEBUG: Groupe invalide : nombre de tuiles (%d) incorrect (doit être 3-4)\n", count);
        return false;
    }

    printf("DEBUG: Validation Groupe : ");
    for(int i = 0; i < count; i++) {
        if (tiles[i].isJoker) printf("[JOKER] ");
        else printf("[%d %d] ", tiles[i].value, tiles[i].color);
    }
    printf("\n");

    int jokerCount = 0;
    int commonValue = -1;
    bool colorsUsed[4] = {false, false, false, false};

    for (int i = 0; i < count; i++) {
        if (tiles[i].isJoker) {
            jokerCount++;
            continue;
        }

        if (commonValue == -1) {
            commonValue = tiles[i].value;
        } else if (tiles[i].value != commonValue) {
            printf("DEBUG: Groupe invalide : valeurs différentes (%d et %d)\n", commonValue, tiles[i].value);
            return false; // All non-jokers must have same value
        }

        if (tiles[i].color < RED || tiles[i].color > YELLOW) {
             printf("DEBUG: Groupe invalide : couleur invalide (%d)\n", tiles[i].color);
             return false;
        }
        if (colorsUsed[tiles[i].color]) {
            printf("DEBUG: Groupe invalide : couleur %d déjà utilisée\n", tiles[i].color);
            return false; // Different colors required
        }
        colorsUsed[tiles[i].color] = true;
    }

    if (jokerCount > 2) {
        printf("DEBUG: Groupe invalide : trop de jokers (%d)\n", jokerCount);
        return false; // Max 2 jokers in group
    }
    if (jokerCount == count) {
        printf("DEBUG: Groupe invalide : que des jokers\n");
        return false; // Need at least one real tile
    }

    printf("DEBUG: Groupe VALIDE\n");
    return true;
}

bool isValidRun(Tile tiles[], int count) {
    if (count < 3 || count > 13) {
        printf("DEBUG: Run invalide : nombre de tuiles (%d) incorrect (doit être 3-13)\n", count);
        return false;
    }

    // Create a copy to sort
    Tile sorted[MAX_COMBINATION_SIZE];
    for (int i = 0; i < count; i++) sorted[i] = tiles[i];
    sortTiles(sorted, count);

    printf("DEBUG: Validation Suite : ");
    for(int i = 0; i < count; i++) {
        if (sorted[i].isJoker) printf("[JOKER] ");
        else printf("[%d %d] ", sorted[i].value, sorted[i].color);
    }
    printf("\n");

    int jokerCount = 0;
    TileColor runColor = NO_COLOR;

    // Find first non-joker to determine color and check color consistency
    for (int i = 0; i < count; i++) {
        if (sorted[i].isJoker) {
            jokerCount++;
            continue;
        }
        if (runColor == NO_COLOR) {
            runColor = sorted[i].color;
        } else if (sorted[i].color != runColor) {
            printf("DEBUG: Run invalide : couleurs multiples détectées (%d et %d)\n", runColor, sorted[i].color);
            return false; // Different colors in a run
        }
    }

    if (runColor == NO_COLOR) {
        printf("DEBUG: Run invalide : que des jokers\n");
        return false; // At least one real tile needed
    }

    // Check for duplicates and find min/max values of real tiles
    int minVal = 14, maxVal = 0;
    int realCount = 0;
    for (int i = 0; i < count; i++) {
        if (sorted[i].isJoker) continue;
        if (sorted[i].value < minVal) minVal = sorted[i].value;
        if (sorted[i].value > maxVal) maxVal = sorted[i].value;
        realCount++;
        
        // Check for duplicate values among real tiles
        for (int j = i + 1; j < count; j++) {
            if (!sorted[j].isJoker && sorted[j].value == sorted[i].value) {
                printf("DEBUG: Run invalide : doublon de valeur %d détecté\n", sorted[i].value);
                return false;
            }
        }
    }

    // A run of 'count' tiles must fit within a span of 'count' values.
    if (maxVal - minVal + 1 > count) {
        printf("DEBUG: Run invalide : l'écart entre %d et %d est trop grand pour %d tuiles\n", minVal, maxVal, count);
        return false;
    }

    bool possible = false;
    for (int v = 1; v <= 14 - count; v++) {
        if (v <= minVal && v + count - 1 >= maxVal) {
            possible = true;
            break;
        }
    }

    if (!possible) {
        printf("DEBUG: Run invalide : impossible de placer les tuiles dans une suite 1-13 valide\n");
    } else {
        printf("DEBUG: Run VALIDE\n");
    }

    return possible;
}

bool isValidCombination(Tile tiles[], int count) {
    return isValidGroup(tiles, count) || isValidRun(tiles, count);
}

int validateCombination(Combination* combo) {
    combo->isValid = isValidCombination(combo->tiles, combo->size);
    return combo->isValid ? 1 : 0;
}

bool validateBoard(Board* board) {
    if (board->combinationCount == 0) return false; // Rummikub board must have at least one combo if played

    for (int i = 0; i < board->combinationCount; i++) {
        Combination* combo = &board->combinations[i];
        if (combo->size == 0) continue;
        if (!isValidCombination(combo->tiles, combo->size)) return false;
    }
    return true;
}

// Vérifier si on peut former une combinaison valide
int canFormValidCombination(Tile tiles[], int size, int* comboType) {
    if(size < 3) {
        *comboType = 0;
        return 0;
    }
    
    if(isValidGroup(tiles, size)) {
        *comboType = 1;  // Série
        return 1;
    }
    
    if(isValidRun(tiles, size)) {
        *comboType = 2;  // Suite
        return 1;
    }
    
    *comboType = 0;
    return 0;
}

// Calculer les points d'une combinaison
int calculateTilePoints(Tile* tile) {
    if (tile->isJoker) return 30;
    return tile->value;
}

int calculateCombinationPoints(Tile tiles[], int size) {
    int points = 0;
    for(int i = 0; i < size; i++) {
        if (isTileValid(tiles[i])) {
            points += calculateTilePoints(&tiles[i]);
        }
    }
    return points;
}
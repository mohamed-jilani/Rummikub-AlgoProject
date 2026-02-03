// src/logic/rules.c
#include <stdio.h>
#include <stdbool.h>
#include "rules.h"
#include "validation.h"

int canPlayFirstTurn(Player* player) {
    // Pour la V1, vérification simplifiée
    // Dans la vraie version, il faut vérifier une combinaison valide de 30+ points
    int total = 0;
    for(int i = 0; i < player->tileCount; i++) {
        if(player->hand[i].isJoker) {
            total += 30;  // Joker = 30 points pour le premier tour
        } else {
            total += player->hand[i].value;
        }
    }
    return (total >= 30);
}

int isValidFirstMove(Tile tiles[], int size) {
    if(!isValidCombination(tiles, size)) return 0;
    
    int total = 0;
    for(int i = 0; i < size; i++) {
        if(tiles[i].isJoker) {
            total += 30;
        } else {
            total += tiles[i].value;
        }
    }
    
    return (total >= 30);
}

int canReplaceJoker(Tile combination[], int size, int jokerIndex, Tile replacement) {
    if(jokerIndex < 0 || jokerIndex >= size) return 0;
    if(!combination[jokerIndex].isJoker) return 0;
    
    // Créer une copie temporaire avec le remplacement
    Tile temp[13];
    for(int i = 0; i < size; i++) {
        if(i == jokerIndex) {
            temp[i] = replacement;
        } else {
            temp[i] = combination[i];
        }
    }
    
    // Vérifier si la combinaison reste valide
    return isValidCombination(temp, size);
}

int getJokerValue(Tile combination[], int size, int jokerIndex) {
    if (jokerIndex < 0 || jokerIndex >= size || !combination[jokerIndex].isJoker) return 0;

    // Check if it's a group (all non-jokers same value)
    int commonValue = -1;
    bool isRun = false;
    TileColor runColor = NO_COLOR;

    for (int i = 0; i < size; i++) {
        if (combination[i].isJoker) continue;
        if (commonValue == -1) commonValue = combination[i].value;
        else if (combination[i].value != commonValue) isRun = true;
        
        if (runColor == NO_COLOR) runColor = combination[i].color;
    }

    if (!isRun && commonValue != -1) return commonValue;

    if (isRun) {
        // It's a run. We need to find the value based on position.
        // Sort non-joker tiles to find a reference.
        int firstRealValue = -1;
        int firstRealPos = -1;
        for (int i = 0; i < size; i++) {
            if (!combination[i].isJoker) {
                firstRealValue = combination[i].value;
                firstRealPos = i;
                break;
            }
        }
        if (firstRealValue != -1) {
            return firstRealValue - (firstRealPos - jokerIndex);
        }
    }

    return 0;
}

int canSplitCombination(Tile combination[], int size, int splitIndex) {
    if (splitIndex < 3 || (size - splitIndex) < 3) return 0;
    
    bool part1 = isValidCombination(combination, splitIndex);
    bool part2 = isValidCombination(combination + splitIndex, size - splitIndex);
    
    return (part1 && part2);
}

int canAddTileToExisting(Tile combination[], int size, Tile newTile) {
    Tile temp[MAX_COMBINATION_SIZE + 1];
    for (int i = 0; i < size; i++) temp[i] = combination[i];
    temp[size] = newTile;
    
    // Check if it's valid at start or end
    if (isValidCombination(temp, size + 1)) return 1;
    
    // Check if it's valid if inserted at start
    for (int i = size; i > 0; i--) temp[i] = temp[i-1];
    temp[0] = newTile;
    if (isValidCombination(temp, size + 1)) return 1;
    
    return 0;
}
// src/logic/rules.c
#include <stdio.h>
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
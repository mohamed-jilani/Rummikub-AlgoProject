// src/data/save_manager.c
#include "save_manager.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int saveGameState(const GameState* game) {
    FILE* file = fopen(SAVE_FILE, "wb");
    if(file == NULL) {
        return 0;
    }
    
    // Sauvegarder l'état du jeu
    fwrite(game, sizeof(GameState), 1, file);
    
    // Sauvegarder les tuiles du deck
    fwrite(&game->board.deck, sizeof(Tile), DECK_SIZE, file);
    
    // Sauvegarder les combinaisons du plateau
    fwrite(&game->board.combinationCount, sizeof(int), 1, file);
    for(int i = 0; i < game->board.combinationCount; i++) {
        fwrite(&game->board.combinations[i], sizeof(Combination), 1, file);
    }
    
    fclose(file);
    return 1;
}

int loadGameState(GameState* game) {
    FILE* file = fopen(SAVE_FILE, "rb");
    if(file == NULL) {
        return 0;
    }
    
    // Charger l'état du jeu
    if(fread(game, sizeof(GameState), 1, file) != 1) {
        fclose(file);
        return 0;
    }
    
    // Charger les tuiles du deck
    if(fread(&game->board.deck, sizeof(Tile), DECK_SIZE, file) != DECK_SIZE) {
        fclose(file);
        return 0;
    }
    
    // Charger les combinaisons du plateau
    int comboCount;
    if(fread(&comboCount, sizeof(int), 1, file) != 1) {
        fclose(file);
        return 0;
    }
    
    game->board.combinationCount = comboCount;
    for(int i = 0; i < comboCount; i++) {
        if(fread(&game->board.combinations[i], sizeof(Combination), 1, file) != 1) {
            fclose(file);
            return 0;
        }
    }
    
    fclose(file);
    return 1;
}

int hasSaveFile() {
    FILE* file = fopen(SAVE_FILE, "rb");
    if(file != NULL) {
        fclose(file);
        return 1;
    }
    return 0;
}




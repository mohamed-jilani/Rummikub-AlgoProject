// src/core/game.c
#include <stdio.h>
#include "game.h"
#include "../logic/validation.h"
#include "../logic/rules.h"

void initGame(GameState* game, int playerCount, const char** names, int* isAI) {
    game->playerCount = playerCount;
    game->currentPlayer = 0;
    game->phase = SETUP;
    
    initBoard(&game->board);
    game->tilesInDeck = DECK_SIZE;
    
    for(int i = 0; i < playerCount; i++) {
        initPlayer(&game->players[i], names[i], isAI[i]);
    }
    
    dealInitialTiles(game);
    game->phase = FIRST_MOVE;
}

void dealInitialTiles(GameState* game) {
    for(int i = 0; i < game->playerCount; i++) {
        for(int j = 0; j < 14; j++) {
            Tile tile = drawTile(&game->board);
            addTileToHand(&game->players[i], tile);
            game->tilesInDeck--;
        }
    }
}

void nextPlayer(GameState* game) {
    game->currentPlayer = (game->currentPlayer + 1) % game->playerCount;
}

int isGameOver(GameState* game) {
    for(int i = 0; i < game->playerCount; i++) {
        if(game->players[i].tileCount == 0) {
            return 1;
        }
    }
    return (game->tilesInDeck == 0);
}

void printGameState(GameState* game) {
    printf("\n=== ÉTAT DU JEU ===\n");
    printf("Phase: %d\n", game->phase);
    printf("Joueur actuel: %s\n", game->players[game->currentPlayer].name);
    printf("Tuiles restantes: %d\n", game->tilesInDeck);
    
    for(int i = 0; i < game->playerCount; i++) {
        printPlayerHand(&game->players[i]);
    }
    
    printBoard(&game->board);
    printf("===================\n");
}

// Dans game.c - Ajouter ces fonctions
int canPlayerPlay(GameState* game, int playerIndex) {
    Player* player = &game->players[playerIndex];
    
    if(game->phase == FIRST_MOVE) {
        return canPlayFirstTurn(player);
    }
    
    return 1;
}

void playerDrawTile(GameState* game, int playerIndex) {
    if(game->tilesInDeck > 0) {
        Tile tile = drawTile(&game->board);
        addTileToHand(&game->players[playerIndex], tile);
        game->tilesInDeck--;
    }
}

int playerPlayCombination(GameState* game, int playerIndex, Combination combo) {
    (void)playerIndex;  // Éviter l'avertissement unused parameter
    // Vérifier la validation
    if(validateCombination(&combo)) {
        // Pour V1, on accepte sans vérifier si le joueur a les tuiles
        addCombinationToBoard(&game->board, combo);
        
        // Changer de phase si premier tour réussi
        if(game->phase == FIRST_MOVE) {
            game->phase = MAIN_GAME;
        }
        
        return 1;
    }
    return 0;
}
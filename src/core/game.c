// src/core/game.c
#include <stdio.h>
#include "game.h"
#include "../logic/validation.h"
#include "../logic/rules.h"

void initGame(GameState* game, int playerCount, const char** names, bool* isComputer, AILevel* levels) {
    game->playerCount = playerCount;
    game->currentPlayer = 0;
    game->phase = SETUP;
    
    initBoard(&game->board);
    game->tilesInDeck = DECK_SIZE;
    
    // Initialiser le système de tour
    game->moves_in_current_turn = 0;
    game->has_drawn_this_turn = 0;
    game->has_played_tile_this_turn = 0;
    
    for(int i = 0; i < playerCount; i++) {
        initPlayer(&game->players[i], names[i], isComputer[i], levels[i]);
        game->aiLevels[i] = levels[i]; // Garder pour le GameState si besoin
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
    // Vérifier si le joueur actuel a un joker à rejouer (ne peut pas passer son tour)
    if (game->hasJokerInHand[game->currentPlayer]) {
        // Le joueur doit rejouer le joker avant de passer son tour
        // Pour l'instant, on permet le passage mais on réinitialise le joker
        game->hasJokerInHand[game->currentPlayer] = 0;
        game->temporaryJoker[game->currentPlayer] = createTile(-1, NO_COLOR);
    }
    
    // Réinitialiser les compteurs de tour pour le nouveau joueur
    game->moves_in_current_turn = 0;
    game->has_drawn_this_turn = 0;
    game->has_played_tile_this_turn = 0;
    
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

// Vérifier si le joueur possède les tuiles de la combinaison
int playerHasTiles(Player* player, Combination* combo) {
    // Créer une copie de la main pour vérifier
    Tile handCopy[MAX_TILES];
    int handCount = player->tileCount;
    for(int i = 0; i < handCount; i++) {
        handCopy[i] = player->hand[i];
    }
    
    // Pour chaque tuile de la combinaison, vérifier si elle existe dans la main
    for(int i = 0; i < combo->size; i++) {
        int found = 0;
        Tile comboTile = combo->tiles[i];
        
        for(int j = 0; j < handCount; j++) {
            if(handCopy[j].isJoker && comboTile.isJoker) {
                // Marquer comme utilisé
                handCopy[j] = createTile(-1, NO_COLOR);
                found = 1;
                break;
            } else if(!handCopy[j].isJoker && !comboTile.isJoker &&
                     handCopy[j].value == comboTile.value &&
                     handCopy[j].color == comboTile.color) {
                // Marquer comme utilisé
                handCopy[j] = createTile(-1, NO_COLOR);
                found = 1;
                break;
            }
        }
        
        if(!found) {
            return 0;  // Tuile non trouvée
        }
    }
    
    return 1;
}

// Retirer les tuiles de la main du joueur
void removeTilesFromHand(Player* player, Combination* combo) {
    // Pour chaque tuile de la combinaison
    for(int i = 0; i < combo->size; i++) {
        Tile comboTile = combo->tiles[i];
        
        // Trouver et retirer la tuile de la main
        for(int j = 0; j < player->tileCount; j++) {
            Tile handTile = player->hand[j];
            
            if(comboTile.isJoker && handTile.isJoker) {
                removeTileFromHand(player, j);
                break;
            } else if(!comboTile.isJoker && !handTile.isJoker &&
                     handTile.value == comboTile.value &&
                     handTile.color == comboTile.color) {
                removeTileFromHand(player, j);
                break;
            }
        }
    }
}

int playerPlayCombination(GameState* game, int playerIndex, Combination combo) {
    Player* player = &game->players[playerIndex];
    
    // Vérifier que le joueur a les tuiles
    if(!playerHasTiles(player, &combo)) {
        return 0;  // Le joueur n'a pas les tuiles
    }
    
    // Vérifier la validation de la combinaison
    if(!validateCombination(&combo)) {
        return 0;  // Combinaison invalide
    }
    
    // Vérifier le premier tour (minimum 30 points)
    if(game->phase == FIRST_MOVE) {
        int points = calculateCombinationPoints(combo.tiles, combo.size);
        if(points < 30) {
            return 0;  // Pas assez de points pour le premier tour
        }
    }
    
    // Retirer les tuiles de la main
    removeTilesFromHand(player, &combo);
    
    // Ajouter la combinaison au plateau
    addCombinationToBoard(&game->board, combo);
    
    // Changer de phase si premier tour réussi
    if(game->phase == FIRST_MOVE) {
        game->phase = MAIN_GAME;
    }
    
    return 1;
}
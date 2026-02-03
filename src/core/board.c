// src/core/board.c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "board.h"
#include "../utils/random.h"
#include "../logic/validation.h"

void initBoard(Board* board) {
    board->combinationCount = 0;
    board->deckIndex = 0;
    createDeck(board);
    shuffleDeck(board);
}

void createDeck(Board* board) {
    int index = 0;
    
    // Créer 2 exemplaires de chaque valeur/couleur (104 tuiles)
    for(int value = 1; value <= 13; value++) {
        for(TileColor color = RED; color <= YELLOW; color++) {
            for(int copy = 0; copy < 2; copy++) {
                board->deck[index++] = createTile(value, color);
            }
        }
    }
    
    // Ajouter 2 jokers
    board->deck[index++] = createJoker();
    board->deck[index++] = createJoker();
}

void shuffleDeck(Board* board) {
    srand(time(NULL));
    for(int i = DECK_SIZE - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Tile temp = board->deck[i];
        board->deck[i] = board->deck[j];
        board->deck[j] = temp;
    }
}

Tile drawTile(Board* board) {
    if(board->deckIndex < DECK_SIZE) {
        return board->deck[board->deckIndex++];
    }
    return createTile(-1, NO_COLOR); // Deck vide
}

void addCombinationToBoard(Board* board, Combination combo) {
    if(board->combinationCount < MAX_COMBINATIONS) {
        board->combinations[board->combinationCount] = combo;
        board->combinationCount++;
    }
}

void printBoard(Board* board) {
    printf("=== PLATEAU ===\n");
    for(int i = 0; i < board->combinationCount; i++) {
        printf("Combinaison %d: ", i+1);
        printCombination(&board->combinations[i]);
    }
    printf("===============\n");
}

// Ajouter une tuile à une position spécifique (row, col)
// Chaque ligne correspond à une combinaison potentielle dans le board logique
int board_add_tile(Board* board, Tile tile, int row, int col) {
    if (row < 0 || row >= MAX_COMBINATIONS || col < 0 || col >= MAX_COMBINATION_SIZE) {
        return 0;  // Position invalide
    }
    
    // S'assurer que combinationCount couvre au moins la ligne demandée
    if (row >= board->combinationCount) {
        while (board->combinationCount <= row && board->combinationCount < MAX_COMBINATIONS) {
            initCombination(&board->combinations[board->combinationCount]);
            board->combinationCount++;
        }
    }
    
    Combination* combo = &board->combinations[row];
    
    // Placer la tuile directement à la colonne (écrase si déjà occupée)
    combo->tiles[col] = tile;
    
    // Mettre à jour la taille logique (doit inclure la colonne la plus à droite)
    if (col >= combo->size) {
        combo->size = col + 1;
    }
    
    return 1;
}

// Retirer une tuile d'une position spécifique
void board_remove_tile(Board* board, int row, int col) {
    if (row < 0 || row >= MAX_COMBINATIONS || col < 0 || col >= MAX_COMBINATION_SIZE) {
        return;
    }
    
    if (row < board->combinationCount) {
        board->combinations[row].tiles[col] = createTile(-1, NO_COLOR);
    }
}

// Valider toutes les combinaisons du board
// Retourne 1 si toutes valides, 0 sinon
int board_validate_all(Board* board) {
    for (int i = 0; i < board->combinationCount; i++) {
        Combination* combo = &board->combinations[i];
        // Ignorer les combinaisons vides
        if (combo->size == 0) {
            continue;
        }
        // Vérifier si la combinaison est valide
        if (!validateCombination(combo)) {
            return 0;  // Au moins une combinaison invalide
        }
    }
    return 1;  // Toutes les combinaisons sont valides
}

// Valider complètement le board avec règles Rummikub
int validate_entire_board(Board* board, int* errorCode) {
    if (errorCode) *errorCode = 0;
    
    int totalPoints = 0;
    int hasValidCombination = 0;
    
    // Vérifier toutes les combinaisons
    for (int i = 0; i < board->combinationCount; i++) {
        Combination* combo = &board->combinations[i];
        
        // Extraire les tuiles valides (ignorer les trous/vides)
        Tile packedTiles[MAX_COMBINATION_SIZE];
        int packedCount = 0;
        for (int j = 0; j < MAX_COMBINATION_SIZE; j++) {
            if (isTileValid(combo->tiles[j])) {
                packedTiles[packedCount++] = combo->tiles[j];
            }
        }
        
        // Ignorer les combinaisons vides
        if (packedCount == 0) {
            continue;
        }
        
        // Vérifier que la combinaison a au moins 3 tuiles
        if (packedCount < 3) {
            if (errorCode) *errorCode = 3;  // Plateau incomplet
            return 0;
        }
        
        // La combinaison packed doit être valide
        if (!isValidCombination(packedTiles, packedCount)) {
            if (errorCode) *errorCode = 1;  // Combinaison invalide
            return 0;
        }
        
        // Calculer les points de cette combinaison packed
        totalPoints += calculateCombinationPoints(packedTiles, packedCount);
        hasValidCombination = 1;
    }
    
    // Vérifier qu'il y a au moins une combinaison valide
    if (!hasValidCombination) {
        if (errorCode) *errorCode = 3;  // Plateau incomplet
        return 0;
    }
    
    return 1;  // Toutes les combinaisons sont valides
}
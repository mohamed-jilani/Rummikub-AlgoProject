// src/core/board.c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "board.h"
#include "../utils/random.h"

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
        for(Color color = RED; color <= YELLOW; color++) {
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
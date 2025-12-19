// src/core/board.h
#ifndef BOARD_H
#define BOARD_H

#include "combination.h"

#define MAX_COMBINATIONS 20
#define DECK_SIZE 106  // 104 tuiles + 2 jokers

typedef struct {
    Combination combinations[MAX_COMBINATIONS];
    int combinationCount;
    Tile deck[DECK_SIZE];
    int deckIndex;  // Index de la prochaine tuile à piocher
} Board;

// Fonctions plateau
void initBoard(Board* board);
void createDeck(Board* board);
void shuffleDeck(Board* board);
Tile drawTile(Board* board);
void addCombinationToBoard(Board* board, Combination combo);
void printBoard(Board* board);

#endif
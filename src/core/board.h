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
// Ajouter une tuile à une position spécifique (row, col)
// Retourne 1 si succès, 0 si échec
int board_add_tile(Board* board, Tile tile, int row, int col);
void board_remove_tile(Board* board, int row, int col);
// Valider toutes les combinaisons du board
// Retourne 1 si toutes valides, 0 sinon
int board_validate_all(Board* board);
// Valider complètement le board avec règles Rummikub
// Retourne 1 si valide, 0 sinon
// errorCode: 0=OK, 1=combinaison invalide, 2=premier tour <30pts, 3=plateau incomplet
int validate_entire_board(Board* board, int* errorCode);

#endif
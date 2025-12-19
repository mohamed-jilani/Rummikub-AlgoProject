// src/core/game.h
#ifndef GAME_H
#define GAME_H

#include "player.h"
#include "board.h"

#define MAX_PLAYERS 4

typedef enum {
    SETUP,
    FIRST_MOVE,  // Phase des 30 points
    MAIN_GAME,
    GAME_OVER
} GamePhase;

typedef struct {
    Player players[MAX_PLAYERS];
    Board board;
    int currentPlayer;
    GamePhase phase;
    int playerCount;
    int tilesInDeck;
} GameState;

// Fonctions jeu
void initGame(GameState* game, int playerCount, const char** names, int* isAI);
void dealInitialTiles(GameState* game);
void nextPlayer(GameState* game);
int isGameOver(GameState* game);
void printGameState(GameState* game);

#endif
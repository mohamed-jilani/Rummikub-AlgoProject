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
    AILevel aiLevels[MAX_PLAYERS];  // Niveau d'IA pour chaque joueur
    
    // Gestion des jokers pris du plateau
    int hasJokerInHand[MAX_PLAYERS];  // 1 si le joueur a un joker à rejouer, 0 sinon
    Tile temporaryJoker[MAX_PLAYERS];  // Joker à rejouer immédiatement (doit être rejoué dans le même tour)
    
    // Système de tour robuste
    int moves_in_current_turn;  // Compte des actions dans le tour actuel
    int has_drawn_this_turn;    // 1 si pioche faite ce tour, 0 sinon
    int has_played_tile_this_turn;  // 1 si tuile posée ce tour, 0 sinon
} GameState;

// Fonctions jeu
void initGame(GameState* game, int playerCount, const char** names, bool* isComputer, AILevel* levels);
void dealInitialTiles(GameState* game);
void nextPlayer(GameState* game);
int isGameOver(GameState* game);
void printGameState(GameState* game);

#endif
// src/core/player.h
#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>
#include "tile.h"

#define MAX_TILES 106  // Capacité max (tout le deck)
#define MAX_NAME 50

typedef enum {
    AI_RANDOM,
    AI_BASIC
} AILevel;

typedef struct {
    char name[MAX_NAME];
    Tile hand[MAX_TILES];
    int tileCount;
    int score;
    bool isComputer;
    AILevel aiLevel;
    int hasMadeInitialMeld; // 1 si le joueur a déjà posé ses 30 points, 0 sinon
} Player;

// Fonctions joueur
void initPlayer(Player* player, const char* name, bool isComputer, AILevel level);
void addTileToHand(Player* player, Tile tile);
Tile removeTileFromHand(Player* player, int index);
void printPlayerHand(Player* player);
int calculateHandPoints(Player* player);

#endif
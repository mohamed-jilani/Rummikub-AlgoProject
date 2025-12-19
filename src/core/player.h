// src/core/player.h
#ifndef PLAYER_H
#define PLAYER_H

#include "tile.h"

#define MAX_TILES 14
#define MAX_NAME 50

typedef struct {
    char name[MAX_NAME];
    Tile hand[MAX_TILES];
    int tileCount;
    int score;
    int isAI;  // 1 si c'est une IA, 0 si humain
} Player;

// Fonctions joueur
void initPlayer(Player* player, const char* name, int isAI);
void addTileToHand(Player* player, Tile tile);
Tile removeTileFromHand(Player* player, int index);
void printPlayerHand(Player* player);
int calculateHandPoints(Player* player);

#endif
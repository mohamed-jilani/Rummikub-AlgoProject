// src/core/player.c
#include <stdio.h>
#include <string.h>
#include "player.h"

void initPlayer(Player* player, const char* name, int isAI) {
    strncpy(player->name, name, MAX_NAME-1);
    player->name[MAX_NAME-1] = '\0';
    player->tileCount = 0;
    player->score = 0;
    player->isAI = isAI;
    
    for(int i = 0; i < MAX_TILES; i++) {
        player->hand[i] = createTile(-1, NO_COLOR); // Main vide
    }
}

void addTileToHand(Player* player, Tile tile) {
    if(player->tileCount < MAX_TILES) {
        player->hand[player->tileCount] = tile;
        player->tileCount++;
    }
}

Tile removeTileFromHand(Player* player, int index) {
    if(index < 0 || index >= player->tileCount) {
        return createTile(-1, NO_COLOR); // Tuile invalide
    }
    
    Tile removed = player->hand[index];
    
    // Décalage des tuiles
    for(int i = index; i < player->tileCount - 1; i++) {
        player->hand[i] = player->hand[i + 1];
    }
    
    player->tileCount--;
    return removed;
}

void printPlayerHand(Player* player) {
    printf("%s : ", player->name);
    for(int i = 0; i < player->tileCount; i++) {
        printTile(player->hand[i]);
        printf(" ");
    }
    printf("\n");
}

int calculateHandPoints(Player* player) {
    int points = 0;
    for(int i = 0; i < player->tileCount; i++) {
        if(player->hand[i].isJoker) {
            points += 30;
        } else {
            points += player->hand[i].value;
        }
    }
    return points;
}
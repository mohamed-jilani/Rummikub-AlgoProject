// src/logic/rules.h
#ifndef RULES_H
#define RULES_H

#include "../core/tile.h"
#include "../core/player.h"

// Règles du jeu
int canPlayFirstTurn(Player* player);  // Vérifie les 30 points
int isValidFirstMove(Tile tiles[], int size);  // Combinaison valide + 30 points

// Gestion des jokers
int canReplaceJoker(Tile combination[], int size, int jokerIndex, Tile replacement);
int getJokerValue(Tile combination[], int size, int jokerIndex);

// Manipulation du plateau
int canSplitCombination(Tile combination[], int size, int splitIndex);
int canAddTileToExisting(Tile combination[], int size, Tile newTile);

#endif
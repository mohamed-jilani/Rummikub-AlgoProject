// src/logic/validation.h
#ifndef VALIDATION_H
#define VALIDATION_H

#include <stdbool.h>
#include "../core/tile.h"
#include "../core/combination.h"
#include "../core/board.h"

// Validation des combinaisons
bool isValidGroup(Tile tiles[], int count);  // Même valeur, couleurs différentes
bool isValidRun(Tile tiles[], int count);    // Suite même couleur
bool isValidCombination(Tile tiles[], int count);
int validateCombination(Combination* combo);
bool validateBoard(Board* board);

// Utilitaires
int canAddTileToSeries(Combination* combo, Tile tile);
int canAddTileToRun(Combination* combo, Tile tile);
void sortTiles(Tile tiles[], int size);  // Trier les tuiles pour validation

// Vérification en temps réel
int canFormValidCombination(Tile tiles[], int size, int* comboType);  // 0=aucune, 1=série, 2=suite
int calculateCombinationPoints(Tile tiles[], int size);  // Calculer les points d'une combinaison
int calculateTilePoints(Tile* tile);  // Calculer les points d'une tuile (Joker = 30)

#endif
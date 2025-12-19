// src/logic/validation.h
#ifndef VALIDATION_H
#define VALIDATION_H

#include "../core/tile.h"
#include "../core/combination.h"

// Validation des combinaisons
int isValidSeries(Tile tiles[], int size);  // Même valeur, couleurs différentes
int isValidRun(Tile tiles[], int size);     // Suite même couleur
int isValidCombination(Tile tiles[], int size);
int validateCombination(Combination* combo);

// Utilitaires
int canAddTileToSeries(Combination* combo, Tile tile);
int canAddTileToRun(Combination* combo, Tile tile);

#endif
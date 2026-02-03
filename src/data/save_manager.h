// src/data/save_manager.h
#ifndef SAVE_MANAGER_H
#define SAVE_MANAGER_H

#include "../core/game.h"

#define SAVE_FILE "data/save.dat"

// Fonctions de sauvegarde/chargement
int saveGameState(const GameState* game);
int loadGameState(GameState* game);
int hasSaveFile();

#endif




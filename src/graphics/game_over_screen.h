// src/graphics/game_over_screen.h
#ifndef GAME_OVER_SCREEN_H
#define GAME_OVER_SCREEN_H

#include "../core/game.h"
#include "tile_wrapper.h"
#include "ui_components.h"

// Écran de fin de partie
typedef struct {
    GameState* game;
    int winnerIndex;
    int* playerScores;
    int* playerRanks;
    Button backToMenuButton;
    float animationTime;
    int showScores;
} GameOverScreen;

// Fonctions
void initGameOverScreen(GameOverScreen* screen, GameState* game);
void updateGameOverScreen(GameOverScreen* screen, float deltaTime);
void drawGameOverScreen(GameOverScreen* screen);
int isBackToMenuClicked(GameOverScreen* screen);
void cleanupGameOverScreen(GameOverScreen* screen);

#endif


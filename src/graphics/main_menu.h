// src/graphics/main_menu.h
#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "ui_components.h"
#include "tile_wrapper.h"
#include "../data/save_manager.h"
#include "../data/scores_manager.h"
#include "raylib_game.h"  // Pour StartMenuState

// État du menu principal
typedef enum {
    MENU_MAIN,
    MENU_NEW_GAME,
    MENU_LOAD_GAME,
    MENU_RANKING,
    MENU_QUIT
} MenuState;

typedef struct {
    MenuState currentState;
    Button newGameButton;
    Button loadGameButton;
    Button rankingButton;
    Button quitButton;
    Button backButton;
    
    // Pour le classement
    ScoresList scores;
    int showRanking;
    
    // Pour le chargement
    int hasSaveFile;
} MainMenuState;

// Fonctions du menu principal
void initMainMenu(MainMenuState* menu);
void updateMainMenu(MainMenuState* menu);
void drawMainMenu(MainMenuState* menu);
MenuState runMainMenu();

#endif


// src/graphics/main_menu.c
#include "main_menu.h"
#include "tile_wrapper.h"  // Inclut déjà raylib.h avec gestion des conflits
#include "../data/save_manager.h"
#include "../data/scores_manager.h"
#include <stdio.h>
#include <string.h>

// Les constantes WINDOW_WIDTH et WINDOW_HEIGHT sont déjà définies dans raylib_renderer.h

void initMainMenu(MainMenuState* menu) {
    menu->currentState = MENU_MAIN;
    menu->showRanking = 0;
    menu->hasSaveFile = hasSaveFile();
    
    // Positionner les boutons au centre
    int centerX = WINDOW_WIDTH / 2 - 150;
    int startY = 250;
    int spacing = 80;
    
    menu->newGameButton = createUIButton(centerX, startY, 300, 60, "Nouvelle partie");
    menu->newGameButton.normalColor = (RaylibColor){80, 150, 80, 255};
    menu->newGameButton.hoverColor = (RaylibColor){100, 180, 100, 255};
    
    menu->loadGameButton = createUIButton(centerX, startY + spacing, 300, 60, "Charger partie");
    menu->loadGameButton.normalColor = (RaylibColor){150, 150, 80, 255};
    menu->loadGameButton.hoverColor = (RaylibColor){180, 180, 100, 255};
    if(!menu->hasSaveFile) {
        menu->loadGameButton.normalColor = (RaylibColor){100, 100, 100, 255};
    }
    
    menu->rankingButton = createUIButton(centerX, startY + spacing * 2, 300, 60, "Classement");
    menu->rankingButton.normalColor = (RaylibColor){80, 80, 150, 255};
    menu->rankingButton.hoverColor = (RaylibColor){100, 100, 180, 255};
    
    menu->quitButton = createUIButton(centerX, startY + spacing * 3, 300, 60, "Quitter");
    menu->quitButton.normalColor = (RaylibColor){150, 80, 80, 255};
    menu->quitButton.hoverColor = (RaylibColor){180, 100, 100, 255};
    
    menu->backButton = createUIButton(50, 50, 150, 50, "Retour");
    menu->backButton.normalColor = (RaylibColor){100, 100, 100, 255};
    menu->backButton.hoverColor = (RaylibColor){130, 130, 130, 255};
    
    // Charger les scores pour le classement
    loadScores(&menu->scores);
}

void updateMainMenu(MainMenuState* menu) {
    updateUIButton(&menu->newGameButton);
    updateUIButton(&menu->loadGameButton);
    updateUIButton(&menu->rankingButton);
    updateUIButton(&menu->quitButton);
    updateUIButton(&menu->backButton);
    
    // Vérifier les clics
    if(menu->currentState == MENU_MAIN) {
        if(isUIButtonClicked(&menu->newGameButton)) {
            menu->currentState = MENU_NEW_GAME;
        }
        if(isUIButtonClicked(&menu->loadGameButton) && menu->hasSaveFile) {
            menu->currentState = MENU_LOAD_GAME;
        }
        if(isUIButtonClicked(&menu->rankingButton)) {
            menu->currentState = MENU_RANKING;
            menu->showRanking = 1;
            loadScores(&menu->scores);  // Recharger les scores
        }
        if(isUIButtonClicked(&menu->quitButton)) {
            menu->currentState = MENU_QUIT;
        }
    } else {
        if(isUIButtonClicked(&menu->backButton) || IsKeyPressed(KEY_ESCAPE)) {
            menu->currentState = MENU_MAIN;
            menu->showRanking = 0;
        }
    }
}

void drawMainMenu(MainMenuState* menu) {
    BeginDrawing();
    ClearBackground((RaylibColor){30, 30, 40, 255});
    
    if(menu->currentState == MENU_MAIN) {
        // Titre
        const char* title = "RUMMIKUB";
        int titleWidth = MeasureText(title, 80);
        DrawText(title, (WINDOW_WIDTH - titleWidth) / 2, 100, 80, (RaylibColor){255, 255, 255, 255});
        
        // Sous-titre
        const char* subtitle = "Menu Principal";
        int subtitleWidth = MeasureText(subtitle, 30);
        DrawText(subtitle, (WINDOW_WIDTH - subtitleWidth) / 2, 180, 30, (RaylibColor){200, 200, 200, 255});
        
        // Boutons
        drawUIButton(&menu->newGameButton);
        drawUIButton(&menu->loadGameButton);
        drawUIButton(&menu->rankingButton);
        drawUIButton(&menu->quitButton);
        
        // Message si pas de sauvegarde
        if(!menu->hasSaveFile) {
            const char* msg = "Aucune partie sauvegardée";
            int msgWidth = MeasureText(msg, 20);
            DrawText(msg, (WINDOW_WIDTH - msgWidth) / 2, 580, 20, (RaylibColor){150, 150, 150, 255});
        }
    } else if(menu->currentState == MENU_RANKING) {
        // Titre
        const char* title = "CLASSEMENT";
        int titleWidth = MeasureText(title, 60);
        DrawText(title, (WINDOW_WIDTH - titleWidth) / 2, 50, 60, (RaylibColor){255, 255, 255, 255});
        
        // Afficher le classement
        if(menu->scores.count == 0) {
            const char* msg = "Aucun score enregistré";
            int msgWidth = MeasureText(msg, 30);
            DrawText(msg, (WINDOW_WIDTH - msgWidth) / 2, 300, 30, (RaylibColor){150, 150, 150, 255});
        } else {
            // Trier les scores
            ScoresList sorted = menu->scores;
            sortScoresByScore(&sorted);
            
            int startY = 150;
            int lineHeight = 40;
            int displayCount = (sorted.count < 10) ? sorted.count : 10;
            
            // En-tête
            DrawText("Rang", 200, startY, 24, (RaylibColor){255, 255, 255, 255});
            DrawText("Nom", 300, startY, 24, (RaylibColor){255, 255, 255, 255});
            DrawText("Score", 600, startY, 24, (RaylibColor){255, 255, 255, 255});
            DrawText("Date", 700, startY, 24, (RaylibColor){255, 255, 255, 255});
            
            // Lignes
            for(int i = 0; i < displayCount; i++) {
                int y = startY + 40 + i * lineHeight;
                char rankStr[10];
                sprintf(rankStr, "%d", i + 1);
                DrawText(rankStr, 200, y, 20, (RaylibColor){255, 255, 255, 255});
                DrawText(sorted.entries[i].name, 300, y, 20, (RaylibColor){255, 255, 255, 255});
                
                char scoreStr[20];
                sprintf(scoreStr, "%d", sorted.entries[i].score);
                DrawText(scoreStr, 600, y, 20, (RaylibColor){255, 255, 255, 255});
                DrawText(sorted.entries[i].date, 700, y, 20, (RaylibColor){200, 200, 200, 255});
            }
        }
        
        drawUIButton(&menu->backButton);
    } else {
        // Autres états (chargement, etc.)
        drawUIButton(&menu->backButton);
    }
    
    EndDrawing();
}

MenuState runMainMenu() {
    if(!IsWindowReady()) {
        InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Rummikub - Menu Principal");
        SetTargetFPS(60);
    }
    
    MainMenuState menu;
    initMainMenu(&menu);
    
    while(!WindowShouldClose()) {
        updateMainMenu(&menu);
        drawMainMenu(&menu);
        
        // Retourner l'état si une action est sélectionnée
        if(menu.currentState == MENU_NEW_GAME) {
            return MENU_NEW_GAME;
        }
        if(menu.currentState == MENU_LOAD_GAME) {
            return MENU_LOAD_GAME;
        }
        if(menu.currentState == MENU_QUIT) {
            return MENU_QUIT;
        }
    }
    
    return MENU_QUIT;
}


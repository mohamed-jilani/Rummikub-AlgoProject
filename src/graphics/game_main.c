// src/graphics/game_main.c
// Point d'entrée principal avec menu graphique
#include "main_menu.h"
#include "raylib_game.h"
#include "tile_wrapper.h"
#include "../data/save_manager.h"
#include "../core/game.h"
#include "../ia/ai.h"
#include "../utils/random.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // Initialiser le générateur aléatoire
    initRandom();
    
    // Créer le dossier data s'il n'existe pas
    system("mkdir -p data");
    
    // Initialiser Raylib
    InitWindow(1400, 900, "Rummikub");
    SetTargetFPS(60);
    
    while(!WindowShouldClose()) {
        MenuState choice = runMainMenu();
        
        if(choice == MENU_QUIT) {
            break;
        }
        
        GameState* game = NULL;
        GameState loadedGame;
        
        if(choice == MENU_NEW_GAME) {
            // Créer une nouvelle partie via le menu de démarrage
            StartMenuState startMenu;
            if(!showStartMenu(&startMenu)) {
                continue;  // Retour au menu principal
            }
            
            // Créer le jeu avec les noms saisis
            const char* names[4];
            bool isComputer[4] = {false, false, false, false};
            AILevel levels[4] = {AI_RANDOM, AI_RANDOM, AI_RANDOM, AI_RANDOM};
            for(int i = 0; i < startMenu.playerCount; i++) {
                names[i] = startMenu.playerInputs[i].text;
                isComputer[i] = startMenu.playerIsAI[i];
                levels[i] = AI_BASIC;
            }
            
            initGame(&loadedGame, startMenu.playerCount, names, isComputer, levels);
            
            game = &loadedGame;
        } else if(choice == MENU_LOAD_GAME) {
            // Charger une partie sauvegardée
            if(loadGameState(&loadedGame)) {
                game = &loadedGame;
            } else {
                // Erreur de chargement, retour au menu
                continue;
            }
        }
        
        if(game != NULL) {
            // Lancer le jeu
            runRaylibGame(game);
        }
    }
    
    CloseWindow();
    return 0;
}


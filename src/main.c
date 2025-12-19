// src/main.c - Version corrigée
#include <stdio.h>
#include <stdlib.h>
#include "core/game.h"
#include "logic/validation.h"
#include "utils/random.h"

void testStructures() {
    printf("=== Test des structures ===\n");
    
    // Test tuile
    Tile t1 = createTile(5, RED);
    printf("Tuile 1: ");
    printTile(t1);
    printf("\n");
    
    Tile joker = createJoker();
    printf("Joker: ");
    printTile(joker);
    printf("\n");
    
    // Test joueur
    Player p1;
    initPlayer(&p1, "Test", 0);
    addTileToHand(&p1, t1);
    addTileToHand(&p1, createTile(7, BLUE));
    addTileToHand(&p1, joker);
    
    printf("\nMain du joueur: ");
    printPlayerHand(&p1);
    
    printf("Points main: %d\n", calculateHandPoints(&p1));
}

void testGame() {
    printf("\n=== Test jeu complet ===\n");
    
    const char* names[] = {"Alice", "Bob", "Charlie"};
    int isAI[] = {0, 0, 1};
    
    GameState game;
    initGame(&game, 3, names, isAI);
    
    printGameState(&game);
    
    // Test validation
    Tile testSeries[] = {
        createTile(7, RED),
        createTile(7, BLUE),
        createTile(7, YELLOW)
    };
    
    Tile testRun[] = {
        createTile(3, RED),
        createTile(4, RED),
        createTile(5, RED)
    };
    
    printf("\nTest validation série: %s\n", 
           isValidSeries(testSeries, 3) ? "OK" : "ÉCHEC");
    printf("Test validation suite: %s\n", 
           isValidRun(testRun, 3) ? "OK" : "ÉCHEC");
}

int main() {
    initRandom();  // Initialiser le générateur aléatoire
    
    printf("=== RUMMIKUB - Version Console ===\n\n");
    
    int choix;
    printf("Menu:\n");
    printf("1. Tester les structures\n");
    printf("2. Démarrer une partie\n");
    printf("3. Quitter\n");
    printf("Choix: ");
    
    if(scanf("%d", &choix) != 1) {
        printf("Erreur de saisie\n");
        return 1;
    }
    
    switch(choix) {
        case 1:
            testStructures();
            break;
        case 2:
            testGame();
            break;
        case 3:
            printf("Au revoir!\n");
            break;
        default:
            printf("Choix invalide\n");
    }
    
    return 0;
}
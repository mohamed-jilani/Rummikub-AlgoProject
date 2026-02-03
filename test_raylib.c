// test_raylib.c - Test de l'interface graphique Raylib
#include <stdio.h>
#include <stdlib.h>
#include "src/core/game.h"
#include "src/graphics/raylib_game.h"
#include "src/utils/random.h"

int main() {
    printf("=== Test de l'interface graphique Raylib ===\n");
    
    // Initialiser le générateur aléatoire
    initRandom();
    
    // Créer un jeu de test avec 2 joueurs
    int nbJoueurs = 2;
    const char* noms[] = {"Joueur 1", "Joueur 2"};
    bool isComputer[] = {false, false};
    AILevel levels[] = {AI_RANDOM, AI_RANDOM};
    
    GameState game;
    initGame(&game, nbJoueurs, noms, isComputer, levels);
    
    printf("Jeu initialisé avec %d joueurs\n", nbJoueurs);
    printf("Chaque joueur a reçu 14 tuiles\n");
    printf("Lancement de l'interface graphique...\n");
    printf("Fermez la fenêtre pour quitter.\n\n");
    
    // Lancer l'interface graphique
    runRaylibGame(&game);
    
    printf("Interface graphique fermée.\n");
    return 0;
}









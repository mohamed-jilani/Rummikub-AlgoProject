// src/main.c - Version corrigée
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  // Ajouter cet en-tête pour strdup

#include "core/game.h"
#include "logic/validation.h"
#include "logic/rules.h"
#include "logic/scoring.h"
#include "utils/random.h"
#include "utils/helpers.h"

void testStructures() {
    printf("=== Test des structures ===\n");
    
    Tile t1 = createTile(5, RED);
    printf("Tuile 1: ");
    printTile(t1);
    printf("\n");
    
    Tile joker = createJoker();
    printf("Joker: ");
    printTile(joker);
    printf("\n");
    
    Player p1;
    initPlayer(&p1, "Test", 0);
    addTileToHand(&p1, t1);
    addTileToHand(&p1, createTile(7, BLUE));
    addTileToHand(&p1, joker);
    
    printf("\nMain du joueur: ");
    printPlayerHand(&p1);
    
    printf("Points main: %d\n", calculateHandPoints(&p1));
}

void playRealGame() {
    printf("\n=== DÉMARRAGE D'UNE PARTIE ===\n");
    
    // Configuration
    int nbJoueurs = readInt("Nombre de joueurs (2-4): ", 2, 4);
    
    // Allouer dynamiquement les noms
    char** noms = malloc(nbJoueurs * sizeof(char*));
    int* isAI = malloc(nbJoueurs * sizeof(int));
    
    if(!noms || !isAI) {
        printf("Erreur d'allocation mémoire!\n");
        if(noms) free(noms);
        if(isAI) free(isAI);
        return;
    }
    
    for(int i = 0; i < nbJoueurs; i++) {
        char nom[50];
        printf("Nom du joueur %d: ", i+1);
        scanf("%49s", nom);
        clearInputBuffer();
        
        int ia = readInt("Est-ce une IA? (0=non, 1=oui): ", 0, 1);
        
        // Allouer et copier le nom
        noms[i] = malloc(strlen(nom) + 1);
        if(noms[i]) {
            strcpy(noms[i], nom);
        } else {
            noms[i] = "Joueur";
        }
        isAI[i] = ia;
    }
    
    // Initialiser le jeu
    GameState game;
    initGame(&game, nbJoueurs, (const char**)noms, isAI);
    
    printf("\n=== PARTIE COMMENCE ===\n");
    printf("Chaque joueur a reçu 14 tuiles.\n");
    
    int tour = 1;
    int abandon = 0;
    
    while(!isGameOver(&game) && !abandon) {
        printf("\n═══════════════════════════════════════\n");
        printf("TOUR %d - %s\n", tour, game.players[game.currentPlayer].name);
        printf("═══════════════════════════════════════\n");
        
        printGameState(&game);
        
        Player* joueurActuel = &game.players[game.currentPlayer];
        
        if(joueurActuel->isAI) {
            printf("\n%s (IA) joue...\n", joueurActuel->name);
            
            // IA simple : pioche une tuile
            if(game.tilesInDeck > 0) {
                Tile nouvelle = drawTile(&game.board);
                addTileToHand(joueurActuel, nouvelle);
                game.tilesInDeck--;
                printf("L'IA a pioché: ");
                printTile(nouvelle);
                printf("\n");
            } else {
                printf("Le sac est vide! L'IA passe son tour.\n");
            }
        } else {
            // Tour joueur humain
            int finTour = 0;
            
            while(!finTour) {
                printf("\n%s, c'est votre tour!\n", joueurActuel->name);
                printf("Votre main: ");
                printPlayerHand(joueurActuel);
                printf("Points dans la main: %d\n", calculateHandPoints(joueurActuel));
                
                printf("\nActions disponibles:\n");
                printf("1. Voir ma main\n");
                printf("2. Voir le plateau\n");
                printf("3. Piocher une tuile (termine le tour)\n");
                printf("4. Poser une combinaison\n");
                printf("5. Passer mon tour\n");
                printf("6. Quitter la partie\n");
                
                int choix = readInt("Votre choix: ", 1, 6);
                
                switch(choix) {
                    case 1:
                        printf("\nVotre main: ");
                        printPlayerHand(joueurActuel);
                        break;
                        
                    case 2:
                        printBoard(&game.board);
                        break;
                        
                    case 3:
                        if(game.tilesInDeck > 0) {
                            Tile nouvelle = drawTile(&game.board);
                            addTileToHand(joueurActuel, nouvelle);
                            game.tilesInDeck--;
                            printf("Vous avez pioché: ");
                            printTile(nouvelle);
                            printf("\n");
                            finTour = 1;
                        } else {
                            printf("Le sac est vide!\n");
                            finTour = 1;
                        }
                        break;
                        
                    case 4:
                        printf("Fonctionnalité 'Poser une combinaison' à implémenter...\n");
                        printf("Pour cette version, vous devez d'abord piocher.\n");
                        break;
                        
                    case 5:
                        printf("Vous passez votre tour.\n");
                        finTour = 1;
                        break;
                        
                    case 6:
                        printf("Partie abandonnée.\n");
                        abandon = 1;
                        finTour = 1;
                        break;
                }
            }
        }
        
        // Vérifier si le joueur a gagné
        if(joueurActuel->tileCount == 0) {
            printf("\n🎉 FÉLICITATIONS ! %s a posé toutes ses tuiles ! 🎉\n", 
                   joueurActuel->name);
            break;
        }
        
        // Passer au joueur suivant seulement si pas abandon
        if(!abandon) {
            nextPlayer(&game);
            tour++;
            
            // Pause pour lisibilité
            if(!joueurActuel->isAI && !abandon) {
                printf("\nAppuyez sur Entrée pour continuer...");
                clearInputBuffer();
            }
        }
    }
    
    // Fin de partie - calcul des scores seulement si pas abandon
    if(!abandon) {
        printf("\n═══════════════════════════════════════\n");
        printf("FIN DE PARTIE - CALCUL DES SCORES\n");
        printf("═══════════════════════════════════════\n");
        
        int gagnant = -1;
        int minPoints = 1000;
        
        for(int i = 0; i < game.playerCount; i++) {
            int points = calculateHandPoints(&game.players[i]);
            printf("%s: %d points ", game.players[i].name, points);
            
            if(points == 0) {
                printf("🎉 (GAGNANT!)\n");
                gagnant = i;
            } else {
                printf("\n");
            }
            
            if(points < minPoints && points > 0) {
                minPoints = points;
                gagnant = i;
            }
        }
        
        if(gagnant != -1) {
            printf("\n🏆 VAINQUEUR: %s ! 🏆\n", game.players[gagnant].name);
            
            // Mettre à jour les scores totaux
            updateScores(game.players, game.playerCount, gagnant);
            
            printf("\nScores totaux:\n");
            for(int i = 0; i < game.playerCount; i++) {
                printf("%s: %d points\n", game.players[i].name, game.players[i].score);
            }
        }
    }
    
    // Libérer la mémoire
    for(int i = 0; i < nbJoueurs; i++) {
        free(noms[i]);
    }
    free(noms);
    free(isAI);
}


void playSimpleGame() {
    printf("=== JEU SIMPLIFIÉ ===\n");
    
    // Configuration fixe
    int nbJoueurs = 2;
    const char* noms[] = {"Joueur1", "Joueur2"};
    int isAI[] = {0, 0};
    
    // Initialiser
    GameState game;
    initGame(&game, nbJoueurs, noms, isAI);
    
    // Afficher état initial
    printf("\nÉtat initial:\n");
    printGameState(&game);
    
    // 3 tours seulement
    for(int tour = 1; tour <= 3; tour++) {
        printf("\n--- Tour %d ---\n", tour);
        
        for(int i = 0; i < nbJoueurs; i++) {
            printf("\n%s joue...\n", game.players[i].name);
            
            // Pioche une tuile
            if(game.tilesInDeck > 0) {
                Tile t = drawTile(&game.board);
                addTileToHand(&game.players[i], t);
                game.tilesInDeck--;
                printf("A pioché: ");
                printTile(t);
                printf("\n");
            }
            
            // Affiche sa main
            printPlayerHand(&game.players[i]);
        }
    }
    
    // Scores finaux
    printf("\n=== SCORES FINAUX ===\n");
    for(int i = 0; i < nbJoueurs; i++) {
        int pts = calculateHandPoints(&game.players[i]);
        printf("%s: %d points\n", game.players[i].name, pts);
    }
}

int main() {
    initRandom();
    
    printf("=== RUMMIKUB - Version Console ===\n");
    printf("Projet Algorithmique - ISTY\n\n");
    
    while(1) {
        printf("\nMenu Principal:\n");
        printf("1. Tester les structures de base\n");
        printf("2. Démarrer une nouvelle partie\n");
        printf("3. Voir les règles du jeu\n");
        printf("4. Sauvegarder/Charger (à venir)\n");
        printf("5. Quitter\n");
        
        int choix = readInt("Votre choix: ", 1, 5);
        
        switch(choix) {
            case 1:
                testStructures();
                break;
            case 2:
                playRealGame();
                break;
            case 3:
                printf("\n=== RÈGLES DU RUMMIKUB ===\n");
                printf("But: Être le premier à poser toutes ses tuiles.\n");
                printf("\nCombinaisons valides:\n");
                printf("  • Série: 3-4 tuiles même valeur, couleurs différentes\n");
                printf("  • Suite: 3+ tuiles consécutives, même couleur\n");
                printf("\nPremier tour: besoin de 30+ points pour poser\n");
                printf("Joker: Remplace n'importe quelle tuile (30 points si gardé)\n");
                printf("\nActions possibles:\n");
                printf("  • Piocher une tuile\n");
                printf("  • Poser/Modifier des combinaisons\n");
                printf("  • Récupérer un joker\n");
                break;
            case 4:
                printf("\nFonctionnalité de sauvegarde à implémenter...\n");
                break;
            case 5:
                printf("Au revoir!\n");
                return 0;
        }
        
        printf("\nAppuyez sur Entrée pour continuer...");
        clearInputBuffer();
    }
    
    return 0;
}
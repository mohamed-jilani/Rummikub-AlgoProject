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
#include "graphics/simple_graphics.h"
#include "graphics/raylib_game.h"
#include "ia/ai.h"  // Inclut AILevel
#include "data/scores_manager.h"
#include "data/save_manager.h"
#include "data/players_manager.h"

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
    initPlayer(&p1, "Test", false, AI_RANDOM);
    addTileToHand(&p1, t1);
    addTileToHand(&p1, createTile(7, BLUE));
    addTileToHand(&p1, joker);
    
    printf("\nMain du joueur: ");
    printPlayerHand(&p1);
    
    printf("Points main: %d\n", calculateHandPoints(&p1));
}



void playSimpleGame() {
    printf("=== JEU SIMPLIFIÉ ===\n");
    
    // Configuration fixe
    int nbJoueurs = 2;
    const char* noms[] = {"Joueur1", "Joueur2"};
    bool isComputer[] = {false, false};
    AILevel levels[] = {AI_RANDOM, AI_RANDOM};
    
    // Initialiser
    GameState game;
    initGame(&game, nbJoueurs, noms, isComputer, levels);
    
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

void displayMainMenu() {
    printf("\n═══════════════════════════════════════\n");
    printf("    RUMMIKUB - Menu Principal\n");
    printf("═══════════════════════════════════════\n");
    printf("1. Nouvelle partie\n");
    printf("2. Charger partie\n");
    printf("3. Classement\n");
    printf("4. Voir les règles du jeu\n");
    printf("5. Interface graphique (démo)\n");
    printf("6. Tester les structures de base\n");
    printf("7. Quitter\n");
    printf("═══════════════════════════════════════\n");
}

void showRanking() {
    ScoresList scores;
    loadScores(&scores);
    displayTopScores(&scores, 10);
}

int loadGameFromFile(GameState* game) {
    if(!hasSaveFile()) {
        printf("Aucune partie sauvegardée.\n");
        return 0;
    }
    
    if(loadGameState(game)) {
        printf("Partie chargée avec succès!\n");
        return 1;
    } else {
        printf("Erreur lors du chargement de la partie.\n");
        return 0;
    }
}

void playGameWithSave(GameState* game, int isNewGame) {
    int tour = 1;
    int abandon = 0;
    
    if(!isNewGame) {
        // Trouver le numéro de tour approximatif
        tour = 1;  // On pourrait le sauvegarder aussi
    }
    
    while(!isGameOver(game) && !abandon) {
        printf("\n═══════════════════════════════════════\n");
        printf("TOUR %d - %s\n", tour, game->players[game->currentPlayer].name);
        printf("═══════════════════════════════════════\n");
        
        printGameState(game);
        
        Player* joueurActuel = &game->players[game->currentPlayer];
        
        if(joueurActuel->isComputer) {
            printf("\n%s (IA) joue...\n", joueurActuel->name);
            
            // L'IA joue son tour
            bool aiPlayed = playAITurn(game, joueurActuel);
            
            if(aiPlayed) {
                printf("L'IA a joué.\n");
                if(joueurActuel->tileCount == 0) {
                    printf("L'IA a posé toutes ses tuiles!\n");
                }
            } else {
                printf("L'IA n'a pas pu jouer (sac vide ou aucun coup possible).\n");
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
                printf("5. Sauvegarder et passer mon tour\n");
                printf("6. Passer mon tour\n");
                printf("7. Quitter la partie\n");
                
                int choix = readInt("Votre choix: ", 1, 7);
                
                switch(choix) {
                    case 1:
                        printf("\nVotre main: ");
                        printPlayerHand(joueurActuel);
                        break;
                        
                    case 2:
                        printBoard(&game->board);
                        break;
                        
                    case 3:
                        if(game->tilesInDeck > 0) {
                            Tile nouvelle = drawTile(&game->board);
                            addTileToHand(joueurActuel, nouvelle);
                            game->tilesInDeck--;
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
                        if(saveGameState(game)) {
                            printf("Partie sauvegardée avec succès!\n");
                        } else {
                            printf("Erreur lors de la sauvegarde.\n");
                        }
                        finTour = 1;
                        break;
                        
                    case 6:
                        printf("Vous passez votre tour.\n");
                        finTour = 1;
                        break;
                        
                    case 7:
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
            nextPlayer(game);
            tour++;
            
            // Pause pour lisibilité
            if(!joueurActuel->isComputer && !abandon) {
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
        
        for(int i = 0; i < game->playerCount; i++) {
            int points = calculateHandPoints(&game->players[i]);
            printf("%s: %d points ", game->players[i].name, points);
            
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
            printf("\n🏆 VAINQUEUR: %s ! 🏆\n", game->players[gagnant].name);
            
            // Mettre à jour les scores totaux
            updateScores(game->players, game->playerCount, gagnant);
            
            printf("\nScores totaux:\n");
            for(int i = 0; i < game->playerCount; i++) {
                printf("%s: %d points\n", game->players[i].name, game->players[i].score);
                
                // Sauvegarder le score dans scores.txt
                saveScore(game->players[i].name, game->players[i].score);
            }
            
            printf("\nScores sauvegardés dans %s\n", SCORES_FILE);
        }
    }
}

int main() {
    initRandom();
    
    printf("=== RUMMIKUB - Version Console ===\n");
    printf("Projet Algorithmique - ISTY\n\n");
    
    // Créer le dossier data s'il n'existe pas
    system("mkdir -p data");
    
    while(1) {
        displayMainMenu();
        
        int choix = readInt("Votre choix: ", 1, 7);
        
        switch(choix) {
            case 1: {  // Nouvelle partie
                printf("\n=== DÉMARRAGE D'UNE NOUVELLE PARTIE ===\n");
                
                // Configuration
                int nbJoueurs = readInt("Nombre de joueurs (2-4): ", 2, 4);
                
                // Charger les pseudos existants
                PlayersList playersList;
                loadPlayers(&playersList);
                
                // Allouer dynamiquement les noms
                char** noms = malloc(nbJoueurs * sizeof(char*));
                bool* isComputer = malloc(nbJoueurs * sizeof(bool));
                AILevel* aiLevels = malloc(nbJoueurs * sizeof(AILevel));
                
                if(!noms || !isComputer || !aiLevels) {
                    printf("Erreur d'allocation mémoire!\n");
                    if(noms) free(noms);
                    if(isComputer) free(isComputer);
                    if(aiLevels) free(aiLevels);
                    break;
                }
                
                for(int i = 0; i < nbJoueurs; i++) {
                    char nom[50];
                    printf("\nJoueur %d:\n", i+1);
                    
                    // Proposer les pseudos existants
                    if(playersList.count > 0) {
                        printf("Pseudos disponibles: ");
                        for(int j = 0; j < playersList.count && j < 5; j++) {
                            printf("%s ", playersList.names[j]);
                        }
                        printf("\n");
                    }
                    
                    printf("Nom du joueur %d: ", i+1);
                    scanf("%49s", nom);
                    clearInputBuffer();
                    
                    // Ajouter le pseudo à la liste s'il n'existe pas
                    addPlayer(&playersList, nom);
                    
                    int ia = readInt("Est-ce une IA? (0=non, 1=oui): ", 0, 1);
                    aiLevels[i] = AI_RANDOM;
                    
                    if(ia) {
                        printf("Niveau d'IA:\n");
                        printf("  0 = Aléatoire\n");
                        printf("  1 = Basique (suites)\n");
                        int level = readInt("Choix (0-1): ", 0, 1);
                        aiLevels[i] = (AILevel)level;
                    }
                    
                    // Allouer et copier le nom
                    noms[i] = malloc(strlen(nom) + 1);
                    if(noms[i]) {
                        strcpy(noms[i], nom);
                    } else {
                        noms[i] = "Joueur";
                    }
                    isComputer[i] = (ia == 1);
                }
                
                // Sauvegarder la liste des pseudos
                savePlayers(&playersList);
                
                // Initialiser le jeu
                GameState game;
                initGame(&game, nbJoueurs, (const char**)noms, isComputer, aiLevels);
                
                printf("\n=== PARTIE COMMENCE ===\n");
                printf("Chaque joueur a reçu 14 tuiles.\n");
                
                // Jouer la partie
                playGameWithSave(&game, 1);
                
                // Libérer la mémoire
                for(int i = 0; i < nbJoueurs; i++) {
                    free(noms[i]);
                }
                free(noms);
                free(isComputer);
                free(aiLevels);
                break;
            }
            
            case 2: {  // Charger partie
                GameState game;
                if(loadGameFromFile(&game)) {
                    printf("\n=== REPRISE DE LA PARTIE ===\n");
                    playGameWithSave(&game, 0);
                }
                break;
            }
            
            case 3:  // Classement
                showRanking();
                break;
                
            case 4:  // Règles
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
                
            case 5:  // Interface graphique
                printf("\nLancement de l'interface graphique Raylib...\n");
                printf("Fermez la fenêtre pour revenir au menu.\n");
                {
                    runRaylibGame(NULL);
                }
                break;
                
            case 6:  // Tester les structures
                testStructures();
                break;
                
            case 7:  // Quitter
                printf("Au revoir!\n");
                return 0;
        }
        
        printf("\nAppuyez sur Entrée pour continuer...");
        clearInputBuffer();
    }
    
    return 0;
}
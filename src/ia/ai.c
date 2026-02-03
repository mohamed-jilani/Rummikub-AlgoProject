// src/ia/ai.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ai.h"
#include "../logic/validation.h"
#include "../logic/rules.h"
#include "../utils/random.h"

#define MAX_POSSIBLE_MOVES 50

// Comparaison de tuiles pour le tri
int compareTiles(const void* a, const void* b) {
    Tile* tileA = (Tile*)a;
    Tile* tileB = (Tile*)b;
    
    // Les jokers en premier
    if(tileA->isJoker && !tileB->isJoker) return -1;
    if(!tileA->isJoker && tileB->isJoker) return 1;
    if(tileA->isJoker && tileB->isJoker) return 0;
    
    // Puis par couleur
    if(tileA->color != tileB->color) {
        return tileA->color - tileB->color;
    }
    
    // Puis par valeur
    return tileA->value - tileB->value;
}

// Trier les tuiles par valeur
void sortTilesByValue(Tile tiles[], int count) {
    qsort(tiles, count, sizeof(Tile), compareTiles);
}

// Trier les tuiles par couleur
void sortTilesByColor(Tile tiles[], int count) {
    qsort(tiles, count, sizeof(Tile), compareTiles);
}

// Vérifier si le joueur a une tuile spécifique
int hasTileInHand(Player* player, Tile tile) {
    for(int i = 0; i < player->tileCount; i++) {
        if(player->hand[i].isJoker && tile.isJoker) return 1;
        if(!player->hand[i].isJoker && !tile.isJoker &&
           player->hand[i].value == tile.value &&
           player->hand[i].color == tile.color) {
            return 1;
        }
    }
    return 0;
}

// Analyser le chevalet et le trier
void analyzeHand(Player* player, Tile sortedHand[], int* sortedCount) {
    *sortedCount = player->tileCount;
    for(int i = 0; i < player->tileCount; i++) {
        sortedHand[i] = player->hand[i];
    }
    sortTilesByValue(sortedHand, *sortedCount);
}

// Trouver des suites possibles (même couleur, chiffres consécutifs)
int findPossibleRuns(Player* player, AIMove moves[], int maxMoves) {
    int moveCount = 0;
    Tile sortedHand[MAX_TILES];
    int sortedCount;
    
    analyzeHand(player, sortedHand, &sortedCount);
    
    // Compter les jokers disponibles
    int jokerCount = 0;
    for(int i = 0; i < sortedCount; i++) {
        if(sortedHand[i].isJoker) jokerCount++;
    }
    
    // Pour chaque couleur
    for(TileColor color = RED; color <= YELLOW; color++) {
        // Trouver toutes les tuiles de cette couleur
        Tile colorTiles[MAX_TILES];
        int colorCount = 0;
        
        for(int i = 0; i < sortedCount; i++) {
            if(!sortedHand[i].isJoker && sortedHand[i].color == color) {
                colorTiles[colorCount++] = sortedHand[i];
            }
        }
        
        if(colorCount < 2 && jokerCount == 0) continue;
        
        // Trier par valeur
        qsort(colorTiles, colorCount, sizeof(Tile), compareTiles);
        
        // Chercher des suites consécutives
        for(int start = 0; start < colorCount; start++) {
            int end = start;
            int consecutive = 1;
            
            // Trouver la plus longue suite consécutive à partir de start
            while(end < colorCount - 1 && colorTiles[end + 1].value == colorTiles[end].value + 1) {
                end++;
                consecutive++;
            }
            
            // Si on a au moins 3 tuiles consécutives, créer une combinaison
            if(consecutive >= 3) {
                Combination combo;
                initCombination(&combo);
                
                for(int i = start; i <= end; i++) {
                    addTileToCombination(&combo, colorTiles[i]);
                }
                
                if(validateCombination(&combo) && moveCount < maxMoves) {
                    moves[moveCount].combination = combo;
                    moves[moveCount].usesTiles = combo.size;
                    moves[moveCount].isValid = 1;
                    moveCount++;
                }
            }
            
            // Essayer aussi avec des jokers pour combler les gaps
            if(jokerCount > 0 && colorCount >= 2) {
                for(int len = 3; len <= MAX_COMBINATION_SIZE && len <= colorCount + jokerCount; len++) {
                    Combination combo;
                    initCombination(&combo);
                    
                    int currentValue = colorTiles[start].value;
                    int tilesAdded = 0;
                    int jokersUsed = 0;
                    int idx = start;
                    
                    while(tilesAdded < len && currentValue <= 13) {
                        if(idx < colorCount && colorTiles[idx].value == currentValue) {
                            addTileToCombination(&combo, colorTiles[idx]);
                            idx++;
                            tilesAdded++;
                            currentValue++;
                        } else if(jokersUsed < jokerCount && jokersUsed < 2) {
                            Tile joker = createJoker();
                            addTileToCombination(&combo, joker);
                            jokersUsed++;
                            tilesAdded++;
                            currentValue++;
                        } else {
                            break;
                        }
                    }
                    
                    if(combo.size >= 3 && validateCombination(&combo) && moveCount < maxMoves) {
                        // Vérifier qu'on n'a pas déjà cette combinaison
                        int duplicate = 0;
                        for(int m = 0; m < moveCount; m++) {
                            if(moves[m].combination.size == combo.size) {
                                int same = 1;
                                for(int t = 0; t < combo.size; t++) {
                                    Tile t1 = moves[m].combination.tiles[t];
                                    Tile t2 = combo.tiles[t];
                                    if(t1.isJoker != t2.isJoker ||
                                       (!t1.isJoker && (t1.value != t2.value || t1.color != t2.color))) {
                                        same = 0;
                                        break;
                                    }
                                }
                                if(same) {
                                    duplicate = 1;
                                    break;
                                }
                            }
                        }
                        
                        if(!duplicate) {
                            moves[moveCount].combination = combo;
                            moves[moveCount].usesTiles = combo.size;
                            moves[moveCount].isValid = 1;
                            moveCount++;
                        }
                    }
                }
            }
        }
    }
    
    return moveCount;
}

// Trouver des groupes possibles (même chiffre, couleurs différentes)
int findPossibleGroups(Player* player, AIMove moves[], int maxMoves) {
    int moveCount = 0;
    Tile sortedHand[MAX_TILES];
    int sortedCount;
    
    analyzeHand(player, sortedHand, &sortedCount);
    
    // Grouper par valeur
    for(int value = 1; value <= 13; value++) {
        Tile valueTiles[MAX_TILES];
        int valueCount = 0;
        int colorsUsed[4] = {0, 0, 0, 0};  // RED, BLUE, BLACK, YELLOW
        int jokerCount = 0;
        
        // Trouver toutes les tuiles de cette valeur
        for(int i = 0; i < sortedCount; i++) {
            if(sortedHand[i].isJoker) {
                jokerCount++;
            } else if(sortedHand[i].value == value) {
                if(!colorsUsed[(int)sortedHand[i].color]) {
                    valueTiles[valueCount++] = sortedHand[i];
                    colorsUsed[(int)sortedHand[i].color] = 1;
                }
            }
        }
        
        // Ajouter les jokers disponibles
        for(int j = 0; j < jokerCount && valueCount < MAX_COMBINATION_SIZE; j++) {
            valueTiles[valueCount++] = createJoker();
        }
        
        if(valueCount < 3) continue;
        
        // Créer des combinaisons avec 3+ tuiles de couleurs différentes
        for(int size = 3; size <= valueCount && size <= MAX_COMBINATION_SIZE; size++) {
            Combination combo;
            initCombination(&combo);
            
            // Prendre les premières 'size' tuiles (déjà triées par couleur)
            for(int i = 0; i < size; i++) {
                addTileToCombination(&combo, valueTiles[i]);
            }
            
            if(validateCombination(&combo) && moveCount < maxMoves) {
                moves[moveCount].combination = combo;
                moves[moveCount].usesTiles = combo.size;
                moves[moveCount].isValid = 1;
                moveCount++;
            }
        }
    }
    
    return moveCount;
}

// Trouver tous les coups possibles selon le niveau d'IA
int findPossibleMoves(Player* player, GameState* game, AIMove moves[], int maxMoves, AILevel level) {
    int moveCount = 0;
    
    // IA aléatoire : pas de recherche de combinaisons complexe
    if(level == AI_RANDOM) {
        return 0;
    }
    
    // Stratégie basique : suites + groupes
    if(level == AI_BASIC) {
        moveCount = findPossibleRuns(player, moves, maxMoves);
        int groupCount = findPossibleGroups(player, moves + moveCount, maxMoves - moveCount);
        moveCount += groupCount;
    }
    
    // Évaluer tous les coups trouvés
    for(int i = 0; i < moveCount; i++) {
        moves[i].score = calculateMoveScore(&moves[i], game, level);
    }
    
    return moveCount;
}

// Évaluer une combinaison (points)
int evaluateCombination(Combination* combo) {
    int score = 0;
    for(int i = 0; i < combo->size; i++) {
        if(combo->tiles[i].isJoker) {
            score += 30;
        } else {
            score += combo->tiles[i].value;
        }
    }
    return score;
}

// Calculer le score d'un coup
int calculateMoveScore(AIMove* move, GameState* game, AILevel level) {
    (void)level;  // Peut être utilisé pour des stratégies futures
    int baseScore = evaluateCombination(&move->combination);
    
    // Bonus pour utiliser plus de tuiles
    int tileBonus = move->usesTiles * 2;
    
    // Bonus pour le premier tour (30+ points requis)
    int firstTurnBonus = 0;
    if(game->phase == FIRST_MOVE && baseScore >= 30) {
        firstTurnBonus = 50;
    }
    
    return baseScore + tileBonus + firstTurnBonus;
}

bool playAITurn(GameState* game, Player* player) {
    if (!player->isComputer) return false;
    
    // Trouver l'index du joueur
    int playerIndex = -1;
    for (int i = 0; i < game->playerCount; i++) {
        if (&game->players[i] == player) {
            playerIndex = i;
            break;
        }
    }
    if (playerIndex == -1) return false;

    if (player->aiLevel == AI_RANDOM) {
        // AI_RANDOM: 3 tentatives de placement aléatoire
        for (int attempt = 0; attempt < 3; attempt++) {
            if (player->tileCount == 0) break;
            
            int tileIdx = rand() % player->tileCount;
            Tile tile = player->hand[tileIdx];
            
            // Essayer de placer sur une ligne aléatoire
            int row = rand() % MAX_COMBINATIONS;
            int col = 0; 
            
            // On ne peut pas manipuler le plateau (ajouter à une ligne existante) 
            // si on n'a pas fait l'initial meld
            if (!player->hasMadeInitialMeld && game->board.combinations[row].size > 0) {
                continue;
            }
            
            if (board_add_tile(&game->board, tile, row, col)) {
                // Vérifier si le plateau est toujours valide
                if (validateBoard(&game->board)) {
                    // Si premier tour, vérifier les 30 points
                    if (!player->hasMadeInitialMeld) {
                        int total = 0;
                        for (int i = 0; i < game->board.combinationCount; i++) {
                            total += calculateCombinationPoints(game->board.combinations[i].tiles, game->board.combinations[i].size);
                        }
                        if (total >= 30) {
                            player->hasMadeInitialMeld = 1;
                            removeTileFromHand(player, tileIdx);
                            game->has_played_tile_this_turn = 1;
                            return true;
                        } else {
                            // Annuler le placement (besoin d'une fonction de rollback ou gérer manuellement)
                            // Pour simplifier l'IA random : on ne joue que si on a déjà fait l'initial meld
                            // ou si on a de la chance. Mais ici on rollback.
                            removeTileFromCombination(&game->board.combinations[row], col);
                        }
                    } else {
                        removeTileFromHand(player, tileIdx);
                        game->has_played_tile_this_turn = 1;
                        return true;
                    }
                } else {
                    // Rollback
                    removeTileFromCombination(&game->board.combinations[row], col);
                }
            }
        }
        
        // Si aucune pose réussie, piocher
        if (game->tilesInDeck > 0) {
            Tile t = drawTile(&game->board);
            addTileToHand(player, t);
            game->tilesInDeck--;
            game->has_drawn_this_turn = 1;
            return true;
        }
    } else if (player->aiLevel == AI_BASIC) {
        // AI_BASIC: Groupes, Suites, Extension
        AIMove moves[MAX_POSSIBLE_MOVES];
        int moveCount = findPossibleMoves(player, game, moves, MAX_POSSIBLE_MOVES, AI_BASIC);
        
        // Essayer aussi d'étendre les combinaisons existantes
        if (player->hasMadeInitialMeld) {
            for (int i = 0; i < player->tileCount; i++) {
                for (int c = 0; c < game->board.combinationCount; c++) {
                    if (canAddTileToExisting(game->board.combinations[c].tiles, game->board.combinations[c].size, player->hand[i])) {
                        // Ajouter la tuile (à la fin pour simplifier)
                        board_add_tile(&game->board, player->hand[i], c, game->board.combinations[c].size);
                        removeTileFromHand(player, i);
                        game->has_played_tile_this_turn = 1;
                        return true;
                    }
                }
            }
        }

        if (moveCount > 0) {
            // Jouer le meilleur coup (celui qui utilise le plus de tuiles ou plus de points)
            int bestIdx = 0;
            for (int i = 1; i < moveCount; i++) {
                if (moves[i].score > moves[bestIdx].score) bestIdx = i;
            }
            
            // Vérifier initial meld
            if (!player->hasMadeInitialMeld) {
                if (calculateCombinationPoints(moves[bestIdx].combination.tiles, moves[bestIdx].combination.size) < 30) {
                    // Ne peut pas jouer ce tour
                } else {
                    player->hasMadeInitialMeld = 1;
                    // Jouer
                    addCombinationToBoard(&game->board, moves[bestIdx].combination);
                    // Retirer les tuiles de la main (besoin d'une boucle)
                    for (int t = 0; t < moves[bestIdx].combination.size; t++) {
                        // Trouver l'index dans la main et retirer
                        for (int h = 0; h < player->tileCount; h++) {
                            if (isTileEqual(player->hand[h], moves[bestIdx].combination.tiles[t])) {
                                removeTileFromHand(player, h);
                                break;
                            }
                        }
                    }
                    game->has_played_tile_this_turn = 1;
                    return true;
                }
            } else {
                // Jouer normalement
                addCombinationToBoard(&game->board, moves[bestIdx].combination);
                for (int t = 0; t < moves[bestIdx].combination.size; t++) {
                    for (int h = 0; h < player->tileCount; h++) {
                        if (isTileEqual(player->hand[h], moves[bestIdx].combination.tiles[t])) {
                            removeTileFromHand(player, h);
                            break;
                        }
                    }
                }
                game->has_played_tile_this_turn = 1;
                return true;
            }
        }
        
        // Piocher si rien trouvé
        if (game->tilesInDeck > 0) {
            Tile t = drawTile(&game->board);
            addTileToHand(player, t);
            game->tilesInDeck--;
            game->has_drawn_this_turn = 1;
            return true;
        }
    }
    
    return false;
}

int aiPlayTurn(GameState* game, int playerIndex, AILevel level) {
    (void)level; // On utilise maintenant playAITurn avec l'objet Player directement
    return (int)playAITurn(game, &game->players[playerIndex]);
}


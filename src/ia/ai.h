#ifndef AI_H
#define AI_H

#include "../core/game.h"
#include "../core/player.h"
#include "../core/combination.h"

typedef struct {
    Combination combination;
    int usesTiles;
    int isValid;
    int score;
} AIMove;

bool playAITurn(GameState* game, Player* player);
int aiPlayTurn(GameState* game, int playerIndex, AILevel level); // Garder pour compatibilité si besoin
int findPossibleMoves(Player* player, GameState* game, AIMove moves[], int maxMoves, AILevel level);
int evaluateCombination(Combination* combo);
int calculateMoveScore(AIMove* move, GameState* game, AILevel level);

#endif

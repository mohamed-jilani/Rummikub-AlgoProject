// src/logic/scoring.h
#ifndef SCORING_H
#define SCORING_H

#include "../core/player.h"

int calculatePlayerScore(Player* player);
int calculateRoundWinner(Player players[], int count);
void updateScores(Player players[], int count, int winnerIndex);

#endif
// src/logic/scoring.c
#include "scoring.h"

int calculatePlayerScore(Player* player) {
    int score = 0;
    for(int i = 0; i < player->tileCount; i++) {
        if(player->hand[i].isJoker) {
            score += 30;
        } else {
            score += player->hand[i].value;
        }
    }
    return score;
}

int calculateRoundWinner(Player players[], int count) {
    int winner = -1;
    int minScore = 1000;
    
    for(int i = 0; i < count; i++) {
        int score = calculatePlayerScore(&players[i]);
        if(score < minScore) {
            minScore = score;
            winner = i;
        }
    }
    
    return winner;
}

void updateScores(Player players[], int count, int winnerIndex) {
    int winnerScore = calculatePlayerScore(&players[winnerIndex]);
    
    for(int i = 0; i < count; i++) {
        if(i == winnerIndex) {
            // Le gagnant marque les points des autres
            int total = 0;
            for(int j = 0; j < count; j++) {
                if(j != winnerIndex) {
                    total += calculatePlayerScore(&players[j]);
                }
            }
            players[i].score += total;
        } else {
            // Les perdants ont des points négatifs
            int playerScore = calculatePlayerScore(&players[i]);
            players[i].score -= (playerScore - winnerScore);
        }
    }
}
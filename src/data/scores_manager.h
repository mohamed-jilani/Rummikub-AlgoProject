// src/data/scores_manager.h
#ifndef SCORES_MANAGER_H
#define SCORES_MANAGER_H

#include "../core/player.h"

#define SCORES_FILE "data/scores.txt"
#define MAX_SCORES 100
#define MAX_DATE_LENGTH 20

// Structure pour un score
typedef struct {
    char name[MAX_NAME];
    int score;
    char date[MAX_DATE_LENGTH];
} ScoreEntry;

// Structure pour la liste des scores
typedef struct {
    ScoreEntry entries[MAX_SCORES];
    int count;
} ScoresList;

// Fonctions de gestion des scores
void initScoresList(ScoresList* list);
int loadScores(ScoresList* list);
int saveScore(const char* name, int score);
int addScoreEntry(ScoresList* list, const char* name, int score, const char* date);
void sortScoresByScore(ScoresList* list);
void displayTopScores(ScoresList* list, int topN);

#endif




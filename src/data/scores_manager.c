// src/data/scores_manager.c
#include "scores_manager.h"
#include "../core/player.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

void initScoresList(ScoresList* list) {
    list->count = 0;
    for(int i = 0; i < MAX_SCORES; i++) {
        list->entries[i].name[0] = '\0';
        list->entries[i].score = 0;
        list->entries[i].date[0] = '\0';
    }
}

// Obtenir la date actuelle au format YYYY-MM-DD HH:MM:SS
void getCurrentDate(char* dateStr, int maxLen) {
    time_t now = time(NULL);
    struct tm* timeinfo = localtime(&now);
    strftime(dateStr, maxLen, "%Y-%m-%d %H:%M:%S", timeinfo);
}

int loadScores(ScoresList* list) {
    initScoresList(list);
    
    FILE* file = fopen(SCORES_FILE, "r");
    if(file == NULL) {
        return 0;  // Fichier n'existe pas encore
    }
    
    char line[200];
    while(fgets(line, sizeof(line), file) != NULL && list->count < MAX_SCORES) {
        // Format: nom:score:date
        char name[MAX_NAME];
        int score;
        char date[MAX_DATE_LENGTH];
        
        // Parser la ligne
        if(sscanf(line, "%49[^:]:%d:%19[^\n]", name, &score, date) == 3) {
            strncpy(list->entries[list->count].name, name, MAX_NAME - 1);
            list->entries[list->count].name[MAX_NAME - 1] = '\0';
            list->entries[list->count].score = score;
            strncpy(list->entries[list->count].date, date, MAX_DATE_LENGTH - 1);
            list->entries[list->count].date[MAX_DATE_LENGTH - 1] = '\0';
            list->count++;
        }
    }
    
    fclose(file);
    return 1;
}

int saveScore(const char* name, int score) {
    FILE* file = fopen(SCORES_FILE, "a");
    if(file == NULL) {
        return 0;
    }
    
    char date[MAX_DATE_LENGTH];
    getCurrentDate(date, MAX_DATE_LENGTH);
    
    fprintf(file, "%s:%d:%s\n", name, score, date);
    
    fclose(file);
    return 1;
}

int addScoreEntry(ScoresList* list, const char* name, int score, const char* date) {
    if(list->count >= MAX_SCORES) {
        return 0;  // Liste pleine
    }
    
    strncpy(list->entries[list->count].name, name, MAX_NAME - 1);
    list->entries[list->count].name[MAX_NAME - 1] = '\0';
    list->entries[list->count].score = score;
    strncpy(list->entries[list->count].date, date, MAX_DATE_LENGTH - 1);
    list->entries[list->count].date[MAX_DATE_LENGTH - 1] = '\0';
    list->count++;
    
    return 1;
}

// Trier les scores par ordre décroissant (meilleurs scores en premier)
void sortScoresByScore(ScoresList* list) {
    for(int i = 0; i < list->count - 1; i++) {
        for(int j = i + 1; j < list->count; j++) {
            if(list->entries[j].score > list->entries[i].score) {
                // Échanger
                ScoreEntry temp = list->entries[i];
                list->entries[i] = list->entries[j];
                list->entries[j] = temp;
            }
        }
    }
}

void displayTopScores(ScoresList* list, int topN) {
    if(list->count == 0) {
        printf("Aucun score enregistré.\n");
        return;
    }
    
    // Créer une copie pour trier sans modifier l'original
    ScoresList sorted = *list;
    sortScoresByScore(&sorted);
    
    int displayCount = (topN < sorted.count) ? topN : sorted.count;
    
    printf("\n═══════════════════════════════════════\n");
    printf("CLASSEMENT - TOP %d\n", displayCount);
    printf("═══════════════════════════════════════\n");
    printf("%-4s %-30s %-10s %-20s\n", "Rang", "Nom", "Score", "Date");
    printf("─────────────────────────────────────────────────────────────\n");
    
    for(int i = 0; i < displayCount; i++) {
        printf("%-4d %-30s %-10d %-20s\n", 
               i + 1, 
               sorted.entries[i].name, 
               sorted.entries[i].score,
               sorted.entries[i].date);
    }
    
    printf("═══════════════════════════════════════\n");
}


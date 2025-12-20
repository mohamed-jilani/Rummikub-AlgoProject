// src/data/file_manager.c - Version basique
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_manager.h"

void saveScores(Player players[], int count) {
    FILE* file = fopen("data/scores.dat", "a");
    if(!file) {
        printf("Erreur: impossible d'ouvrir scores.dat\n");
        return;
    }
    
    fprintf(file, "=== Nouvelle Partie ===\n");
    for(int i = 0; i < count; i++) {
        fprintf(file, "%s: %d points\n", players[i].name, players[i].score);
    }
    fprintf(file, "\n");
    
    fclose(file);
    printf("Scores sauvegardés dans data/scores.dat\n");
}

void loadScores() {
    FILE* file = fopen("data/scores.dat", "r");
    if(!file) {
        printf("Aucun score sauvegardé.\n");
        return;
    }
    
    printf("\n=== SCORES SAUVEGARDÉS ===\n");
    char ligne[100];
    while(fgets(ligne, sizeof(ligne), file)) {
        printf("%s", ligne);
    }
    
    fclose(file);
}
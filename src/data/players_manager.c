// src/data/players_manager.c
#include "players_manager.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void initPlayersList(PlayersList* list) {
    list->count = 0;
    for (int i = 0; i < MAX_PLAYERS_HISTORY; i++) {
        list->names[i][0] = '\0';
    }
}

int loadPlayers(PlayersList* list) {
    initPlayersList(list);
    
    FILE* file = fopen(PLAYERS_FILE, "r");
    if (file == NULL) {
        return 0;  // Fichier n'existe pas encore
    }
    
    char line[MAX_NAME_LENGTH + 2];  // +2 pour \n et \0
    while (fgets(line, sizeof(line), file) != NULL && list->count < MAX_PLAYERS_HISTORY) {
        // Supprimer le \n à la fin
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        
        // Ignorer les lignes vides
        if (strlen(line) > 0) {
            strncpy(list->names[list->count], line, MAX_NAME_LENGTH - 1);
            list->names[list->count][MAX_NAME_LENGTH - 1] = '\0';
            list->count++;
        }
    }
    
    fclose(file);
    return 1;
}

int savePlayers(const PlayersList* list) {
    FILE* file = fopen(PLAYERS_FILE, "w");
    if (file == NULL) {
        return 0;
    }
    
    for (int i = 0; i < list->count; i++) {
        fprintf(file, "%s\n", list->names[i]);
    }
    
    fclose(file);
    return 1;
}

int addPlayer(PlayersList* list, const char* name) {
    if (list->count >= MAX_PLAYERS_HISTORY) {
        return 0;  // Liste pleine
    }
    
    if (strlen(name) == 0) {
        return 0;  // Nom vide
    }
    
    // Vérifier si le nom existe déjà
    for (int i = 0; i < list->count; i++) {
        if (strcmp(list->names[i], name) == 0) {
            return 0;  // Nom déjà existant
        }
    }
    
    strncpy(list->names[list->count], name, MAX_NAME_LENGTH - 1);
    list->names[list->count][MAX_NAME_LENGTH - 1] = '\0';
    list->count++;
    
    return 1;
}





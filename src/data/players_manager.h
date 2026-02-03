// src/data/players_manager.h
#ifndef PLAYERS_MANAGER_H
#define PLAYERS_MANAGER_H

#define MAX_PLAYERS_HISTORY 10
#define MAX_NAME_LENGTH 50
#define PLAYERS_FILE "players.txt"

// Structure pour stocker les pseudos
typedef struct {
    char names[MAX_PLAYERS_HISTORY][MAX_NAME_LENGTH];
    int count;
} PlayersList;

// Fonctions de gestion des pseudos
int loadPlayers(PlayersList* list);
int savePlayers(const PlayersList* list);
int addPlayer(PlayersList* list, const char* name);
void initPlayersList(PlayersList* list);

#endif





// src/graphics/raylib_game.h
#ifndef RAYLIB_GAME_H
#define RAYLIB_GAME_H

// Inclure les headers du projet en premier
#include "../core/game.h"
#include "raylib_renderer.h"
#include "ui_components.h"
#include "../data/players_manager.h"
// Note: raylib.h sera inclus dans raylib_game.c pour éviter les conflits

typedef enum {
    SCREEN_MAIN_MENU,
    SCREEN_PLAYER_SETUP,
    SCREEN_GAME,
    SCREEN_RULES,
    SCREEN_HISTORY
} GameScreen;

// Structure pour l'état de l'interface graphique
typedef struct {
    GameState* game;
    GameScreen currentScreen;
    
    // Scrolling du plateau
    float boardScrollY;
    
    int selectedTileIndex;  // Index de la tuile sélectionnée dans le chevalet (-1 si aucune)
    int hoveredTileIndex;   // Index de la tuile survolée
    int isRunning;
    float animationTime;
    
    // État du drag & drop
    int isDragging;         // 1 si on est en train de déplacer une tuile
    int isDraggingFromBoard; // 1 si on déplace depuis le plateau
    int dragTileIndex;      // Index de la tuile (dans la main si !isDraggingFromBoard)
    int dragBoardRow;       // Ligne source (si isDraggingFromBoard)
    int dragBoardCol;       // Colonne source (si isDraggingFromBoard)
    int dragOffsetX;        // Offset X de la souris par rapport à la tuile
    int dragOffsetY;        // Offset Y de la souris par rapport à la tuile
    int hoveredBoardRow;    // Ligne du plateau survolée (-1 si aucune)
    int hoveredBoardCol;    // Colonne du plateau survolée (-1 si aucune)
    
    // Gestion spéciale des jokers
    int selectedJoker;      // 1 si un joker est sélectionné, 0 sinon
    Tile draggedJokerTile;  // Copie de la tuile joker en cours de déplacement
    int jokerFromBoard;     // 1 si le joker vient du plateau (pas du chevalet), 0 sinon
    int jokerBoardRow;      // Ligne du joker sur le plateau (si jokerFromBoard)
    int jokerBoardCol;      // Colonne du joker sur le plateau (si jokerFromBoard)
    
    // Plateau de jeu (tableau 2D pour les interactions)
    Tile** boardTiles;      // Tableau 2D des tuiles sur le plateau
    int boardRows;           // Nombre de lignes du plateau
    int boardCols;          // Nombre de colonnes du plateau
    
    // Interface utilisateur
    MessageLog messageLog;  // Journal des messages
    Button drawButton;      // Bouton "Piocher"
    Button validateButton;  // Bouton "Valider"
    Button passButton;      // Bouton "Passer"
    Button undoButton;      // Bouton "Annuler"
    
    // Boutons du menu principal
    Button menuNewGame;
    Button menuRules;
    Button menuHistory;
    Button menuExit;
    Button backButton;
    
    int currentTurn;        // Numéro du tour actuel
    Board turnStartBoard;   // État du plateau au début du tour
} RaylibGameState;

// Fonctions principales
int initRaylibGame(RaylibGameState* state, GameState* game);
void updateRaylibGame(RaylibGameState* state);
void renderRaylibGame(RaylibGameState* state);
void cleanupRaylibGame(RaylibGameState* state);
void runRaylibGame(GameState* game);

// Fonctions de gestion des entrées
void handleInput(RaylibGameState* state);
void handleMouseInput(RaylibGameState* state);

// Synchroniser le board logique vers l'affichage
void sync_board_to_display(RaylibGameState* state);

// Fonctions d'affichage de l'interface
void drawUI(RaylibGameState* state);
void drawInfoPanel(RaylibGameState* state);

// Interface de démarrage
typedef struct {
    TextInput playerInputs[4];
    int currentInputIndex;
    int playerCount;
    Button startButton;
    Button cancelButton;
    PlayersList savedPlayers;
    int showSavedPlayers;
    int playerIsAI[4];   // 1 si IA, 0 si humain (joueur 1 forcé humain)
    int showError;       // 1 si message d'erreur à afficher
} StartMenuState;

int showStartMenu(StartMenuState* menu);
void initStartMenu(StartMenuState* menu);
void updateStartMenu(StartMenuState* menu);
void drawStartMenu(StartMenuState* menu);

#endif


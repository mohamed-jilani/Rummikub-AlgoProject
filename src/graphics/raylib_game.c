// src/graphics/raylib_game.c
#include "raylib_game.h"
#include "ui_components.h"
#include "../data/players_manager.h"
#include "tile_wrapper.h"  // Utiliser le wrapper pour éviter les conflits
#include "../logic/validation.h"
#include "../logic/rules.h"
#include "../core/combination.h"
#include "../ia/ai.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Initialiser le jeu Raylib
int initRaylibGame(RaylibGameState* state, GameState* game) {
    state->game = game;
    state->selectedTileIndex = -1;
    state->hoveredTileIndex = -1;
    state->isRunning = 1;
    state->animationTime = 0.0f;
    state->currentScreen = SCREEN_MAIN_MENU;
    state->boardScrollY = 0.0f;
    
    // Initialiser l'état du drag & drop
    state->isDragging = 0;
    state->isDraggingFromBoard = 0;
    state->dragTileIndex = -1;
    state->dragBoardRow = -1;
    state->dragBoardCol = -1;
    state->dragOffsetX = 0;
    state->dragOffsetY = 0;
    state->hoveredBoardRow = -1;
    state->hoveredBoardCol = -1;
    
    // Initialiser la gestion des jokers
    state->selectedJoker = 0;
    state->draggedJokerTile = createTile(-1, NO_COLOR);
    state->jokerFromBoard = 0;
    state->jokerBoardRow = -1;
    state->jokerBoardCol = -1;
    
    // Initialiser le plateau (tableau 2D)
    state->boardRows = MAX_COMBINATIONS;
    state->boardCols = MAX_COMBINATION_SIZE;
    state->boardTiles = (Tile**)calloc(state->boardRows, sizeof(Tile*));
    for (int i = 0; i < state->boardRows; i++) {
        state->boardTiles[i] = (Tile*)calloc(state->boardCols, sizeof(Tile));
    }
    
    // Initialiser les messages
    initMessageLog(&state->messageLog);
    
    // Initialiser les boutons horizontalement au-dessus du chevalet
    int btnWidth = 140;
    int btnHeight = 50;
    int spacing = 20;
    int totalWidth = 4 * btnWidth + 3 * spacing;
    int btnX = (WINDOW_WIDTH - totalWidth) / 2;
    int btnY = BUTTONS_Y;
    
    state->drawButton = createUIButton(btnX, btnY, btnWidth, btnHeight, "Piocher");
    state->validateButton = createUIButton(btnX + btnWidth + spacing, btnY, btnWidth, btnHeight, "Valider");
    state->passButton = createUIButton(btnX + 2 * (btnWidth + spacing), btnY, btnWidth, btnHeight, "Passer");
    state->undoButton = createUIButton(btnX + 3 * (btnWidth + spacing), btnY, btnWidth, btnHeight, "Annuler");
    
    // Initialiser les boutons du menu principal
    int menuBtnX = WINDOW_WIDTH / 2 - 100;
    state->menuNewGame = createUIButton(menuBtnX, 300, 200, 50, "Nouvelle Partie");
    state->menuRules = createUIButton(menuBtnX, 380, 200, 50, "Règles du Jeu");
    state->menuHistory = createUIButton(menuBtnX, 460, 200, 50, "Historique");
    state->menuExit = createUIButton(menuBtnX, 540, 200, 50, "Quitter");
    state->backButton = createUIButton(20, WINDOW_HEIGHT - 70, 150, 40, "Retour Menu");
    
    // Couleurs personnalisées pour les boutons
    state->drawButton.normalColor = (RaylibColor){80, 150, 80, 255};
    state->drawButton.hoverColor = (RaylibColor){100, 180, 100, 255};
    state->validateButton.normalColor = (RaylibColor){150, 150, 80, 255};
    state->validateButton.hoverColor = (RaylibColor){180, 180, 100, 255};
    state->passButton.normalColor = (RaylibColor){150, 80, 80, 255};
    state->passButton.hoverColor = (RaylibColor){180, 100, 100, 255};
    state->undoButton.normalColor = (RaylibColor){150, 150, 150, 255};
    state->undoButton.hoverColor = (RaylibColor){180, 180, 180, 255};
    
    // Couleurs menu
    state->menuNewGame.normalColor = (RaylibColor){80, 150, 80, 255};
    state->menuNewGame.hoverColor = (RaylibColor){100, 180, 100, 255};
    state->menuRules.normalColor = (RaylibColor){80, 80, 150, 255};
    state->menuRules.hoverColor = (RaylibColor){100, 100, 180, 255};
    state->menuHistory.normalColor = (RaylibColor){150, 150, 80, 255};
    state->menuHistory.hoverColor = (RaylibColor){180, 180, 100, 255};
    state->menuExit.normalColor = (RaylibColor){150, 80, 80, 255};
    state->menuExit.hoverColor = (RaylibColor){180, 100, 100, 255};
    state->backButton.normalColor = (RaylibColor){100, 100, 100, 255};
    state->backButton.hoverColor = (RaylibColor){130, 130, 130, 255};
    
    state->currentTurn = 1;
    state->turnStartBoard = game->board; // Sauvegarder l'état initial
    
    // Synchroniser le board logique vers l'affichage
    sync_board_to_display(state);
    
    // Initialiser la fenêtre Raylib
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Rummikub - Interface Graphique");
    SetTargetFPS(60);
    
    if (!IsWindowReady()) {
        return 0;
    }
    
    return 1;
}

// Synchroniser le board logique vers l'affichage (boardTiles)
void sync_board_to_display(RaylibGameState* state) {
    // Réinitialiser le tableau 2D
    for (int i = 0; i < state->boardRows; i++) {
        for (int j = 0; j < state->boardCols; j++) {
            state->boardTiles[i][j] = createTile(-1, NO_COLOR);  // Tuile vide
        }
    }
    
    // Copier les combinaisons du board logique vers le tableau 2D
    Board* board = &state->game->board;
    for (int i = 0; i < board->combinationCount && i < state->boardRows; i++) {
        Combination* combo = &board->combinations[i];
        for (int j = 0; j < combo->size && j < state->boardCols; j++) {
            if (isTileValid(combo->tiles[j])) {
                state->boardTiles[i][j] = combo->tiles[j];
            }
        }
    }
}


// Synchroniser le board logique vers l'affichage

// Gérer les entrées clavier
void handleInput(RaylibGameState* state) {
    Player* currentPlayer = &state->game->players[state->game->currentPlayer];
    
    // Navigation dans le chevalet avec les flèches
    if (IsKeyPressed(KEY_LEFT)) {
        if (state->selectedTileIndex > 0) {
            state->selectedTileIndex--;
        } else {
            state->selectedTileIndex = currentPlayer->tileCount - 1;
        }
    }
    
    if (IsKeyPressed(KEY_RIGHT)) {
        if (state->selectedTileIndex < currentPlayer->tileCount - 1) {
            state->selectedTileIndex++;
        } else {
            state->selectedTileIndex = 0;
        }
    }
    
    // Sélection avec la barre d'espace
    if (IsKeyPressed(KEY_SPACE)) {
        if (state->selectedTileIndex >= 0 && 
            state->selectedTileIndex < currentPlayer->tileCount) {
            // Logique de sélection (à implémenter)
            printf("Tuile sélectionnée: %d\n", state->selectedTileIndex);
        }
    }
    
    // Quitter avec ECHAP
    if (IsKeyPressed(KEY_ESCAPE)) {
        state->isRunning = 0;
    }
}

// Vérifier si une position tente de manipuler une combinaison existante
static bool isTryingToManipulateExistingCombination(RaylibGameState* state, int row) {
    // Si la ligne n'était pas vide au début du tour, toute action dessus est une manipulation
    if (row >= 0 && row < state->game->board.combinationCount) {
        return (state->turnStartBoard.combinations[row].size > 0);
    }
    return false;
}

// Vérifier si une tuile spécifique a été posée pendant ce tour
static bool isTilePlacedThisTurn(RaylibGameState* state, int row, int col) {
    if (row < 0 || row >= state->boardRows || col < 0 || col >= state->boardCols) return false;
    
    Tile currentTile = state->boardTiles[row][col];
    if (!isTileValid(currentTile)) return false;
    
    // Si la ligne n'existait pas au début du tour, c'est forcément nouveau
    if (row >= state->turnStartBoard.combinationCount) return true;
    
    // Sinon, vérifier si la tuile à cette position était vide au début du tour
    Tile startTile = state->turnStartBoard.combinations[row].tiles[col];
    return !isTileValid(startTile);
}

// Gérer les entrées souris avec drag & drop
void handleMouseInput(RaylibGameState* state) {
    Player* currentPlayer = &state->game->players[state->game->currentPlayer];
    Vector2 mousePos = GetMousePosition();
    
    // 1. Gestion du clic droit (Annuler ou Retirer du plateau)
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        int row, col;
        if (getBoardPositionAtMouseExtended(mousePos.x, mousePos.y, state->boardScrollY, &row, &col)) {
            // Si clic droit sur une tuile posée ce tour-ci, la remettre dans la main
            if (isTilePlacedThisTurn(state, row, col)) {
                Tile tileToRemove = state->boardTiles[row][col];
                addTileToHand(currentPlayer, tileToRemove);
                
                // Retirer du plateau logique et visuel
                board_remove_tile(&state->game->board, row, col);
                state->boardTiles[row][col] = createTile(-1, NO_COLOR);
                
                addMessage(&state->messageLog, "Tuile remise dans votre main");
                
                // Mettre à jour has_played_tile_this_turn si plus aucune tuile sur le plateau n'est nouvelle
                bool anyNew = false;
                for (int r = 0; r < state->boardRows; r++) {
                    for (int c = 0; c < state->boardCols; c++) {
                        if (isTilePlacedThisTurn(state, r, c)) { anyNew = true; break; }
                    }
                    if (anyNew) break;
                }
                if (!anyNew) state->game->has_played_tile_this_turn = 0;
            }
        }
        
        state->selectedTileIndex = -1;
        state->isDragging = 0;
        state->isDraggingFromBoard = 0;
        state->dragTileIndex = -1;
        return;
    }
    
    // 2. Gestion du Drag & Drop en cours
    if (state->isDragging) {
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            int row, col;
            if (getBoardPositionAtMouseExtended(mousePos.x, mousePos.y, state->boardScrollY, &row, &col)) {
                Tile tileToPlace;
                bool canPlace = false;
                
                if (state->isDraggingFromBoard) {
                    tileToPlace = state->boardTiles[state->dragBoardRow][state->dragBoardCol];
                    canPlace = true;
                } else if (state->dragTileIndex >= 0 && state->dragTileIndex < currentPlayer->tileCount) {
                    tileToPlace = currentPlayer->hand[state->dragTileIndex];
                    canPlace = true;
                }
                
                if (canPlace) {
                    if (isValidBoardPosition(row, col, state->boardTiles, state->boardRows, state->boardCols)) {
                        // Vérifier la restriction du premier tour
                        if (!currentPlayer->hasMadeInitialMeld && isTryingToManipulateExistingCombination(state, row)) {
                            addMessage(&state->messageLog, "Initial Meld requis pour manipuler le plateau !");
                        } else {
                            // Si on vient du plateau, retirer de l'ancienne position
                            if (state->isDraggingFromBoard) {
                                board_remove_tile(&state->game->board, state->dragBoardRow, state->dragBoardCol);
                                state->boardTiles[state->dragBoardRow][state->dragBoardCol] = createTile(-1, NO_COLOR);
                            } else {
                                // Sinon, retirer de la main
                                removeTileFromHand(currentPlayer, state->dragTileIndex);
                            }
                            
                            // Placer à la nouvelle position
                            state->boardTiles[row][col] = tileToPlace;
                            board_add_tile(&state->game->board, tileToPlace, row, col);
                            
                            state->game->moves_in_current_turn++;
                            state->game->has_played_tile_this_turn = 1;
                            addMessage(&state->messageLog, "Tuile déplacée");
                        }
                    } else {
                        addMessage(&state->messageLog, "Position invalide ou occupée !");
                    }
                }
            }
            
            state->isDragging = 0;
            state->isDraggingFromBoard = 0;
            state->dragTileIndex = -1;
            state->selectedTileIndex = -1;
        } else {
            getBoardPositionAtMouseExtended(mousePos.x, mousePos.y, state->boardScrollY, &state->hoveredBoardRow, &state->hoveredBoardCol);
        }
        return;
    }
    
    // 3. Détection du début de Drag ou Sélection
    // Boutons d'abord
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mousePos, state->drawButton.bounds) ||
            CheckCollisionPointRec(mousePos, state->validateButton.bounds) ||
            CheckCollisionPointRec(mousePos, state->passButton.bounds)) {
            return;
        }
    }
    
    // Plateau d'abord (pour déplacer ses propres tuiles)
    int bRow, bCol;
    if (getBoardPositionAtMouseExtended(mousePos.x, mousePos.y, state->boardScrollY, &bRow, &bCol)) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (isTilePlacedThisTurn(state, bRow, bCol)) {
                state->isDragging = 1;
                state->isDraggingFromBoard = 1;
                state->dragBoardRow = bRow;
                state->dragBoardCol = bCol;
                
                // Centrer la tuile sur le curseur (pas besoin de calculer startX/startY)
                state->dragOffsetX = TILE_WIDTH / 2;
                state->dragOffsetY = TILE_HEIGHT / 2;
                return;
            }
        }
    }
    
    // Chevalet
    int rackTileIndex = -1;
    getRackTileAtPosition(mousePos.x, mousePos.y, currentPlayer->hand, currentPlayer->tileCount, &rackTileIndex);
    state->hoveredTileIndex = rackTileIndex;
    
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && rackTileIndex >= 0) {
        state->selectedTileIndex = rackTileIndex;
        state->isDragging = 1;
        state->isDraggingFromBoard = 0;
        state->dragTileIndex = rackTileIndex;
        
        // Centrer la tuile sur le curseur
        state->dragOffsetX = TILE_WIDTH / 2;
        state->dragOffsetY = TILE_HEIGHT / 2;
    }
    
    if (!state->isDragging) {
        getBoardPositionAtMouseExtended(mousePos.x, mousePos.y, state->boardScrollY, &state->hoveredBoardRow, &state->hoveredBoardCol);
    }
}

// Dessiner le panneau d'information
void drawInfoPanel(RaylibGameState* state) {
    int panelX = WINDOW_WIDTH - 280;
    int panelY = 100;
    int panelWidth = 250;
    int panelHeight = 90;  // Réduit pour laisser place aux messages et boutons
    
    // Fond du panneau
    DrawRectangleRounded((Rectangle){panelX, panelY, panelWidth, panelHeight}, 
                        0.1f, 10, (RaylibColor){30, 30, 50, 255});
    DrawRectangleRoundedLines((Rectangle){panelX, panelY, panelWidth, panelHeight}, 
                             0.1f, 10, (RaylibColor){100, 100, 200, 255});
    
    // Titre
    DrawText("INFORMATIONS", panelX + 20, panelY + 20, 22, 
            (RaylibColor){220, 180, 60, 255});
    
    // Informations du joueur actuel
    Player* currentPlayer = &state->game->players[state->game->currentPlayer];
    int y = panelY + 60;
    
    DrawText("Joueur actuel:", panelX + 20, y, 18, (RaylibColor){255, 255, 255, 255});
    y += 25;
    DrawText(currentPlayer->name, panelX + 30, y, 20, 
            (RaylibColor){150, 200, 255, 255});
    y += 35;
    
    DrawText(TextFormat("Tuiles: %d/14", currentPlayer->tileCount), 
            panelX + 20, y, 16, (RaylibColor){200, 200, 200, 255});
    y += 25;
    
    DrawText(TextFormat("Score: %d", currentPlayer->score), 
            panelX + 20, y, 16, (RaylibColor){200, 200, 200, 255});
    y += 40;
    
    // Informations sur la tuile sélectionnée
    if (state->selectedTileIndex >= 0 && 
        state->selectedTileIndex < currentPlayer->tileCount) {
        Tile selectedTile = currentPlayer->hand[state->selectedTileIndex];
        
        DrawText("Tuile selectionnee:", panelX + 20, y, 18, (RaylibColor){255, 255, 255, 255});
        y += 25;
        
        if (selectedTile.isJoker) {
            DrawText("JOKER", panelX + 30, y, 18, (RaylibColor){200, 200, 200, 255});
            y += 25;
            DrawText("Valeur: 30 pts", panelX + 30, y, 16, (RaylibColor){200, 200, 200, 255});
        } else {
            DrawText(TextFormat("Valeur: %d", selectedTile.value), 
                    panelX + 30, y, 18, (RaylibColor){200, 200, 200, 255});
            y += 25;
            DrawText(TextFormat("Couleur: %s", getColorName((int)selectedTile.color)), 
                    panelX + 30, y, 16, (RaylibColor){200, 200, 200, 255});
        }
        y += 40;
    }
    
    // Informations du jeu
    DrawText("Etat du jeu:", panelX + 20, y, 18, (RaylibColor){255, 255, 255, 255});
    y += 25;
    DrawText(TextFormat("Tuiles restantes: %d", state->game->tilesInDeck), 
            panelX + 30, y, 16, (RaylibColor){200, 200, 200, 255});
    y += 25;
    DrawText(TextFormat("Joueurs: %d", state->game->playerCount), 
            panelX + 30, y, 16, (RaylibColor){200, 200, 200, 255});
}

// Dessiner l'interface utilisateur
void drawUI(RaylibGameState* state) {
    // En-tête
    DrawRectangle(0, 0, WINDOW_WIDTH, 60, (RaylibColor){20, 20, 40, 255});
    DrawRectangle(0, 60, WINDOW_WIDTH, 2, (RaylibColor){100, 100, 200, 255});
    
    DrawText("RUMMIKUB", 30, 10, 40, (RaylibColor){220, 180, 60, 255});
    
    // Panneau de gauche : Liste des joueurs (ajusté pour 1920x1080)
    DrawRectangle(0, 62, 240, WINDOW_HEIGHT - 62, (RaylibColor){30, 30, 45, 255});
    DrawRectangle(240, 62, 2, WINDOW_HEIGHT - 62, (RaylibColor){100, 100, 200, 255});
    
    DrawText("JOUEURS", 20, 80, 24, (RaylibColor){220, 180, 60, 255});
    
    for (int i = 0; i < state->game->playerCount; i++) {
        Player* p = &state->game->players[i];
        int yPos = 120 + i * 120;  // Plus d'espace entre joueurs
        
        // Highlight joueur actuel
        if (i == state->game->currentPlayer) {
            DrawRectangle(5, yPos - 10, 230, 110, (RaylibColor){60, 60, 90, 255});
            DrawRectangleLines(5, yPos - 10, 230, 110, (RaylibColor){220, 180, 60, 255});
        }
        
        DrawText(p->name, 20, yPos, 20, (i == state->game->currentPlayer) ? (RaylibColor){220, 180, 60, 255} : WHITE);
        DrawText(TextFormat("Tuiles: %d", p->tileCount), 20, yPos + 30, 18, LIGHTGRAY);
        DrawText(TextFormat("Score: %d", p->score), 20, yPos + 55, 18, LIGHTGRAY);
        
        if (p->isComputer) {
            DrawText("IA", 180, yPos, 16, (RaylibColor){150, 200, 255, 255});
        }
    }
    
    // FPS en haut à droite
    DrawText(TextFormat("FPS: %d", GetFPS()), WINDOW_WIDTH - 100, 10, 18, (RaylibColor){0, 255, 0, 255});
    
    // Crédits en bas
    DrawText("IATIC-3 - Équipe: Hmida, Ghazel, Mathlouthi, Joudi", 260, WINDOW_HEIGHT - 30, 16, DARKGRAY);
}

// Mettre à jour l'état du jeu
void updateRaylibGame(RaylibGameState* state) {
    float deltaTime = GetFrameTime();
    state->animationTime += deltaTime;
    
    // Gérer les écrans
    switch (state->currentScreen) {
        case SCREEN_MAIN_MENU:
            updateUIButton(&state->menuNewGame);
            updateUIButton(&state->menuRules);
            updateUIButton(&state->menuHistory);
            updateUIButton(&state->menuExit);
            
            if (isUIButtonClicked(&state->menuNewGame)) {
                // Pour simplifier, on lance directement le setup des joueurs
                // Dans une version complète, on pourrait passer par SCREEN_PLAYER_SETUP
                state->currentScreen = SCREEN_PLAYER_SETUP;
            }
            if (isUIButtonClicked(&state->menuRules)) {
                state->currentScreen = SCREEN_RULES;
            }
            if (isUIButtonClicked(&state->menuHistory)) {
                state->currentScreen = SCREEN_HISTORY;
            }
            if (isUIButtonClicked(&state->menuExit)) {
                state->isRunning = 0;
            }
            return;
            
        case SCREEN_RULES:
        case SCREEN_HISTORY:
            updateUIButton(&state->backButton);
            if (isUIButtonClicked(&state->backButton)) {
                state->currentScreen = SCREEN_MAIN_MENU;
            }
            return;
            
        case SCREEN_PLAYER_SETUP: {
            // Ici on gère normalement le menu de setup. 
            // Pour l'instant on appelle showStartMenu mais de façon non bloquante si possible,
            // ou on garde la logique actuelle qui initialise le jeu.
            // Vu la structure de showStartMenu qui est bloquante, on va tricher un peu.
            StartMenuState menu;
            if (showStartMenu(&menu)) {
                const char* names[4];
                bool isComputer[4] = {false, false, false, false};
                AILevel levels[4] = {AI_RANDOM, AI_RANDOM, AI_RANDOM, AI_RANDOM};
                for (int i = 0; i < menu.playerCount; i++) {
                    names[i] = menu.playerInputs[i].text;
                    if (i == 0) isComputer[i] = false;
                    else {
                        isComputer[i] = menu.playerIsAI[i];
                        levels[i] = AI_BASIC;
                    }
                }
                initGame(state->game, menu.playerCount, names, isComputer, levels);
                state->turnStartBoard = state->game->board;
                state->currentScreen = SCREEN_GAME;
                sync_board_to_display(state);
            } else {
                state->currentScreen = SCREEN_MAIN_MENU;
            }
            return;
        }
            
        case SCREEN_GAME:
            // Gérer le défilement du plateau avec la molette
            float wheel = GetMouseWheelMove();
            if (wheel != 0) {
                state->boardScrollY += wheel * 40.0f;
                // Limiter le défilement
                if (state->boardScrollY > 0) state->boardScrollY = 0;
                // Maximum 20 lignes * 90 pixels = 1800. Visible = 780.
                float maxScroll = -(20 * (TILE_HEIGHT + TILE_SPACING) - 730);
                if (state->boardScrollY < maxScroll) state->boardScrollY = maxScroll;
            }
            break;
    }
    
    // Gérer le tour de l'IA
    Player* currentPlayer = &state->game->players[state->game->currentPlayer];
    if (currentPlayer->isComputer && state->isRunning) {
        static float aiTimer = 0;
        aiTimer += deltaTime;
        
        if (aiTimer > 1.0f) { // Attendre 1 seconde pour que ce soit visible
            bool played = playAITurn(state->game, currentPlayer);
            
            if (played) {
                addMessage(&state->messageLog, TextFormat("%s (IA) a fini son tour", currentPlayer->name));
            } else {
                addMessage(&state->messageLog, TextFormat("%s (IA) ne peut pas jouer", currentPlayer->name));
            }
            
            nextPlayer(state->game);
            state->currentTurn++;
            state->turnStartBoard = state->game->board; // Update after AI turn
            aiTimer = 0;
            
            Player* nextP = &state->game->players[state->game->currentPlayer];
            addMessage(&state->messageLog, TextFormat("Tour %d - %s", state->currentTurn, nextP->name));
        }
        
        // Mettre à jour quand même les messages et synchroniser
        updateMessageLog(&state->messageLog, deltaTime);
        sync_board_to_display(state);
        return; // Ne pas traiter les entrées utilisateur pendant le tour de l'IA
    }
    
    // Mettre à jour les messages
    updateMessageLog(&state->messageLog, deltaTime);
    
    // Mettre à jour les boutons
    updateUIButton(&state->drawButton);
    updateUIButton(&state->validateButton);
    updateUIButton(&state->passButton);
    updateUIButton(&state->undoButton);
    
    if (isUIButtonClicked(&state->undoButton)) {
        // Annuler la dernière action (simplement remettre toutes les nouvelles tuiles dans la main pour l'instant)
        Player* currentPlayer = &state->game->players[state->game->currentPlayer];
        for (int r = 0; r < state->boardRows; r++) {
            for (int c = 0; c < state->boardCols; c++) {
                if (isTilePlacedThisTurn(state, r, c)) {
                    Tile t = state->boardTiles[r][c];
                    addTileToHand(currentPlayer, t);
                    board_remove_tile(&state->game->board, r, c);
                    state->boardTiles[r][c] = createTile(-1, NO_COLOR);
                }
            }
        }
        state->game->has_played_tile_this_turn = 0;
        addMessage(&state->messageLog, "Toutes les tuiles ont été annulées");
    }
    
    // Gérer les actions des boutons (AVANT handleMouseInput pour éviter les conflits)
    if (isUIButtonClicked(&state->drawButton)) {
        Player* currentPlayer = &state->game->players[state->game->currentPlayer];
        
        // Vérifier les restrictions : pioche max 1 fois par tour et impossible si tuile posée
        if (state->game->has_drawn_this_turn) {
            addMessage(&state->messageLog, "Vous avez déjà pioché ce tour !");
        } else if (state->game->has_played_tile_this_turn) {
            addMessage(&state->messageLog, "Impossible de piocher après avoir posé une tuile !");
        } else if (state->game->tilesInDeck > 0) {
            Tile newTile = drawTile(&state->game->board);
            addTileToHand(currentPlayer, newTile);
            state->game->tilesInDeck--;
            
            // Tracker les actions du tour
            state->game->moves_in_current_turn++;
            state->game->has_drawn_this_turn = 1;
            
            char msg[200];
            if (newTile.isJoker) {
                sprintf(msg, "%s a pioché un JOKER", currentPlayer->name);
            } else {
                sprintf(msg, "%s a pioché %d %s", currentPlayer->name, 
                       newTile.value, getColorName((int)newTile.color));
            }
            addMessage(&state->messageLog, msg);
        } else {
            addMessage(&state->messageLog, "Le sac est vide !");
        }
    }
    
    if (isUIButtonClicked(&state->validateButton)) {
        Player* currentPlayer = &state->game->players[state->game->currentPlayer];
        
        // 1. Vérifier si le joueur a posé au moins une tuile
        if (!state->game->has_played_tile_this_turn) {
            addMessage(&state->messageLog, "Vous devez poser au moins une tuile !");
        } else {
            // 2. Vérifier si TOUTES les combinaisons sur le plateau sont valides
            int errorCode = 0;
            if (!validate_entire_board(&state->game->board, &errorCode)) {
                if (errorCode == 1) addMessage(&state->messageLog, "Invalide : Mauvaise suite ou groupe !");
                else if (errorCode == 2) addMessage(&state->messageLog, "Invalide : Couleurs ou valeurs incorrectes !");
                else if (errorCode == 3) addMessage(&state->messageLog, "Invalide : Groupes de moins de 3 tuiles !");
                else addMessage(&state->messageLog, "Plateau invalide !");
            } else {
                // 3. Calculer les points pour l'initial meld (uniquement les nouvelles combinaisons)
                int totalPoints = 0;
                for (int i = 0; i < state->game->board.combinationCount; i++) {
                    // Pour le premier tour, on ne compte que les lignes qui étaient vides au début du tour
                    if (state->turnStartBoard.combinations[i].size == 0) {
                        Combination* combo = &state->game->board.combinations[i];
                        totalPoints += calculateCombinationPoints(combo->tiles, combo->size);
                    }
                }
                
                // 4. Appliquer la règle du premier tour (30 points)
                if (!currentPlayer->hasMadeInitialMeld) {
                    if (totalPoints < 30) {
                        addMessage(&state->messageLog, TextFormat("Premier tour : %d/30 points requis !", totalPoints));
                    } else {
                        addMessage(&state->messageLog, "Validation réussie ! (30+ points)");
                        currentPlayer->hasMadeInitialMeld = 1;
                        state->game->phase = MAIN_GAME;
                        
                        // Fin de tour
                        nextPlayer(state->game);
                        state->currentTurn++;
                        state->turnStartBoard = state->game->board; // Update for next player
                        Player* nextP = &state->game->players[state->game->currentPlayer];
                        addMessage(&state->messageLog, TextFormat("Tour %d - %s", state->currentTurn, nextP->name));
                    }
                } else {
                    // Tours suivants
                    addMessage(&state->messageLog, "Validation réussie !");
                    nextPlayer(state->game);
                    state->currentTurn++;
                    state->turnStartBoard = state->game->board; // Update for next player
                    Player* nextP = &state->game->players[state->game->currentPlayer];
                    addMessage(&state->messageLog, TextFormat("Tour %d - %s", state->currentTurn, nextP->name));
                }
            }
        }
    }
    
    if (isUIButtonClicked(&state->passButton)) {
        // Conditions pour passer :
        // 1. Soit le joueur a pioché ce tour-ci
        // 2. Soit le joueur n'a posé aucune tuile (il piochera automatiquement une tuile de pénalité)
        if (state->game->has_played_tile_this_turn) {
            addMessage(&state->messageLog, "Validez vos tuiles ou annulez avant de passer !");
        } else {
            if (!state->game->has_drawn_this_turn) {
                // Piocher une tuile de pénalité si on passe sans avoir pioché ni joué
                Player* currentPlayer = &state->game->players[state->game->currentPlayer];
                if (state->game->tilesInDeck > 0) {
                    Tile newTile = drawTile(&state->game->board);
                    addTileToHand(currentPlayer, newTile);
                    state->game->tilesInDeck--;
                    addMessage(&state->messageLog, "Passé : une tuile de pénalité piochée.");
                }
            }
            
            // Passer le tour
            nextPlayer(state->game);
            state->currentTurn++;
            state->turnStartBoard = state->game->board;
            Player* nextP = &state->game->players[state->game->currentPlayer];
            addMessage(&state->messageLog, TextFormat("Tour %d - %s", state->currentTurn, nextP->name));
        }
    }
    
    handleInput(state);
    handleMouseInput(state);
    
    // Synchroniser le board logique vers l'affichage après chaque mise à jour
    // Cela garantit que l'état graphique reflète toujours l'état logique
    sync_board_to_display(state);
}

// Rendre le jeu
void renderRaylibGame(RaylibGameState* state) {
    BeginDrawing();
    drawRaylibBackground();
    
    switch (state->currentScreen) {
        case SCREEN_MAIN_MENU: {
            // Titre stylé
            DrawText("RUMMIKUB", WINDOW_WIDTH/2 - MeasureText("RUMMIKUB", 80)/2, 100, 80, (RaylibColor){220, 180, 60, 255});
            
            // ISTY Logo ou texte
            DrawText("ISTY", 30, 30, 30, (RaylibColor){100, 100, 200, 255});
            
            // Boutons
            drawUIButton(&state->menuNewGame);
            drawUIButton(&state->menuRules);
            drawUIButton(&state->menuHistory);
            drawUIButton(&state->menuExit);
            
            // Crédits
            const char* credits = "IATIC-3 - Équipe: Jilani Hmida, Ghazel, Mathlouthi, Joudi";
            DrawText(credits, WINDOW_WIDTH/2 - MeasureText(credits, 20)/2, WINDOW_HEIGHT - 40, 20, LIGHTGRAY);
            break;
        }
        
        case SCREEN_RULES: {
            DrawText("RÈGLES DU JEU", 50, 50, 40, (RaylibColor){220, 180, 60, 255});
            const char* rules = "1. Chaque joueur commence avec 14 tuiles.\n"
                                "2. Pour poser la première fois, il faut 30 points minimum.\n"
                                "3. Les combinaisons valides sont :\n"
                                "   - Suites : Même couleur, chiffres consécutifs (ex: 3-4-5 rouge)\n"
                                "   - Groupes : Chiffres identiques, couleurs différentes (ex: 7 rouge, 7 bleu, 7 jaune)\n"
                                "4. On peut manipuler le plateau après avoir posé son premier tour.\n"
                                "5. Le premier à n'avoir plus de tuiles gagne !";
            
            // Afficher les règles sur plusieurs lignes
            int yPos = 150;
            int lineHeight = 30;
            const char* line = rules;
            char buffer[200];
            int bufIdx = 0;
            
            while (*line) {
                if (*line == '\n' || bufIdx >= 190) {
                    buffer[bufIdx] = '\0';
                    DrawText(buffer, 100, yPos, 22, WHITE);
                    yPos += lineHeight;
                    bufIdx = 0;
                    if (*line == '\n') { line++; continue; }
                }
                buffer[bufIdx++] = *line++;
            }
            if (bufIdx > 0) {
                buffer[bufIdx] = '\0';
                DrawText(buffer, 100, yPos, 22, WHITE);
            }
            
            drawUIButton(&state->backButton);
            break;
        }
        
        case SCREEN_HISTORY: {
            DrawText("HISTORIQUE DES JOUEURS", 50, 50, 40, (RaylibColor){220, 180, 60, 255});
            
            // Charger et afficher les scores
            PlayersList list;
            loadPlayers(&list);
            for (int i = 0; i < list.count && i < 15; i++) {
                DrawText(TextFormat("%d. %s", i+1, list.names[i]), 100, 150 + i * 30, 24, WHITE);
            }
            
            drawUIButton(&state->backButton);
            break;
        }
        
        case SCREEN_GAME: {
            // Logic de rendu du jeu existante (un peu modifiée pour le layout)
            drawUI(state);
            
            // Utiliser le scrolling pour le plateau
            // On peut utiliser une "ScissorMode" pour limiter l'affichage à une zone
            BeginScissorMode(BOARD_START_X, BOARD_START_Y, WINDOW_WIDTH - 300, 500);
            
            // Appliquer le scroll (un peu complexe car draw_board ne prend pas d'offset Y)
            // On va tricher en décalant BOARD_START_Y temporairement ou en modifiant draw_board
            // Pour l'instant, on affiche normalement, le scroll sera géré par un offset visuel
            
            // Plateau de jeu
            // Cacher la tuile si on est en train de la déplacer depuis le plateau
            Tile draggedTileBackup = createTile(-1, NO_COLOR);
            bool restoreTile = false;
            if (state->isDragging && state->isDraggingFromBoard) {
                draggedTileBackup = state->boardTiles[state->dragBoardRow][state->dragBoardCol];
                state->boardTiles[state->dragBoardRow][state->dragBoardCol] = createTile(-1, NO_COLOR);
                restoreTile = true;
            }
            
            // Rendu du plateau décalé par boardScrollY
            // Note: draw_board devrait être mis à jour pour accepter un offset
            // Mais on peut dessiner manuellement ici ou modifier draw_board
            
            // Dessiner manuellement le plateau avec scroll
            int startX = BOARD_START_X + 20;
            int startY = BOARD_START_Y + 50 + (int)state->boardScrollY;
            
            // Fond du plateau (fixe)
            EndScissorMode(); // On sort du scissor pour dessiner le fond fixe
            int boardW = WINDOW_WIDTH - BOARD_START_X - 300;
            int boardH = 780;  // Correspond à la nouvelle taille du plateau
            DrawRectangleRounded((Rectangle){BOARD_START_X, BOARD_START_Y, boardW, boardH}, 0.1f, 10, (RaylibColor){30, 50, 30, 255});
            DrawRectangleRoundedLines((Rectangle){BOARD_START_X, BOARD_START_Y, boardW, boardH}, 0.1f, 10, (RaylibColor){100, 150, 100, 255});
            BeginScissorMode(BOARD_START_X + 5, BOARD_START_Y + 40, boardW - 10, boardH - 50);
            
            for (int row = 0; row < state->boardRows; row++) {
                // Dessiner un rectangle autour de chaque combinaison non vide
                bool comboVisible = false;
                for (int col = 0; col < state->boardCols; col++) {
                    if (isTileValid(state->boardTiles[row][col])) {
                        comboVisible = true;
                        int x = startX + col * (TILE_WIDTH + TILE_SPACING);
                        int y = startY + row * (TILE_HEIGHT + TILE_SPACING);
                        draw_tile(state->boardTiles[row][col], x, y);
                    }
                }
                
                if (comboVisible) {
                    // Rectangle de regroupement
                    int rectY = startY + row * (TILE_HEIGHT + TILE_SPACING) - 5;
                    DrawRectangleRoundedLines((Rectangle){startX - 5, rectY, (TILE_WIDTH + TILE_SPACING) * MAX_COMBINATION_SIZE, TILE_HEIGHT + 10}, 0.2f, 10, (RaylibColor){100, 100, 100, 150});
                }
            }
            
            EndScissorMode();
            
            if (restoreTile) {
                state->boardTiles[state->dragBoardRow][state->dragBoardCol] = draggedTileBackup;
            }
            
            // Mettre en évidence les tuiles posées ce tour-ci
            BeginScissorMode(BOARD_START_X + 5, BOARD_START_Y + 40, boardW - 10, boardH - 50);
            for (int r = 0; r < state->boardRows; r++) {
                for (int c = 0; c < state->boardCols; c++) {
                    if (isTilePlacedThisTurn(state, r, c)) {
                        if (state->isDragging && state->isDraggingFromBoard && state->dragBoardRow == r && state->dragBoardCol == c) continue;
                        int x = startX + c * (TILE_WIDTH + TILE_SPACING);
                        int y = startY + r * (TILE_HEIGHT + TILE_SPACING);
                        DrawRectangleRoundedLines((Rectangle){x, y, TILE_WIDTH, TILE_HEIGHT}, 0.2f, 10, (RaylibColor){0, 255, 0, 255});
                    }
                }
            }
            EndScissorMode();
            
            // Informations et boutons (Right panel)
            drawInfoPanel(state);
            drawMessageLog(&state->messageLog, WINDOW_WIDTH - 280, 250, 260, 500);
            
            // Chevalet (Bottom)
            Player* currentPlayer = &state->game->players[state->game->currentPlayer];
            draw_rack_interactive(currentPlayer->hand, currentPlayer->tileCount, state->selectedTileIndex, state->hoveredTileIndex);
            
            // Boutons d'action
            drawUIButton(&state->drawButton);
            drawUIButton(&state->validateButton);
            drawUIButton(&state->passButton);
            drawUIButton(&state->undoButton);
            
            // Dragging tile
            if (state->isDragging) {
                Tile dragTile;
                bool hasTile = false;
                if (state->isDraggingFromBoard) {
                    dragTile = state->boardTiles[state->dragBoardRow][state->dragBoardCol];
                    hasTile = true;
                } else if (state->dragTileIndex >= 0 && state->dragTileIndex < currentPlayer->tileCount) {
                    dragTile = currentPlayer->hand[state->dragTileIndex];
                    hasTile = true;
                }
                
                if (hasTile) {
                    Vector2 mousePos = GetMousePosition();
                    int dragX = mousePos.x - state->dragOffsetX;
                    int dragY = mousePos.y - state->dragOffsetY;
                    RaylibColor fillColor = getTileColor(dragTile);
                    fillColor.a = 200;
                    DrawRectangleRounded((Rectangle){dragX, dragY, TILE_WIDTH, TILE_HEIGHT}, 0.2f, 10, fillColor);
                    DrawRectangleRoundedLines((Rectangle){dragX, dragY, TILE_WIDTH, TILE_HEIGHT}, 0.2f, 10, (RaylibColor){255, 255, 0, 255});
                    RaylibColor textColor = getTileTextColor(dragTile);
                    if (dragTile.isJoker) DrawText("JOKER", dragX + (TILE_WIDTH - MeasureText("JOKER", 14))/2, dragY + 30, 14, textColor);
                    else {
                        char valTxt[4]; sprintf(valTxt, "%d", dragTile.value);
                        DrawText(valTxt, dragX + (TILE_WIDTH - MeasureText(valTxt, 24))/2, dragY + 28, 24, textColor);
                    }
                }
            }
            break;
        }
        
        default: break;
    }
    
    EndDrawing();
}

// Nettoyer les ressources
void cleanupRaylibGame(RaylibGameState* state) {
    // Libérer le plateau
    if (state->boardTiles != NULL) {
        for (int i = 0; i < state->boardRows; i++) {
            if (state->boardTiles[i] != NULL) {
                free(state->boardTiles[i]);
            }
        }
        free(state->boardTiles);
    }
    
    CloseWindow();
}

// ============================================
// Interface de démarrage
// ============================================

void initStartMenu(StartMenuState* menu) {
    menu->currentInputIndex = 0;
    menu->playerCount = 2;  // Par défaut 2 joueurs, mais peut aller jusqu'à 4
    menu->showSavedPlayers = 0;
    menu->showError = 0;
    
    // Par défaut, seul le joueur 1 est humain, les autres aussi (IA décochée)
    for (int i = 0; i < 4; i++) {
        menu->playerIsAI[i] = 0;
    }
    
    // Créer les champs de texte pour les joueurs (jusqu'à 4)
    int startY = 200;
    int spacing = 80;
    for (int i = 0; i < 4; i++) {
        menu->playerInputs[i] = createTextInput(400, startY + i * spacing, 400, 50, 50, 24);
    }
    
    // Ajouter un bouton pour changer le nombre de joueurs
    // (on peut aussi permettre de sélectionner directement jusqu'à 4 champs)
    
    // Créer les boutons
    menu->startButton = createUIButton(400, 550, 180, 50, "Démarrer");
    menu->startButton.normalColor = (RaylibColor){80, 150, 80, 255};
    menu->startButton.hoverColor = (RaylibColor){100, 180, 100, 255};
    
    menu->cancelButton = createUIButton(600, 550, 180, 50, "Annuler");
    menu->cancelButton.normalColor = (RaylibColor){150, 80, 80, 255};
    menu->cancelButton.hoverColor = (RaylibColor){180, 100, 100, 255};
    
    // Charger les joueurs sauvegardés
    loadPlayers(&menu->savedPlayers);
}

void updateStartMenu(StartMenuState* menu) {
    // Mettre à jour les champs de texte
    for (int i = 0; i < 4; i++) {
        updateTextInput(&menu->playerInputs[i]);
    }
    
    // Mettre à jour les boutons
    updateUIButton(&menu->startButton);
    updateUIButton(&menu->cancelButton);
    
    // Navigation entre les champs avec Tab (tous les 4 joueurs possibles)
    if (IsKeyPressed(KEY_TAB)) {
        menu->currentInputIndex = (menu->currentInputIndex + 1) % 4;
        for (int i = 0; i < 4; i++) {
            setTextInputActive(&menu->playerInputs[i], (i == menu->currentInputIndex));
        }
    }
    
    // Gestion des clics sur les cases à cocher IA (joueurs 2 à 4)
    Vector2 mousePos = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        for (int i = 1; i < 4; i++) { // i=0 = joueur 1, toujours humain
            int baseY = 200 + i * 80;
            Rectangle checkbox = (Rectangle){820, baseY + 10, 24, 24};
            if (CheckCollisionPointRec(mousePos, checkbox)) {
                // On permet de cocher IA même si le nom est vide, la validation finale gère le cas
                menu->playerIsAI[i] = !menu->playerIsAI[i];
            }
        }
    }
}

void drawStartMenu(StartMenuState* menu) {
    BeginDrawing();
    
    // Fond
    drawRaylibBackground();
    
    // Titre
    DrawText("RUMMIKUB", 400, 50, 60, (RaylibColor){220, 180, 60, 255});
    DrawText("Configuration de la partie", 350, 120, 28, (RaylibColor){180, 180, 220, 255});
    
    // Instructions
    DrawText("Entrez les noms des joueurs (2-4 joueurs):", 300, 160, 20, 
            (RaylibColor){200, 200, 200, 255});
    
    // Champs de texte + cases IA
    for (int i = 0; i < 4; i++) {
        int baseY = 200 + i * 80;
        char label[30];
        sprintf(label, "Joueur %d:", i + 1);
        DrawText(label, 300, baseY + 15, 20, (RaylibColor){255, 255, 255, 255});
        drawTextInput(&menu->playerInputs[i]);
        
        // Case à cocher "Computer" (IA) pour tous les joueurs sauf le premier (ou tous?)
        // L'énoncé dit "except first"
        if (i > 0) {
            Rectangle checkbox = (Rectangle){820, baseY + 10, 24, 24};
            DrawRectangleLinesEx(checkbox, 2, (RaylibColor){200, 200, 200, 255});
            if (menu->playerIsAI[i]) {
                DrawRectangle(checkbox.x + 4, checkbox.y + 4, checkbox.width - 8, checkbox.height - 8, (RaylibColor){100, 200, 100, 255});
            }
            DrawText("Computer", checkbox.x + 32, checkbox.y + 2, 20, (RaylibColor){200, 200, 200, 255});
        }
    }
    
    // Boutons
    drawUIButton(&menu->startButton);
    drawUIButton(&menu->cancelButton);
    
    // Message d'erreur si nombre de joueurs invalide
    if (menu->showError) {
        const char* err = "Erreur : il faut entre 2 et 4 joueurs avec un nom.";
        int errWidth = MeasureText(err, 20);
        DrawText(err,
                 (WINDOW_WIDTH - errWidth) / 2,
                 520,
                 20,
                 (RaylibColor){255, 80, 80, 255});
    }
    
    // Afficher les joueurs sauvegardés
    if (menu->savedPlayers.count > 0) {
        DrawText("Joueurs sauvegardés:", 50, 200, 18, (RaylibColor){150, 200, 150, 255});
        for (int i = 0; i < menu->savedPlayers.count && i < 10; i++) {
            DrawText(menu->savedPlayers.names[i], 50, 230 + i * 25, 16, 
                    (RaylibColor){200, 200, 200, 255});
        }
    }
    
    EndDrawing();
}

int showStartMenu(StartMenuState* menu) {
    // Initialiser la fenêtre si pas déjà fait
    if (!IsWindowReady()) {
        InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Rummikub - Configuration");
        SetTargetFPS(60);
    }
    
    initStartMenu(menu);
    
    // Activer le premier champ
    setTextInputActive(&menu->playerInputs[0], 1);
    
    while (!WindowShouldClose()) {
        updateStartMenu(menu);
        drawStartMenu(menu);
        
        // Vérifier si on démarre
        if (isUIButtonClicked(&menu->startButton)) {
            // Compter les joueurs avec des noms
            menu->playerCount = 0;
            for (int i = 0; i < 4; i++) {
                if (strlen(menu->playerInputs[i].text) > 0) {
                    menu->playerCount++;
                }
            }
            
            // Permettre de 2 à 4 joueurs
            if (menu->playerCount >= 2 && menu->playerCount <= 4) {
                menu->showError = 0;
                // Sauvegarder les noms
                for (int i = 0; i < menu->playerCount; i++) {
                    addPlayer(&menu->savedPlayers, menu->playerInputs[i].text);
                }
                savePlayers(&menu->savedPlayers);
                return 1;  // Démarrer le jeu
            } else {
                // Afficher un message d'erreur graphique
                menu->showError = 1;
            }
        }
        
        // Vérifier si on annule
        if (isUIButtonClicked(&menu->cancelButton) || IsKeyPressed(KEY_ESCAPE)) {
            return 0;  // Annuler
        }
    }
    
    return 0;
}

// Fonction principale pour lancer le jeu graphique
void runRaylibGame(GameState* game) {
    RaylibGameState state;
    
    // Si le jeu n'est pas initialisé, on crée un objet GameState vide pour state.game
    GameState fallbackGame;
    if (game == NULL) {
        memset(&fallbackGame, 0, sizeof(GameState));
        game = &fallbackGame;
    }
    
    if (!initRaylibGame(&state, game)) {
        printf("Erreur: Impossible d'initialiser la fenêtre Raylib\n");
        return;
    }
    
    // Définir l'écran de départ
    state.currentScreen = SCREEN_MAIN_MENU;
    
    // Boucle principale
    while (!WindowShouldClose() && state.isRunning) {
        updateRaylibGame(&state);
        renderRaylibGame(&state);
    }
    
    cleanupRaylibGame(&state);
}


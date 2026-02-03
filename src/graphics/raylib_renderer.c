// src/graphics/raylib_renderer.c
// Utiliser le wrapper pour éviter le conflit Color
#include "tile_wrapper.h"

// Enfin inclure notre header
#include "raylib_renderer.h"
#include <stdio.h>
#include <string.h>

// Utiliser les constantes du wrapper
#define TILE_RED TILE_COLOR_RED
#define TILE_BLUE TILE_COLOR_BLUE
#define TILE_BLACK TILE_COLOR_BLACK
#define TILE_YELLOW TILE_COLOR_YELLOW
#define TILE_NO_COLOR TILE_COLOR_NO_COLOR

// Convertir la couleur de la tuile en couleur Raylib
// Utilise les valeurs numériques pour éviter le conflit avec l'enum Color
static RaylibColor getTileColor_internal(Tile tile) {
    if (tile.isJoker) {
        return (RaylibColor){120, 120, 120, 255};  // Gris pour joker
    }
    
    // Utiliser les valeurs numériques de l'enum Color du projet
    int colorValue = (int)tile.color;  // Cast direct car c'est un enum
    switch(colorValue) {
        case TILE_RED:
            return (RaylibColor){200, 50, 50, 255};
        case TILE_BLUE:
            return (RaylibColor){50, 100, 200, 255};
        case TILE_BLACK:
            return (RaylibColor){40, 40, 40, 255};
        case TILE_YELLOW:
            return (RaylibColor){220, 200, 50, 255};
        default:
            return (RaylibColor){150, 150, 150, 255};
    }
}

// Déterminer la couleur du texte selon la couleur de fond
// Utilise les valeurs numériques pour éviter le conflit
static RaylibColor getTileTextColor_internal(Tile tile) {
    if (tile.isJoker) {
        return (RaylibColor){255, 255, 255, 255};  // Blanc
    }
    
    // Utiliser les valeurs numériques
    int colorValue = (int)tile.color;  // Cast direct
    switch(colorValue) {
        case TILE_RED:
        case TILE_BLUE:
        case TILE_BLACK:
            return (RaylibColor){255, 255, 255, 255};  // Blanc
        case TILE_YELLOW:
            return (RaylibColor){0, 0, 0, 255};  // Noir
        default:
            return (RaylibColor){0, 0, 0, 255};  // Noir
    }
}

// Wrappers publics pour les fonctions de couleur
RaylibColor getTileColor(Tile tile) {
    return getTileColor_internal(tile);
}

RaylibColor getTileTextColor(Tile tile) {
    return getTileTextColor_internal(tile);
}

// Dessiner une tuile à une position donnée
void drawTileAt(Tile tile, int x, int y, int selected, int hovered) {
    // Ne dessiner que les tuiles valides (value > 0 ou joker)
    if (!tile.isJoker && tile.value <= 0) {
        return;  // Ne pas dessiner les tuiles invalides
    }
    
    RaylibColor fillColor = getTileColor_internal(tile);
    RaylibColor textColor = getTileTextColor_internal(tile);
    RaylibColor borderColor;
    
    // Couleur de la bordure selon l'état
    if (selected) {
        borderColor = (RaylibColor){255, 255, 0, 255};  // Jaune
    } else if (hovered) {
        borderColor = (RaylibColor){255, 255, 100, 255};  // Jaune clair
    } else {
        borderColor = (RaylibColor){0, 0, 0, 255};  // Noir
    }
    
    // Ombre si sélectionnée
    if (selected) {
        DrawRectangle(x + 3, y + 3, TILE_WIDTH, TILE_HEIGHT, (RaylibColor){0, 0, 0, 100});
    }
    
    // Rectangle principal avec coins arrondis
    DrawRectangleRounded((Rectangle){x, y, TILE_WIDTH, TILE_HEIGHT}, 0.2f, 10, fillColor);
    
    // Bordure
    int borderThickness = (selected || hovered) ? (selected ? 3 : 2) : 1;
    for (int i = 0; i < borderThickness; i++) {
        DrawRectangleRoundedLines((Rectangle){x - i, y - i, 
                                             TILE_WIDTH + 2*i, TILE_HEIGHT + 2*i}, 
                                 0.2f, 10, borderColor);
    }
    
    // Effet de relief léger
    DrawRectangleRoundedLines((Rectangle){x + 1, y + 1, TILE_WIDTH - 2, TILE_HEIGHT - 2}, 
                             0.2f, 10, (RaylibColor){255, 255, 255, 50});
    
    // Texte de la valeur
    if (tile.isJoker) {
        // Afficher "JOKER"
        int textWidth = MeasureText("JOKER", 14);
        int textX = x + (TILE_WIDTH - textWidth) / 2;
        DrawText("JOKER", textX, y + 30, 14, textColor);
    } else {
        // Afficher la valeur
        char valueText[4];
        sprintf(valueText, "%d", tile.value);
        int textWidth = MeasureText(valueText, 24);
        int textX = x + (TILE_WIDTH - textWidth) / 2;
        DrawText(valueText, textX, y + 28, 24, textColor);
    }
    
    // Indicateur de couleur dans le coin (petit carré)
    if (!tile.isJoker) {
        DrawRectangle(x + TILE_WIDTH - 15, y + 5, 10, 10, borderColor);
    }
}

// Dessiner le plateau de jeu (vide pour l'instant)
void drawBoard(Board* board) {
    // Zone du plateau
    int boardWidth = WINDOW_WIDTH - 300;
    int boardHeight = 500;
    
    // Fond du plateau
    DrawRectangleRounded((Rectangle){BOARD_START_X, BOARD_START_Y, 
                                    boardWidth, boardHeight}, 
                        0.1f, 10, (RaylibColor){30, 50, 30, 255});
    
    // Bordure du plateau
    DrawRectangleRoundedLines((Rectangle){BOARD_START_X, BOARD_START_Y, 
                                         boardWidth, boardHeight}, 
                             0.1f, 10, (RaylibColor){100, 150, 100, 255});
    
    // Titre du plateau
    DrawText("PLATEAU DE JEU", BOARD_START_X + 20, BOARD_START_Y + 10, 24, 
            (RaylibColor){200, 255, 200, 255});
    
    // Afficher les combinaisons si elles existent
    if (board->combinationCount > 0) {
        int y = BOARD_START_Y + 50;
        for (int i = 0; i < board->combinationCount; i++) {
            // Afficher chaque tuile de la combinaison
            for (int j = 0; j < board->combinations[i].size; j++) {
                int x = BOARD_START_X + 20 + j * (TILE_WIDTH + TILE_SPACING);
                drawTileAt(board->combinations[i].tiles[j], x, y, 0, 0);
            }
            y += TILE_HEIGHT + TILE_SPACING;
        }
    } else {
        // Message si le plateau est vide
        const char* message = "Le plateau est vide. Posez vos combinaisons ici.";
        int textWidth = MeasureText(message, 20);
        int textX = BOARD_START_X + (boardWidth - textWidth) / 2;
        DrawText(message, textX, BOARD_START_Y + boardHeight / 2, 20, 
                (RaylibColor){150, 150, 150, 255});
    }
}

// Dessiner le chevalet du joueur (14 tuiles)
void drawRack(Player* player, int selectedIndex) {
    // Fond du chevalet
    int rackWidth = WINDOW_WIDTH - 100;
    int rackHeight = 100;
    
    DrawRectangleRounded((Rectangle){RACK_START_X, RACK_START_Y, 
                                    rackWidth, rackHeight}, 
                        0.1f, 10, (RaylibColor){50, 30, 30, 255});
    
    // Bordure du chevalet
    DrawRectangleRoundedLines((Rectangle){RACK_START_X, RACK_START_Y, 
                                         rackWidth, rackHeight}, 
                             0.1f, 10, (RaylibColor){150, 100, 100, 255});
    
    // Titre du chevalet
    char rackTitle[100];
    sprintf(rackTitle, "CHEVALET - %s (%d tuiles)", player->name, player->tileCount);
    DrawText(rackTitle, RACK_START_X + 20, RACK_START_Y + 10, 20, 
            (RaylibColor){255, 200, 200, 255});
    
    // Calculer l'espacement pour centrer les tuiles
    int totalTilesWidth = player->tileCount * TILE_WIDTH + 
                         (player->tileCount - 1) * TILE_SPACING;
    int startX = RACK_START_X + (rackWidth - totalTilesWidth) / 2;
    
    // Dessiner chaque tuile du chevalet
    for (int i = 0; i < player->tileCount; i++) {
        int x = startX + i * (TILE_WIDTH + TILE_SPACING);
        int y = RACK_START_Y + 35;
        
        int selected = (i == selectedIndex);
        int hovered = 0;  // Sera géré par la détection de la souris
        
        drawTileAt(player->hand[i], x, y, selected, hovered);
    }
}

// Obtenir le nom de la couleur (pour debug/affichage)
const char* getColorName(int colorValue) {
    switch(colorValue) {
        case TILE_RED: return "ROUGE";
        case TILE_BLUE: return "BLEU";
        case TILE_BLACK: return "NOIR";
        case TILE_YELLOW: return "JAUNE";
        case TILE_NO_COLOR: return "JOKER";
        default: return "INCONNU";
    }
}

// Dessiner le fond de la fenêtre
void drawRaylibBackground() {
    // Dégradé de fond
    RaylibColor topColor = (RaylibColor){30, 30, 60, 255};
    RaylibColor bottomColor = (RaylibColor){10, 10, 30, 255};
    
    DrawRectangleGradientV(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, topColor, bottomColor);
}

// ============================================
// Nouvelles fonctions graphiques simplifiées
// ============================================

// Dessine un rectangle de la bonne couleur avec le chiffre au centre
void draw_tile(Tile tile, int x, int y) {
    draw_tile_selected(tile, x, y, 0);
}

// Version avec sélection visuelle (bordure différente)
void draw_tile_selected(Tile tile, int x, int y, int selected) {
    // Ne dessiner que les tuiles valides (value > 0 ou joker)
    if (!tile.isJoker && tile.value <= 0) {
        return;  // Ne pas dessiner les tuiles invalides
    }
    
    RaylibColor fillColor = getTileColor_internal(tile);
    RaylibColor textColor = getTileTextColor_internal(tile);
    RaylibColor borderColor;
    int borderThickness;
    
    // Choisir la couleur et l'épaisseur de la bordure selon la sélection
    if (selected) {
        borderColor = (RaylibColor){255, 255, 0, 255};  // Jaune pour sélection
        borderThickness = 3;
    } else {
        borderColor = (RaylibColor){0, 0, 0, 255};  // Noir par défaut
        borderThickness = 1;
    }
    
    // Ombre si sélectionnée
    if (selected) {
        DrawRectangle(x + 3, y + 3, TILE_WIDTH, TILE_HEIGHT, (RaylibColor){0, 0, 0, 100});
    }
    
    // Rectangle principal avec coins arrondis
    DrawRectangleRounded((Rectangle){x, y, TILE_WIDTH, TILE_HEIGHT}, 0.2f, 10, fillColor);
    
    // Bordure
    for (int i = 0; i < borderThickness; i++) {
        DrawRectangleRoundedLines((Rectangle){x - i, y - i, 
                                             TILE_WIDTH + 2*i, TILE_HEIGHT + 2*i}, 
                                 0.2f, 10, borderColor);
    }
    
    // Texte de la valeur au centre
    if (tile.isJoker) {
        // Afficher "JOKER"
        int textWidth = MeasureText("JOKER", 14);
        int textX = x + (TILE_WIDTH - textWidth) / 2;
        DrawText("JOKER", textX, y + 30, 14, textColor);
    } else {
        // Afficher la valeur
        char valueText[4];
        sprintf(valueText, "%d", tile.value);
        int textWidth = MeasureText(valueText, 24);
        int textX = x + (TILE_WIDTH - textWidth) / 2;
        DrawText(valueText, textX, y + 28, 24, textColor);
    }
}

// Affiche le plateau de jeu (tableau 2D de tuiles)
void draw_board(Tile** board, int rows, int cols) {
    if (board == NULL) return;
    
    // Zone du plateau (étendue jusqu'aux boutons)
    int boardWidth = WINDOW_WIDTH - BOARD_START_X - 300;
    int boardHeight = 620;  // De BOARD_START_Y (80) jusqu'à BUTTONS_Y (720) - 20px marge
    
    // Fond du plateau
    DrawRectangleRounded((Rectangle){BOARD_START_X, BOARD_START_Y, 
                                    boardWidth, boardHeight}, 
                        0.1f, 10, (RaylibColor){30, 50, 30, 255});
    
    // Bordure du plateau
    DrawRectangleRoundedLines((Rectangle){BOARD_START_X, BOARD_START_Y, 
                                         boardWidth, boardHeight}, 
                             0.1f, 10, (RaylibColor){100, 150, 100, 255});
    
    // Titre du plateau
    DrawText("PLATEAU DE JEU", BOARD_START_X + 20, BOARD_START_Y + 10, 24, 
            (RaylibColor){200, 255, 200, 255});
    
    // Afficher les tuiles du plateau
    int startX = BOARD_START_X + 20;
    int startY = BOARD_START_Y + 50;
    
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            if (board[row] != NULL && board[row][col].value > 0) {
                // Dessiner la tuile seulement si elle existe (valeur > 0)
                int x = startX + col * (TILE_WIDTH + TILE_SPACING);
                int y = startY + row * (TILE_HEIGHT + TILE_SPACING);
                draw_tile(board[row][col], x, y);
            } else if (board[row] != NULL && board[row][col].isJoker) {
                // Dessiner aussi les jokers (value == 0 mais isJoker == 1)
                int x = startX + col * (TILE_WIDTH + TILE_SPACING);
                int y = startY + row * (TILE_HEIGHT + TILE_SPACING);
                draw_tile(board[row][col], x, y);
            }
        }
    }
}

// Affiche le chevalet du joueur en bas de l'écran
void draw_rack(Tile* rack, int size) {
    draw_rack_interactive(rack, size, -1, -1);
}

// Version interactive avec sélection et survol
void draw_rack_interactive(Tile* rack, int size, int selectedIndex, int hoveredIndex) {
    if (rack == NULL || size == 0) return;
    
    // Fond du chevalet (position ajustée)
    int rackWidth = WINDOW_WIDTH - RACK_START_X - 300;
    int rackHeight = 110;
    
    DrawRectangleRounded((Rectangle){RACK_START_X, RACK_START_Y, 
                                    rackWidth, rackHeight}, 
                        0.1f, 10, (RaylibColor){50, 30, 30, 255});
    
    // Bordure du chevalet
    DrawRectangleRoundedLines((Rectangle){RACK_START_X, RACK_START_Y, 
                                         rackWidth, rackHeight}, 
                             0.1f, 10, (RaylibColor){150, 100, 100, 255});
    
    // Titre du chevalet
    DrawText("CHEVALET", RACK_START_X + 20, RACK_START_Y + 10, 20, 
            (RaylibColor){255, 200, 200, 255});
    
    // Calculer l'espacement pour centrer les tuiles
    int totalTilesWidth = size * TILE_WIDTH + (size - 1) * TILE_SPACING;
    int startX = RACK_START_X + (rackWidth - totalTilesWidth) / 2;
    
    // Dessiner chaque tuile du chevalet
    for (int i = 0; i < size; i++) {
        int x = startX + i * (TILE_WIDTH + TILE_SPACING);
        int y = RACK_START_Y + 35;
        
        // Déterminer si la tuile est sélectionnée ou survolée
        int isSelected = (i == selectedIndex);
        int isHovered = (i == hoveredIndex && selectedIndex == -1);
        
        draw_tile_selected(rack[i], x, y, isSelected || isHovered);
    }
}

// Affiche le score en haut à droite
void draw_score(int score) {
    char scoreText[50];
    sprintf(scoreText, "Score: %d", score);
    
    int textWidth = MeasureText(scoreText, 24);
    int x = WINDOW_WIDTH - textWidth - 20;
    int y = 10;
    
    // Fond semi-transparent pour le score
    DrawRectangle(x - 10, y - 5, textWidth + 20, 35, (RaylibColor){0, 0, 0, 150});
    
    // Texte du score
    DrawText(scoreText, x, y, 24, (RaylibColor){255, 255, 0, 255});  // Jaune
}

// ============================================
// Fonctions d'interaction souris
// ============================================

// Retourne l'index de la tuile du chevalet à la position de la souris, ou -1 si aucune
int getRackTileAtPosition(int mouseX, int mouseY, Tile* rack, int size, int* outIndex) {
    if (rack == NULL || size == 0) {
        if (outIndex) *outIndex = -1;
        return 0;
    }
    
    int rackWidth = WINDOW_WIDTH - RACK_START_X - 300;
    int totalTilesWidth = size * TILE_WIDTH + (size - 1) * TILE_SPACING;
    int startX = RACK_START_X + (rackWidth - totalTilesWidth) / 2;
    int startY = RACK_START_Y + 35;
    
    for (int i = 0; i < size; i++) {
        int x = startX + i * (TILE_WIDTH + TILE_SPACING);
        int y = startY;
        
        Rectangle tileRect = {x, y, TILE_WIDTH, TILE_HEIGHT};
        if (CheckCollisionPointRec((Vector2){mouseX, mouseY}, tileRect)) {
            if (outIndex) *outIndex = i;
            return 1;
        }
    }
    
    if (outIndex) *outIndex = -1;
    return 0;
}

// Retourne la position du plateau (row, col) à la position de la souris, ou -1 si hors plateau
int getBoardPositionAtMouse(int mouseX, int mouseY, int* outRow, int* outCol) {
    return getBoardPositionAtMouseExtended(mouseX, mouseY, 0.0f, outRow, outCol);
}

int getBoardPositionAtMouseExtended(int mouseX, int mouseY, float scrollY, int* outRow, int* outCol) {
    if (outRow) *outRow = -1;
    if (outCol) *outCol = -1;
    
    int boardWidth = WINDOW_WIDTH - BOARD_START_X - 300;
    int boardHeight = 620;
    int startX = BOARD_START_X + 20;
    int startY = BOARD_START_Y + 50; // On ne rajoute pas le scroll ici car startY définit le début visuel
    
    // Vérifier si la souris est dans la zone du plateau (zone fixe)
    if (mouseX < BOARD_START_X || mouseX > BOARD_START_X + boardWidth ||
        mouseY < BOARD_START_Y || mouseY > BOARD_START_Y + boardHeight) {
        return 0;
    }
    
    // Calculer la position relative en tenant compte du scroll
    int relX = mouseX - startX;
    int relY = mouseY - (startY + (int)scrollY);
    
    // Calculer la colonne et la ligne
    int col = relX / (TILE_WIDTH + TILE_SPACING);
    int row = relY / (TILE_HEIGHT + TILE_SPACING);
    
    // Empêcher les lignes négatives à cause du scroll
    if (row < 0) return 0;
    
    // Vérifier si on est dans une tuile (pas dans l'espace entre les tuiles)
    int tileX = col * (TILE_WIDTH + TILE_SPACING);
    int tileY = row * (TILE_HEIGHT + TILE_SPACING);
    
    if (relX >= tileX && relX < tileX + TILE_WIDTH &&
        relY >= tileY && relY < tileY + TILE_HEIGHT) {
        if (outRow) *outRow = row;
        if (outCol) *outCol = col;
        return 1;
    }
    
    return 0;
}

// Vérifie si une position sur le plateau est valide (vide ou peut être remplacée)
int isValidBoardPosition(int row, int col, Tile** board, int rows, int cols) {
    if (row < 0 || row >= rows || col < 0 || col >= cols) {
        return 0;  // Out of bounds
    }
    
    if (board == NULL || board[row] == NULL) {
        return 1;  // Should not happen with current init, but safe
    }
    
    // Position is valid if it's empty (value <= 0 and not a joker)
    return (board[row][col].value <= 0 && !board[row][col].isJoker);
}


// src/graphics/raylib_renderer.h
#ifndef RAYLIB_RENDERER_H
#define RAYLIB_RENDERER_H

// Note: Les types Tile, Board, Player sont définis dans les headers du projet
// mais ne sont pas inclus ici pour éviter le conflit avec le type Color de Raylib.
// Ils sont inclus dans raylib_renderer.c après résolution du conflit.

// Dimensions de la fenêtre (optimisée pour écrans standards)
#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 900

// Dimensions des tuiles
#define TILE_WIDTH 60
#define TILE_HEIGHT 80
#define TILE_SPACING 10

// Positions
#define BOARD_START_X 250
#define BOARD_START_Y 80
#define RACK_START_X 250
#define RACK_START_Y 790
#define BUTTONS_Y 720

// Structure pour le rendu d'une tuile
typedef struct {
    int x, y;
    int selected;
    int hovered;
} TileRenderState;

// Fonctions de rendu des tuiles
// Note: Les fonctions retournent Color de Raylib, mais on ne peut pas l'exposer ici
// à cause du conflit avec l'enum Color du projet. Utilisez ces fonctions dans les .c
void drawTileAt(Tile tile, int x, int y, int selected, int hovered);

// Fonctions utilitaires de couleur (déclarées dans le .c avec RaylibColor)
// Note: Ces fonctions retournent RaylibColor mais on ne peut pas l'exposer dans le header
// Elles sont déclarées dans raylib_renderer.c et peuvent être utilisées via tile_wrapper.h

// Fonctions de rendu du plateau
void drawBoard(Board* board);

// Fonctions de rendu du chevalet
void drawRack(Player* player, int selectedIndex);

// Fonctions utilitaires
const char* getColorName(int colorValue);  // Utilise int pour éviter le conflit
void drawRaylibBackground();  // Renommé pour éviter le conflit avec simple_graphics

// Nouvelles fonctions graphiques simplifiées
// Note: Les types Tile sont définis via tile_wrapper.h dans le .c
void draw_tile(Tile tile, int x, int y);
void draw_tile_selected(Tile tile, int x, int y, int selected);  // Version avec sélection
void draw_board(Tile** board, int rows, int cols);
void draw_rack(Tile* rack, int size);
void draw_rack_interactive(Tile* rack, int size, int selectedIndex, int hoveredIndex);  // Version interactive
void draw_score(int score);

// Fonctions d'interaction
int getRackTileAtPosition(int mouseX, int mouseY, Tile* rack, int size, int* outIndex);
int getBoardPositionAtMouse(int mouseX, int mouseY, int* outRow, int* outCol);
int getBoardPositionAtMouseExtended(int mouseX, int mouseY, float scrollY, int* outRow, int* outCol);
int isValidBoardPosition(int row, int col, Tile** board, int rows, int cols);

#endif


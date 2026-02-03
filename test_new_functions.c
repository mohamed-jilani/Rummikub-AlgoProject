// test_new_functions.c - Test des nouvelles fonctions graphiques
#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"
#include "src/graphics/tile_wrapper.h"  // Inclure le wrapper pour avoir les types
#include "src/graphics/raylib_renderer.h"
#include "src/utils/random.h"

int main() {
    printf("=== Test des nouvelles fonctions graphiques ===\n");
    
    // Initialiser le générateur aléatoire
    initRandom();
    
    // Créer la fenêtre
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Test Nouvelles Fonctions Graphiques");
    SetTargetFPS(60);
    
    // Créer quelques tuiles de test
    Tile testTiles[14];
    for (int i = 0; i < 14; i++) {
        TileColor colors[] = {RED, BLUE, BLACK, YELLOW};
        testTiles[i] = createTile((i % 13) + 1, colors[i % 4]);
    }
    
    // Créer un plateau de test (3x5)
    int boardRows = 3;
    int boardCols = 5;
    Tile** board = (Tile**)malloc(boardRows * sizeof(Tile*));
    for (int i = 0; i < boardRows; i++) {
        board[i] = (Tile*)calloc(boardCols, sizeof(Tile));
    }
    
    // Placer quelques tuiles sur le plateau
    board[0][0] = createTile(1, RED);
    board[0][1] = createTile(2, RED);
    board[0][2] = createTile(3, RED);
    board[1][0] = createTile(7, BLUE);
    board[1][1] = createTile(7, YELLOW);
    board[1][2] = createTile(7, BLACK);
    
    int score = 45;
    
    printf("Fenêtre ouverte. Fermez-la pour quitter.\n");
    
    // Boucle principale
    while (!WindowShouldClose()) {
        BeginDrawing();
        
        // Fond
        drawRaylibBackground();
        
        // Dessiner le score
        draw_score(score);
        
        // Dessiner le plateau
        draw_board(board, boardRows, boardCols);
        
        // Dessiner le chevalet
        draw_rack(testTiles, 14);
        
        // Instructions
        DrawText("Nouvelles fonctions graphiques", 50, 50, 24, 
                (RaylibColor){255, 255, 255, 255});
        DrawText("draw_tile() - dessine une tuile", 50, 80, 18, 
                (RaylibColor){200, 200, 200, 255});
        DrawText("draw_board() - affiche le plateau", 50, 100, 18, 
                (RaylibColor){200, 200, 200, 255});
        DrawText("draw_rack() - affiche le chevalet", 50, 120, 18, 
                (RaylibColor){200, 200, 200, 255});
        DrawText("draw_score() - affiche le score", 50, 140, 18, 
                (RaylibColor){200, 200, 200, 255});
        
        EndDrawing();
    }
    
    // Libérer la mémoire
    for (int i = 0; i < boardRows; i++) {
        free(board[i]);
    }
    free(board);
    
    CloseWindow();
    
    printf("Test terminé.\n");
    return 0;
}

// src/graphics/minimal_graphics.c
#include <stdio.h>
#include "raylib.h"

// On évite d'inclure les fichiers qui ont des conflits
// On va créer une structure Tile simple pour tester

typedef struct {
    int value;
    int color;  // 0=RED, 1=BLUE, 2=BLACK, 3=YELLOW, 4=JOKER
    int isJoker;
} SimpleTile;

// Fonction pour convertir notre couleur en couleur Raylib
Color getSimpleColor(SimpleTile tile) {
    if (tile.isJoker) return GRAY;
    
    switch(tile.color) {
        case 0: return RED;     // RED
        case 1: return BLUE;    // BLUE  
        case 2: return BLACK;   // BLACK
        case 3: return YELLOW;  // YELLOW
        default: return WHITE;
    }
}

// Créer une tuile simple
SimpleTile createSimpleTile(int value, int color, int isJoker) {
    SimpleTile tile;
    tile.value = value;
    tile.color = color;
    tile.isJoker = isJoker;
    return tile;
}

// Dessiner une tuile
void drawSimpleTile(SimpleTile tile, int x, int y) {
    // Rectangle de la tuile
    DrawRectangle(x, y, 60, 80, getSimpleColor(tile));
    DrawRectangleLines(x, y, 60, 80, BLACK);
    
    // Texte
    if (tile.isJoker) {
        DrawText("JOKER", x + 10, y + 30, 15, WHITE);
    } else {
        char text[10];
        sprintf(text, "%d", tile.value);
        DrawText(text, x + 25, y + 35, 20, WHITE);
    }
}

// Fonction de démonstration graphique ULTRA SIMPLE
void runMinimalGraphicalDemo() {
    const int screenWidth = 1000;
    const int screenHeight = 600;
    
    InitWindow(screenWidth, screenHeight, "Rummikub - Démo Graphique");
    SetTargetFPS(60);
    
    // Créer quelques tuiles de test
    SimpleTile tiles[6];
    tiles[0] = createSimpleTile(7, 0, 0);  // 7 ROUGE
    tiles[1] = createSimpleTile(7, 1, 0);  // 7 BLEU
    tiles[2] = createSimpleTile(7, 3, 0);  // 7 JAUNE
    tiles[3] = createSimpleTile(1, 0, 0);  // 1 ROUGE
    tiles[4] = createSimpleTile(2, 0, 0);  // 2 ROUGE
    tiles[5] = createSimpleTile(3, 0, 0);  // 3 ROUGE
    
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        // Titre
        DrawText("RUMMIKUB - Démonstration Graphique", 200, 20, 40, DARKBLUE);
        DrawText("Projet Algorithmique - ISTY IATIC3", 250, 70, 20, DARKGRAY);
        
        // Exemple de série (même valeur, couleurs différentes)
        DrawText("Exemple de SÉRIE valide:", 100, 120, 25, DARKGREEN);
        drawSimpleTile(tiles[0], 100, 160);  // 7 ROUGE
        drawSimpleTile(tiles[1], 180, 160);  // 7 BLEU
        drawSimpleTile(tiles[2], 260, 160);  // 7 JAUNE
        DrawText("Même valeur (7), couleurs différentes", 100, 260, 18, GRAY);
        
        // Exemple de suite (même couleur, valeurs consécutives)
        DrawText("Exemple de SUITE valide:", 500, 120, 25, DARKPURPLE);
        drawSimpleTile(tiles[3], 500, 160);  // 1 ROUGE
        drawSimpleTile(tiles[4], 580, 160);  // 2 ROUGE
        drawSimpleTile(tiles[5], 660, 160);  // 3 ROUGE
        DrawText("Même couleur (rouge), valeurs consécutives", 500, 260, 18, GRAY);
        
        // Joker
        DrawText("Joker:", 100, 320, 25, MAROON);
        SimpleTile joker = createSimpleTile(0, 4, 1);
        drawSimpleTile(joker, 100, 360);
        DrawText("Remplace n'importe quelle tuile", 180, 390, 18, GRAY);
        
        // Instructions
        DrawText("Instructions:", 100, 450, 25, DARKBROWN);
        DrawText("- Version console complète disponible", 120, 490, 18, GRAY);
        DrawText("- Interface graphique en développement", 120, 520, 18, GRAY);
        DrawText("- Appuyez sur ÉCHAP pour quitter", 120, 550, 18, GRAY);
        
        EndDrawing();
    }
    
    CloseWindow();
}
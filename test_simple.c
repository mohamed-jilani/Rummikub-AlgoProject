// test_simple.c
#include "raylib.h"

int main() {
    InitWindow(800, 600, "Test Raylib");
    SetTargetFPS(60);
    
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        DrawText("TEST RAYLIB REUSSI!", 200, 250, 40, BLUE);
        DrawText("Si tu vois ce texte, Raylib fonctionne.", 150, 320, 20, DARKGRAY);
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
// src/graphics/game_over_screen.c
#include "game_over_screen.h"
#include "ui_components.h"
#include "../logic/scoring.h"
#include "../data/scores_manager.h"
#include "raylib_renderer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void initGameOverScreen(GameOverScreen* screen, GameState* game) {
    screen->game = game;
    screen->animationTime = 0.0f;
    screen->showScores = 0;
    
    // Calculer les scores finaux
    screen->playerScores = (int*)malloc(game->playerCount * sizeof(int));
    screen->playerRanks = (int*)malloc(game->playerCount * sizeof(int));
    
    if(!screen->playerScores || !screen->playerRanks) {
        screen->playerScores = NULL;
        screen->playerRanks = NULL;
        return;
    }
    
    // Calculer les points de chaque joueur
    for(int i = 0; i < game->playerCount; i++) {
        screen->playerScores[i] = calculateHandPoints(&game->players[i]);
    }
    
    // Déterminer le gagnant (celui avec 0 points ou le moins de points)
    screen->winnerIndex = -1;
    int minScore = 1000;
    for(int i = 0; i < game->playerCount; i++) {
        if(screen->playerScores[i] == 0) {
            screen->winnerIndex = i;
            break;
        }
        if(screen->playerScores[i] < minScore) {
            minScore = screen->playerScores[i];
            screen->winnerIndex = i;
        }
    }
    
    // Calculer les rangs
    for(int i = 0; i < game->playerCount; i++) {
        int rank = 1;
        for(int j = 0; j < game->playerCount; j++) {
            if(i != j && screen->playerScores[j] < screen->playerScores[i]) {
                rank++;
            }
        }
        screen->playerRanks[i] = rank;
    }
    
    // Sauvegarder les scores
    for(int i = 0; i < game->playerCount; i++) {
        saveScore(game->players[i].name, game->players[i].score);
    }
    
    // Bouton retour au menu
    Button backButton = createUIButton(
        WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT - 100, 200, 50, "Retour au menu");
    backButton.normalColor = (RaylibColor){80, 150, 80, 255};
    backButton.hoverColor = (RaylibColor){100, 180, 100, 255};
    screen->backToMenuButton = backButton;
}

void updateGameOverScreen(GameOverScreen* screen, float deltaTime) {
    screen->animationTime += deltaTime;
    
    // Afficher les scores après 1 seconde
    if(screen->animationTime > 1.0f) {
        screen->showScores = 1;
    }
    
    updateUIButton(&screen->backToMenuButton);
}

void drawGameOverScreen(GameOverScreen* screen) {
    BeginDrawing();
    ClearBackground((RaylibColor){20, 20, 30, 255});
    
    // Titre
    const char* title = "FIN DE PARTIE";
    int titleWidth = MeasureText(title, 60);
    float titleY = 50.0f + sinf(screen->animationTime * 2.0f) * 5.0f;  // Animation légère
    DrawText(title, (WINDOW_WIDTH - titleWidth) / 2, (int)titleY, 60,
            (RaylibColor){255, 255, 255, 255});
    
    if(screen->showScores && screen->playerScores && screen->playerRanks) {
        // Afficher le gagnant
        if(screen->winnerIndex >= 0) {
            const char* winnerName = screen->game->players[screen->winnerIndex].name;
            char winnerText[200];
            sprintf(winnerText, "🏆 VAINQUEUR: %s ! 🏆", winnerName);
            
            int winnerWidth = MeasureText(winnerText, 40);
            DrawText(winnerText, (WINDOW_WIDTH - winnerWidth) / 2, 150, 40,
                    (RaylibColor){255, 215, 0, 255});
        }
        
        // Afficher les scores
        int startY = 250;
        int lineHeight = 50;
        
        // En-tête
        DrawText("Rang", 200, startY, 24, (RaylibColor){255, 255, 255, 255});
        DrawText("Joueur", 350, startY, 24, (RaylibColor){255, 255, 255, 255});
        DrawText("Points restants", 600, startY, 24, (RaylibColor){255, 255, 255, 255});
        DrawText("Score total", 850, startY, 24, (RaylibColor){255, 255, 255, 255});
        
        // Lignes de séparation
        DrawLine(150, startY + 30, WINDOW_WIDTH - 150, startY + 30,
                (RaylibColor){100, 100, 100, 255});
        
        // Afficher chaque joueur
        for(int i = 0; i < screen->game->playerCount; i++) {
            int y = startY + 50 + i * lineHeight;
            
            // Animation d'apparition
            float alpha = 1.0f;
            if(screen->animationTime < 2.0f + i * 0.3f) {
                alpha = (screen->animationTime - 1.0f - i * 0.3f) / 0.3f;
                if(alpha < 0.0f) alpha = 0.0f;
                if(alpha > 1.0f) alpha = 1.0f;
            }
            
            RaylibColor textColor = (RaylibColor){255, 255, 255, (unsigned char)(alpha * 255)};
            
            // Rang avec médaille
            char rankText[20];
            if(screen->playerRanks[i] == 1) {
                sprintf(rankText, "🥇 1er");
            } else if(screen->playerRanks[i] == 2) {
                sprintf(rankText, "🥈 2ème");
            } else if(screen->playerRanks[i] == 3) {
                sprintf(rankText, "🥉 3ème");
            } else {
                sprintf(rankText, "%dème", screen->playerRanks[i]);
            }
            DrawText(rankText, 200, y, 20, textColor);
            
            // Nom du joueur
            DrawText(screen->game->players[i].name, 350, y, 20, textColor);
            
            // Points restants
            char pointsText[20];
            sprintf(pointsText, "%d", screen->playerScores[i]);
            DrawText(pointsText, 600, y, 20, textColor);
            
            // Score total
            char scoreText[20];
            sprintf(scoreText, "%d", screen->game->players[i].score);
            DrawText(scoreText, 850, y, 20, textColor);
        }
    }
    
    // Bouton retour au menu
    drawUIButton(&screen->backToMenuButton);
    
    EndDrawing();
}

int isBackToMenuClicked(GameOverScreen* screen) {
    return isUIButtonClicked(&screen->backToMenuButton);
}

void cleanupGameOverScreen(GameOverScreen* screen) {
    if(screen->playerScores) {
        free(screen->playerScores);
        screen->playerScores = NULL;
    }
    if(screen->playerRanks) {
        free(screen->playerRanks);
        screen->playerRanks = NULL;
    }
}


// src/graphics/timer.c
#include "timer.h"
#include "tile_wrapper.h"
#include <stdio.h>

void initTimer(GameTimer* timer, float maxTime) {
    timer->maxTime = maxTime;
    timer->timeRemaining = maxTime;
    timer->isActive = 0;
    timer->isPaused = 0;
    timer->hasExpired = 0;
    timer->warningTime = 10.0f;  // Avertissement à 10 secondes
}

void startTimer(GameTimer* timer) {
    timer->isActive = 1;
    timer->isPaused = 0;
    timer->hasExpired = 0;
    timer->timeRemaining = timer->maxTime;
}

void pauseTimer(GameTimer* timer) {
    timer->isPaused = 1;
}

void resumeTimer(GameTimer* timer) {
    timer->isPaused = 0;
}

void resetTimer(GameTimer* timer) {
    timer->timeRemaining = timer->maxTime;
    timer->hasExpired = 0;
}

void updateTimer(GameTimer* timer, float deltaTime) {
    if(!timer->isActive || timer->isPaused) return;
    
    timer->timeRemaining -= deltaTime;
    
    if(timer->timeRemaining <= 0.0f) {
        timer->timeRemaining = 0.0f;
        timer->hasExpired = 1;
        timer->isActive = 0;
    }
}

void drawTimer(GameTimer* timer, int x, int y, int fontSize) {
    char timeStr[20];
    int minutes = (int)(timer->timeRemaining / 60.0f);
    int seconds = (int)(timer->timeRemaining) % 60;
    sprintf(timeStr, "%02d:%02d", minutes, seconds);
    
    // Choisir la couleur selon le temps restant
    RaylibColor color;
    if(timer->hasExpired) {
        color = (RaylibColor){255, 0, 0, 255};  // Rouge
    } else if(timer->timeRemaining <= timer->warningTime) {
        // Clignoter en rouge/orange
        float blink = (int)(timer->timeRemaining * 2) % 2;
        if(blink) {
            color = (RaylibColor){255, 100, 0, 255};  // Orange
        } else {
            color = (RaylibColor){255, 0, 0, 255};  // Rouge
        }
    } else {
        color = (RaylibColor){255, 255, 255, 255};  // Blanc
    }
    
    // Fond semi-transparent
    int textWidth = MeasureText(timeStr, fontSize);
    DrawRectangle(x - 10, y - 5, textWidth + 20, fontSize + 10, 
                 (RaylibColor){0, 0, 0, 150});
    
    // Texte
    DrawText(timeStr, x, y, fontSize, color);
    
    // Barre de progression
    float progress = timer->timeRemaining / timer->maxTime;
    int barWidth = 200;
    int barHeight = 5;
    int barX = x;
    int barY = y + fontSize + 5;
    
    // Fond de la barre
    DrawRectangle(barX, barY, barWidth, barHeight, (RaylibColor){50, 50, 50, 255});
    
    // Barre de progression
    RaylibColor barColor;
    if(progress > 0.5f) {
        barColor = (RaylibColor){0, 255, 0, 255};  // Vert
    } else if(progress > 0.25f) {
        barColor = (RaylibColor){255, 255, 0, 255};  // Jaune
    } else {
        barColor = (RaylibColor){255, 0, 0, 255};  // Rouge
    }
    
    DrawRectangle(barX, barY, (int)(barWidth * progress), barHeight, barColor);
}

int isTimerExpired(GameTimer* timer) {
    return timer->hasExpired;
}

int isTimerWarning(GameTimer* timer) {
    return timer->timeRemaining <= timer->warningTime && !timer->hasExpired;
}




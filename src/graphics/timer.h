// src/graphics/timer.h
#ifndef TIMER_H
#define TIMER_H

// Chronomètre pour minute par tour
typedef struct {
    float timeRemaining;  // Temps restant en secondes
    float maxTime;        // Temps maximum par tour (60 secondes par défaut)
    int isActive;         // 1 si le chronomètre est actif
    int isPaused;         // 1 si le chronomètre est en pause
    int hasExpired;       // 1 si le temps est écoulé
    float warningTime;     // Temps d'avertissement (10 secondes)
} GameTimer;

// Fonctions
void initTimer(GameTimer* timer, float maxTime);
void startTimer(GameTimer* timer);
void pauseTimer(GameTimer* timer);
void resumeTimer(GameTimer* timer);
void resetTimer(GameTimer* timer);
void updateTimer(GameTimer* timer, float deltaTime);
void drawTimer(GameTimer* timer, int x, int y, int fontSize);
int isTimerExpired(GameTimer* timer);
int isTimerWarning(GameTimer* timer);

#endif




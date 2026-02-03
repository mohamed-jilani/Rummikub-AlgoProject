// src/graphics/animations.h
#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include "tile_wrapper.h"
#include "../core/tile.h"

#define MAX_ANIMATIONS 20

// Type d'animation
typedef enum {
    ANIM_NONE,
    ANIM_SLIDE,      // Glissement
    ANIM_FADE,       // Fondu
    ANIM_PULSE,      // Pulsation
    ANIM_HIGHLIGHT   // Mise en évidence
} AnimationType;

// État d'une animation
typedef struct {
    AnimationType type;
    float startTime;
    float duration;
    int active;
    
    // Position de départ et d'arrivée (pour slide)
    float startX, startY;
    float endX, endY;
    float currentX, currentY;
    
    // Opacité (pour fade)
    float opacity;
    float startOpacity;
    float endOpacity;
    
    // Échelle (pour pulse)
    float scale;
    
    // Données de la tuile
    Tile tile;
    int tileIndex;  // Index dans le chevalet ou sur le plateau
} TileAnimation;

// Gestionnaire d'animations
typedef struct {
    TileAnimation animations[MAX_ANIMATIONS];
    int animationCount;
    float currentTime;
} AnimationManager;

// Fonctions
void initAnimationManager(AnimationManager* manager);
void updateAnimations(AnimationManager* manager, float deltaTime);
void addSlideAnimation(AnimationManager* manager, Tile tile, int tileIndex,
                      float startX, float startY, float endX, float endY, float duration);
void addFadeAnimation(AnimationManager* manager, Tile tile, int tileIndex,
                     float duration);
void addPulseAnimation(AnimationManager* manager, Tile tile, int tileIndex,
                      float duration);
void addHighlightAnimation(AnimationManager* manager, Tile tile, int tileIndex,
                          float duration);
void drawAnimatedTile(AnimationManager* manager, Tile tile, int tileIndex,
                     int defaultX, int defaultY);
void cleanupAnimations(AnimationManager* manager);

#endif




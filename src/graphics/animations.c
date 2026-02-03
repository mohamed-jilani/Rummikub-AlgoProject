// src/graphics/animations.c
#include "animations.h"
#include "raylib_renderer.h"
#include <math.h>
#include <string.h>

void initAnimationManager(AnimationManager* manager) {
    manager->animationCount = 0;
    manager->currentTime = 0.0f;
    memset(manager->animations, 0, sizeof(manager->animations));
}

void updateAnimations(AnimationManager* manager, float deltaTime) {
    manager->currentTime += deltaTime;
    
    for(int i = 0; i < manager->animationCount; i++) {
        TileAnimation* anim = &manager->animations[i];
        
        if(!anim->active) continue;
        
        float elapsed = manager->currentTime - anim->startTime;
        float progress = elapsed / anim->duration;
        
        if(progress >= 1.0f) {
            // Animation terminée
            anim->active = 0;
            progress = 1.0f;
        }
        
        // Fonction d'easing (ease-out)
        float eased = 1.0f - powf(1.0f - progress, 3.0f);
        
        switch(anim->type) {
            case ANIM_SLIDE:
                anim->currentX = anim->startX + (anim->endX - anim->startX) * eased;
                anim->currentY = anim->startY + (anim->endY - anim->startY) * eased;
                break;
                
            case ANIM_FADE:
                anim->opacity = anim->startOpacity + (anim->endOpacity - anim->startOpacity) * progress;
                break;
                
            case ANIM_PULSE:
                anim->scale = 1.0f + 0.2f * sinf(progress * 3.14159f * 2.0f);
                break;
                
            case ANIM_HIGHLIGHT:
                anim->opacity = 0.5f + 0.5f * sinf(progress * 3.14159f * 4.0f);
                break;
                
            default:
                break;
        }
    }
    
    // Nettoyer les animations terminées
    int writeIndex = 0;
    for(int i = 0; i < manager->animationCount; i++) {
        if(manager->animations[i].active) {
            if(writeIndex != i) {
                manager->animations[writeIndex] = manager->animations[i];
            }
            writeIndex++;
        }
    }
    manager->animationCount = writeIndex;
}

void addSlideAnimation(AnimationManager* manager, Tile tile, int tileIndex,
                      float startX, float startY, float endX, float endY, float duration) {
    if(manager->animationCount >= MAX_ANIMATIONS) return;
    
    TileAnimation* anim = &manager->animations[manager->animationCount++];
    anim->type = ANIM_SLIDE;
    anim->startTime = manager->currentTime;
    anim->duration = duration;
    anim->active = 1;
    anim->startX = startX;
    anim->startY = startY;
    anim->endX = endX;
    anim->endY = endY;
    anim->currentX = startX;
    anim->currentY = startY;
    anim->tile = tile;
    anim->tileIndex = tileIndex;
    anim->opacity = 1.0f;
    anim->scale = 1.0f;
}

void addFadeAnimation(AnimationManager* manager, Tile tile, int tileIndex,
                   float duration) {
    if(manager->animationCount >= MAX_ANIMATIONS) return;
    
    TileAnimation* anim = &manager->animations[manager->animationCount++];
    anim->type = ANIM_FADE;
    anim->startTime = manager->currentTime;
    anim->duration = duration;
    anim->active = 1;
    anim->startOpacity = 1.0f;
    anim->endOpacity = 0.0f;
    anim->tile = tile;
    anim->tileIndex = tileIndex;
    anim->opacity = 1.0f;
    anim->scale = 1.0f;
}

void addPulseAnimation(AnimationManager* manager, Tile tile, int tileIndex,
                      float duration) {
    if(manager->animationCount >= MAX_ANIMATIONS) return;
    
    TileAnimation* anim = &manager->animations[manager->animationCount++];
    anim->type = ANIM_PULSE;
    anim->startTime = manager->currentTime;
    anim->duration = duration;
    anim->active = 1;
    anim->tile = tile;
    anim->tileIndex = tileIndex;
    anim->opacity = 1.0f;
    anim->scale = 1.0f;
}

void addHighlightAnimation(AnimationManager* manager, Tile tile, int tileIndex,
                          float duration) {
    if(manager->animationCount >= MAX_ANIMATIONS) return;
    
    TileAnimation* anim = &manager->animations[manager->animationCount++];
    anim->type = ANIM_HIGHLIGHT;
    anim->startTime = manager->currentTime;
    anim->duration = duration;
    anim->active = 1;
    anim->tile = tile;
    anim->tileIndex = tileIndex;
    anim->opacity = 1.0f;
    anim->scale = 1.0f;
}

void drawAnimatedTile(AnimationManager* manager, Tile tile, int tileIndex,
                     int defaultX, int defaultY) {
    // Chercher si cette tuile a une animation active
    TileAnimation* anim = NULL;
    for(int i = 0; i < manager->animationCount; i++) {
        if(manager->animations[i].active &&
           manager->animations[i].tileIndex == tileIndex) {
            anim = &manager->animations[i];
            break;
        }
    }
    
    int x = defaultX;
    int y = defaultY;
    float opacity = 1.0f;
    float scale = 1.0f;
    
    if(anim) {
        switch(anim->type) {
            case ANIM_SLIDE:
                x = (int)anim->currentX;
                y = (int)anim->currentY;
                opacity = anim->opacity;
                break;
            case ANIM_FADE:
                opacity = anim->opacity;
                break;
            case ANIM_PULSE:
                scale = anim->scale;
                break;
            case ANIM_HIGHLIGHT:
                opacity = anim->opacity;
                break;
            default:
                break;
        }
    }
    
    // Dessiner la tuile avec les paramètres d'animation
    RaylibColor fillColor = getTileColor(tile);
    fillColor.a = (unsigned char)(opacity * 255);
    
    // Appliquer l'échelle si nécessaire
    int width = (int)(TILE_WIDTH * scale);
    int height = (int)(TILE_HEIGHT * scale);
    int offsetX = (TILE_WIDTH - width) / 2;
    int offsetY = (TILE_HEIGHT - height) / 2;
    
    DrawRectangleRounded((Rectangle){x + offsetX, y + offsetY, width, height},
                        0.2f, 10, fillColor);
    
    // Bordure
    DrawRectangleRoundedLines((Rectangle){x + offsetX, y + offsetY, width, height},
                             0.2f, 10, (RaylibColor){0, 0, 0, (unsigned char)(opacity * 255)});
    
    // Texte
    RaylibColor textColor = getTileTextColor(tile);
    textColor.a = (unsigned char)(opacity * 255);
    
    if(tile.isJoker) {
        int textWidth = MeasureText("JOKER", 14);
        DrawText("JOKER", x + (TILE_WIDTH - textWidth) / 2, y + 30, 14, textColor);
    } else {
        char valueText[4];
        sprintf(valueText, "%d", tile.value);
        int textWidth = MeasureText(valueText, 24);
        DrawText(valueText, x + (TILE_WIDTH - textWidth) / 2, y + 28, 24, textColor);
    }
}

void cleanupAnimations(AnimationManager* manager) {
    manager->animationCount = 0;
    memset(manager->animations, 0, sizeof(manager->animations));
}




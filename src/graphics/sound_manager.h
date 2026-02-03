// src/graphics/sound_manager.h
#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include "tile_wrapper.h"  // Inclut raylib.h avec gestion des conflits

// Types de sons
typedef enum {
    SOUND_CLICK,
    SOUND_VALIDATE,
    SOUND_ERROR,
    SOUND_TILE_PLACE,
    SOUND_TURN_END,
    SOUND_WIN
} SoundType;

// Gestionnaire de sons
typedef struct {
    Sound clickSound;
    Sound validateSound;
    Sound errorSound;
    Sound tilePlaceSound;
    Sound turnEndSound;
    Sound winSound;
    int soundsLoaded;
    float masterVolume;
} SoundManager;

// Fonctions
int initSoundManager(SoundManager* manager);
void playSound(SoundManager* manager, SoundType type);
void setMasterVolume(SoundManager* manager, float volume);
void cleanupSoundManager(SoundManager* manager);

#endif


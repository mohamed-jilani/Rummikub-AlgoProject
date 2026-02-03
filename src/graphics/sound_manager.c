// src/graphics/sound_manager.c
#include "sound_manager.h"
#include "tile_wrapper.h"
#include <math.h>
#include <stdlib.h>

// Note: Les sons sont désactivés si l'audio n'est pas disponible
// On pourrait utiliser des fichiers WAV à la place pour une meilleure qualité

int initSoundManager(SoundManager* manager) {
    manager->soundsLoaded = 0;
    manager->masterVolume = 0.5f;
    
    // Initialiser l'audio
    InitAudioDevice();
    if(!IsAudioDeviceReady()) {
        return 0;  // Audio non disponible, continuer sans sons
    }
    
    // Pour l'instant, on désactive les sons car la génération de Wave
    // nécessite une API plus complexe. On peut ajouter des fichiers WAV plus tard.
    // Les sons seront joués via PlaySound() mais seront silencieux.
    manager->soundsLoaded = 0;  // Désactivé pour l'instant
    return 0;  // Retourner 0 mais continuer sans sons
}

void playSound(SoundManager* manager, SoundType type) {
    // Pour l'instant, les sons sont désactivés
    // On peut ajouter des fichiers WAV plus tard
    (void)manager;  // Éviter l'avertissement unused
    (void)type;
    
    // TODO: Implémenter avec des fichiers WAV ou une API de génération de sons
}

void setMasterVolume(SoundManager* manager, float volume) {
    if(volume < 0.0f) volume = 0.0f;
    if(volume > 1.0f) volume = 1.0f;
    manager->masterVolume = volume;
}

void cleanupSoundManager(SoundManager* manager) {
    if(manager->soundsLoaded) {
        // Nettoyer les sons si chargés
        // Pour l'instant, rien à nettoyer car les sons sont désactivés
        if(IsAudioDeviceReady()) {
            CloseAudioDevice();
        }
        manager->soundsLoaded = 0;
    }
}


// src/graphics/ui_components.h
#ifndef UI_COMPONENTS_H
#define UI_COMPONENTS_H

#include "tile_wrapper.h"

// Structure pour un champ de texte
typedef struct {
    char text[256];
    int textLength;
    int maxLength;
    int cursorPos;
    int isActive;
    Rectangle bounds;
    int fontSize;
} TextInput;

// Structure pour un bouton
typedef struct {
    Rectangle bounds;
    char label[64];
    int isHovered;
    int isClicked;
    RaylibColor normalColor;
    RaylibColor hoverColor;
    RaylibColor clickColor;
} Button;

// Structure pour les messages
#define MAX_MESSAGES 10
#define MESSAGE_LENGTH 200

typedef struct {
    char messages[MAX_MESSAGES][MESSAGE_LENGTH];
    int messageCount;
    float messageTimes[MAX_MESSAGES];  // Temps d'affichage de chaque message
} MessageLog;

// Fonctions pour les champs de texte
TextInput createTextInput(int x, int y, int width, int height, int maxLength, int fontSize);
void updateTextInput(TextInput* input);
void drawTextInput(TextInput* input);
void setTextInputActive(TextInput* input, int active);

// Fonctions pour les boutons
Button createUIButton(int x, int y, int width, int height, const char* label);
void updateUIButton(Button* button);
void drawUIButton(Button* button);
int isUIButtonClicked(Button* button);

// Fonctions pour les messages
void initMessageLog(MessageLog* log);
void addMessage(MessageLog* log, const char* message);
void updateMessageLog(MessageLog* log, float deltaTime);
void drawMessageLog(MessageLog* log, int x, int y, int width, int height);

#endif


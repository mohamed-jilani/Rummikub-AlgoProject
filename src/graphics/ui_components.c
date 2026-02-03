// src/graphics/ui_components.c
#include "ui_components.h"
#include <string.h>
#include <stdio.h>

// ============================================
// Champs de texte
// ============================================

TextInput createTextInput(int x, int y, int width, int height, int maxLength, int fontSize) {
    TextInput input;
    input.text[0] = '\0';
    input.textLength = 0;
    input.maxLength = maxLength < 256 ? maxLength : 255;
    input.cursorPos = 0;
    input.isActive = 0;
    input.bounds = (Rectangle){x, y, width, height};
    input.fontSize = fontSize;
    return input;
}

void updateTextInput(TextInput* input) {
    if (!input->isActive) return;
    
    // Gérer la saisie de texte
    int key = GetCharPressed();
    while (key > 0) {
        if (key >= 32 && key <= 126 && input->textLength < input->maxLength) {
            // Insérer le caractère à la position du curseur
            memmove(&input->text[input->cursorPos + 1], 
                   &input->text[input->cursorPos],
                   input->textLength - input->cursorPos + 1);
            input->text[input->cursorPos] = (char)key;
            input->cursorPos++;
            input->textLength++;
            input->text[input->textLength] = '\0';
        }
        key = GetCharPressed();
    }
    
    // Gérer la suppression
    if (IsKeyPressed(KEY_BACKSPACE) && input->cursorPos > 0) {
        memmove(&input->text[input->cursorPos - 1],
               &input->text[input->cursorPos],
               input->textLength - input->cursorPos + 1);
        input->cursorPos--;
        input->textLength--;
        input->text[input->textLength] = '\0';
    }
    
    // Gérer la suppression (Delete)
    if (IsKeyPressed(KEY_DELETE) && input->cursorPos < input->textLength) {
        memmove(&input->text[input->cursorPos],
               &input->text[input->cursorPos + 1],
               input->textLength - input->cursorPos);
        input->textLength--;
        input->text[input->textLength] = '\0';
    }
    
    // Navigation du curseur
    if (IsKeyPressed(KEY_LEFT) && input->cursorPos > 0) {
        input->cursorPos--;
    }
    if (IsKeyPressed(KEY_RIGHT) && input->cursorPos < input->textLength) {
        input->cursorPos++;
    }
    
    // Gérer le clic pour activer/désactiver
    Vector2 mousePos = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mousePos, input->bounds)) {
            input->isActive = 1;
        } else {
            input->isActive = 0;
        }
    }
}

void drawTextInput(TextInput* input) {
    RaylibColor bgColor = input->isActive ? 
                         (RaylibColor){60, 60, 80, 255} : 
                         (RaylibColor){40, 40, 50, 255};
    RaylibColor borderColor = input->isActive ? 
                             (RaylibColor){100, 150, 255, 255} : 
                             (RaylibColor){80, 80, 100, 255};
    
    // Fond
    DrawRectangleRounded(input->bounds, 0.1f, 10, bgColor);
    DrawRectangleRoundedLines(input->bounds, 0.1f, 10, borderColor);
    
    // Texte
    if (input->textLength > 0) {
        DrawText(input->text, input->bounds.x + 10, 
                input->bounds.y + (input->bounds.height - input->fontSize) / 2,
                input->fontSize, (RaylibColor){255, 255, 255, 255});
    } else if (!input->isActive) {
        // Texte placeholder
        DrawText("Entrez un nom...", input->bounds.x + 10,
                input->bounds.y + (input->bounds.height - input->fontSize) / 2,
                input->fontSize, (RaylibColor){150, 150, 150, 255});
    }
    
    // Curseur (clignotant)
    if (input->isActive) {
        static float cursorTime = 0.0f;
        cursorTime += GetFrameTime();
        if ((int)(cursorTime * 2) % 2 == 0) {
            int textWidth = MeasureText(input->text, input->fontSize);
            int cursorX = input->bounds.x + 10 + textWidth;
            DrawLine(cursorX, input->bounds.y + 5,
                    cursorX, input->bounds.y + input->bounds.height - 5,
                    (RaylibColor){255, 255, 255, 255});
        }
    }
}

void setTextInputActive(TextInput* input, int active) {
    input->isActive = active;
}

// ============================================
// Boutons
// ============================================

Button createUIButton(int x, int y, int width, int height, const char* label) {
    Button btn;
    btn.bounds = (Rectangle){x, y, width, height};
    strncpy(btn.label, label, sizeof(btn.label) - 1);
    btn.label[sizeof(btn.label) - 1] = '\0';
    btn.isHovered = 0;
    btn.isClicked = 0;
    btn.normalColor = (RaylibColor){60, 100, 180, 255};
    btn.hoverColor = (RaylibColor){80, 120, 220, 255};
    btn.clickColor = (RaylibColor){40, 80, 160, 255};
    return btn;
}

void updateUIButton(Button* button) {
    Vector2 mousePos = GetMousePosition();
    button->isHovered = CheckCollisionPointRec(mousePos, button->bounds);
    
    if (button->isHovered && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        button->isClicked = 1;
    } else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        button->isClicked = 0;
    }
}

void drawUIButton(Button* button) {
    RaylibColor btnColor = button->normalColor;
    
    if (button->isClicked) {
        btnColor = button->clickColor;
    } else if (button->isHovered) {
        btnColor = button->hoverColor;
    }
    
    // Ombre si survolé
    if (button->isHovered || button->isClicked) {
        DrawRectangleRounded((Rectangle){button->bounds.x + 3, button->bounds.y + 3,
                                        button->bounds.width, button->bounds.height},
                           0.3f, 10, (RaylibColor){0, 0, 0, 100});
    }
    
    // Rectangle principal
    DrawRectangleRounded(button->bounds, 0.3f, 10, btnColor);
    DrawRectangleRoundedLines(button->bounds, 0.3f, 10, (RaylibColor){255, 255, 255, 255});
    
    // Texte centré
    int fontSize = 20;
    int textWidth = MeasureText(button->label, fontSize);
    int textX = button->bounds.x + (button->bounds.width - textWidth) / 2;
    int textY = button->bounds.y + (button->bounds.height - fontSize) / 2;
    
    DrawText(button->label, textX, textY, fontSize, (RaylibColor){255, 255, 255, 255});
}

int isUIButtonClicked(Button* button) {
    return button->isHovered && IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
}

// ============================================
// Messages
// ============================================

void initMessageLog(MessageLog* log) {
    log->messageCount = 0;
    for (int i = 0; i < MAX_MESSAGES; i++) {
        log->messages[i][0] = '\0';
        log->messageTimes[i] = 0.0f;
    }
}

void addMessage(MessageLog* log, const char* message) {
    // Décaler les messages existants
    for (int i = MAX_MESSAGES - 1; i > 0; i--) {
        strcpy(log->messages[i], log->messages[i - 1]);
        log->messageTimes[i] = log->messageTimes[i - 1];
    }
    
    // Ajouter le nouveau message
    strncpy(log->messages[0], message, MESSAGE_LENGTH - 1);
    log->messages[0][MESSAGE_LENGTH - 1] = '\0';
    log->messageTimes[0] = 5.0f;  // Afficher pendant 5 secondes
    
    if (log->messageCount < MAX_MESSAGES) {
        log->messageCount++;
    }
}

void updateMessageLog(MessageLog* log, float deltaTime) {
    for (int i = 0; i < log->messageCount; i++) {
        log->messageTimes[i] -= deltaTime;
        if (log->messageTimes[i] <= 0.0f) {
            // Supprimer le message expiré
            for (int j = i; j < log->messageCount - 1; j++) {
                strcpy(log->messages[j], log->messages[j + 1]);
                log->messageTimes[j] = log->messageTimes[j + 1];
            }
            log->messageCount--;
            i--;  // Re-vérifier cette position
        }
    }
}

void drawMessageLog(MessageLog* log, int x, int y, int width, int height) {
    // Fond
    DrawRectangleRounded((Rectangle){x, y, width, height}, 0.1f, 10,
                        (RaylibColor){20, 20, 30, 255});
    DrawRectangleRoundedLines((Rectangle){x, y, width, height}, 0.1f, 10,
                            (RaylibColor){100, 100, 150, 255});
    
    // Titre
    DrawText("Messages", x + 10, y + 10, 18, (RaylibColor){220, 180, 60, 255});
    
    // Messages
    int fontSize = 14;
    int lineHeight = fontSize + 5;
    int startY = y + 35;
    
    for (int i = 0; i < log->messageCount && i < (height - 40) / lineHeight; i++) {
        float alpha = log->messageTimes[i] > 1.0f ? 255.0f : log->messageTimes[i] * 255.0f;
        RaylibColor textColor = (RaylibColor){200, 200, 200, (unsigned char)alpha};
        DrawText(log->messages[i], x + 10, startY + i * lineHeight, fontSize, textColor);
    }
}


#include <stdio.h>
#include <string.h>
#include <math.h>
#include "raylib.h"
#include "../utils/helpers.h"

// Structure pour une tuile simplifiée
typedef struct {
    int value;
    int color;  // 0=ROUGE, 1=BLEU, 2=NOIR, 3=JAUNE, 4=JOKER
    int isJoker;
} SimpleTile;

// État de l'interface
typedef struct {
    int screenWidth;
    int screenHeight;
    int selectedTile;
    int hoveredTile;
    int showInstructions;
    float animationTime;
} UIState;

// Fonction pour créer une tuile simple
SimpleTile createSimpleTile(int value, int color, int isJoker) {
    SimpleTile tile;
    tile.value = value;
    tile.color = color;
    tile.isJoker = isJoker;
    return tile;
}

// Couleurs personnalisées pour un meilleur contraste
Color getSimpleColor(SimpleTile tile) {
    if (tile.isJoker) return (Color){120, 120, 120, 255};
    
    switch(tile.color) {
        case 0: return (Color){180, 50, 50, 255};    // ROUGE
        case 1: return (Color){50, 100, 200, 255};   // BLEU
        case 2: return (Color){40, 40, 40, 255};     // NOIR
        case 3: return (Color){200, 180, 50, 255};   // JAUNE
        default: return WHITE;
    }
}

// Déterminer la couleur du texte
Color getTextColor(SimpleTile tile) {
    if (tile.isJoker) return WHITE;
    
    switch(tile.color) {
        case 0:  // ROUGE
        case 1:  // BLEU  
        case 2:  // NOIR
            return WHITE;
        case 3:  // JAUNE
            return BLACK;
        default: return BLACK;
    }
}

// Fonction pour dessiner un rectangle avec bordure (compatible)
void drawRoundedRectWithBorder(int x, int y, int width, int height, float roundness, 
                               Color fillColor, Color borderColor, int borderWidth) {
    // Dessiner le rectangle rempli
    DrawRectangleRounded((Rectangle){x, y, width, height}, roundness, 10, fillColor);
    
    // Dessiner la bordure en plusieurs rectangles plus petits
    for (int i = 0; i < borderWidth; i++) {
        DrawRectangleRoundedLines((Rectangle){x - i, y - i, width + 2*i, height + 2*i}, 
                                 roundness, 10, borderColor);
    }
}

// Dessiner une tuile avec style (version compatible)
void drawSimpleTile(SimpleTile tile, int x, int y, int selected, int hovered) {
    Color borderColor = selected ? RED : (hovered ? YELLOW : BLACK);
    Color fillColor = getSimpleColor(tile);
    
    // Ombre pour l'effet de profondeur
    if (selected) {
        DrawRectangle(x + 3, y + 3, 60, 80, (Color){0, 0, 0, 100});
    }
    
    // La tuile avec coins arrondis
    DrawRectangleRounded((Rectangle){x, y, 60, 80}, 0.2, 10, fillColor);
    
    // Bordure
    int borderThickness = (selected || hovered) ? (selected ? 3 : 2) : 1;
    for (int i = 0; i < borderThickness; i++) {
        DrawRectangleRoundedLines((Rectangle){x - i, y - i, 60 + 2*i, 80 + 2*i}, 0.2, 10, borderColor);
    }
    
    // Effet de relief léger
    DrawRectangleRoundedLines((Rectangle){x+1, y+1, 58, 78}, 0.2, 10, 
                             (Color){255, 255, 255, 50});
    
    // Texte de la valeur
    Color textColor = getTextColor(tile);
    
    if (tile.isJoker) {
        DrawText("JOKER", x + 8, y + 28, 13, textColor);
    } else {
        char text[10];
        sprintf(text, "%d", tile.value);
        
        // Ajuster la position pour les nombres à 2 chiffres
        int textX = (tile.value >= 10) ? x + 15 : x + 20;
        DrawText(text, textX, y + 32, 22, textColor);
    }
    
    // Petit indicateur de couleur dans le coin
    DrawRectangle(x + 45, y + 5, 10, 10, borderColor);
}

// Dessiner le fond avec dégradé (version simplifiée)
void drawBackground(UIState* state) {
    // Dégradé simple
    Color topColor = (Color){30, 30, 60, 255};
    Color bottomColor = (Color){10, 10, 30, 255};
    
    // Dessiner deux rectangles pour le dégradé
    DrawRectangleGradientV(0, 0, state->screenWidth, state->screenHeight/2, topColor, bottomColor);
    DrawRectangleGradientV(0, state->screenHeight/2, state->screenWidth, state->screenHeight/2, bottomColor, topColor);
    
    // Points décoratifs (étoiles)
    for (int i = 0; i < 20; i++) {
        float time = state->animationTime + i * 0.5f;
        int x = (int)(sin(time) * 50 + i * 100);
        int y = (int)(cos(time) * 30 + 100);
        DrawCircle(x, y, 2, (Color){255, 255, 255, 30});
    }
}

// Dessiner l'en-tête de l'application
void drawHeader(UIState* state) {
    // Barre d'en-tête
    DrawRectangle(0, 0, state->screenWidth, 80, (Color){20, 20, 40, 255});
    DrawRectangle(0, 80, state->screenWidth, 2, (Color){100, 100, 200, 255});
    
    // Logo/titre
    DrawText("RUMMIKUB", 50, 20, 50, (Color){220, 180, 60, 255});
    
    // Sous-titre
    DrawText("Projet Algorithmique - ISTY IATIC3", 350, 35, 18, (Color){180, 180, 220, 255});
    
    // Ligne décorative animée
    float pulse = sin(state->animationTime * 2) * 0.2f + 0.8f;
    int lineWidth = 250 + (int)(sin(state->animationTime) * 10);
    DrawRectangle(45, 70, lineWidth, 3, (Color){220, 180, 60, (unsigned char)(200 * pulse)});
}

// Dessiner le panneau de contrôle latéral
void drawControlPanel(UIState* state, SimpleTile* tiles, int tileCount) {
    int panelX = state->screenWidth - 300;
    int panelY = 100;
    
    // Fond du panneau
    DrawRectangleRounded((Rectangle){panelX, panelY, 280, 500}, 0.1, 10, 
                        (Color){30, 30, 50, 255});
    // Bordure épaisse (2 pixels)
    for (int i = 0; i < 2; i++) {
        DrawRectangleRoundedLines((Rectangle){panelX - i, panelY - i, 280 + 2*i, 500 + 2*i}, 0.1, 10, 
                                (Color){100, 100, 200, 255});
    }
    
    // Titre du panneau
    DrawText("CONTROLES", panelX + 20, panelY + 20, 24, (Color){220, 180, 60, 255});
    
    // Informations sur la tuile sélectionnée
    if (state->selectedTile >= 0 && state->selectedTile < tileCount) {
        SimpleTile tile = tiles[state->selectedTile];
        DrawText(TextFormat("Tuile selectionnee: %d/%d", 
                           state->selectedTile + 1, tileCount), 
                panelX + 20, panelY + 60, 18, WHITE);
        
        char tileInfo[50];
        if (tile.isJoker) {
            sprintf(tileInfo, "JOKER (valeur: 30)");
        } else {
            const char* colorNames[] = {"ROUGE", "BLEU", "NOIR", "JAUNE"};
            sprintf(tileInfo, "Valeur: %d | Couleur: %s", 
                   tile.value, colorNames[tile.color]);
        }
        DrawText(tileInfo, panelX + 20, panelY + 90, 16, LIGHTGRAY);
    }
    
    // Instructions de contrôle
    int y = panelY + 130;
    const char* instructions[] = {
        "FLECHES: Selectionner une tuile",
        "CLIC GAUCHE: Selectionner",
        "ESPACE: Simuler un tour",
        "I: Afficher/cacher info",
        "R: Reinitialiser",
        "ECHAP: Quitter",
        "",
        "Regles du jeu:",
        "• Serie: Meme valeur,",
        "  couleurs differentes",
        "• Suite: Meme couleur,",
        "  valeurs consecutives",
        "• Minimum: 3 tuiles"
    };
    
    for (int i = 0; i < 13; i++) {
        Color textColor = (i >= 7) ? (Color){180, 220, 180, 255} : LIGHTGRAY;
        DrawText(instructions[i], panelX + 20, y, 16, textColor);
        y += 25;
    }
    
    // Bouton de simulation de tour
    Rectangle simButton = {panelX + 50, panelY + 400, 180, 40};
    Color buttonColor;
    
    if (CheckCollisionPointRec(GetMousePosition(), simButton)) {
        buttonColor = IsMouseButtonDown(MOUSE_LEFT_BUTTON) ? 
                     (Color){60, 100, 160, 255} : 
                     (Color){80, 120, 200, 255};
    } else {
        buttonColor = (Color){60, 100, 180, 255};
    }
    
    DrawRectangleRounded(simButton, 0.3, 10, buttonColor);
    // Bordure épaisse (2 pixels)
    for (int i = 0; i < 2; i++) {
        Rectangle borderRect = {simButton.x - i, simButton.y - i, simButton.width + 2*i, simButton.height + 2*i};
        DrawRectangleRoundedLines(borderRect, 0.3, 10, WHITE);
    }
    DrawText("SIMULER UN TOUR", panelX + 65, panelY + 412, 16, WHITE);
    
    // État simulé du jeu
    DrawText("Etat simule:", panelX + 20, panelY + 450, 18, (Color){220, 180, 60, 255});
    DrawText("• Tour: 1", panelX + 40, panelY + 480, 16, LIGHTGRAY);
    DrawText("• Joueur: Humain", panelX + 40, panelY + 505, 16, LIGHTGRAY);
    DrawText("• Score: 45 points", panelX + 40, panelY + 530, 16, LIGHTGRAY);
}

// Fonction principale de la démo graphique améliorée
void runImprovedGraphicalDemo() {
    // Initialiser l'état de l'interface
    UIState state = {
        .screenWidth = 1400,
        .screenHeight = 800,
        .selectedTile = 0,
        .hoveredTile = -1,
        .showInstructions = 1,
        .animationTime = 0
    };
    
    // Créer la fenêtre
    InitWindow(state.screenWidth, state.screenHeight, "Rummikub - Interface Professionnelle");
    SetTargetFPS(60);
    
    // Créer un jeu de tuiles pour la démonstration
    SimpleTile tiles[13];
    int tileIndex = 0;
    
    // Série de 7 (exemple de série valide)
    tiles[tileIndex++] = createSimpleTile(7, 0, 0);  // 7 ROUGE
    tiles[tileIndex++] = createSimpleTile(7, 1, 0);  // 7 BLEU
    tiles[tileIndex++] = createSimpleTile(7, 3, 0);  // 7 JAUNE
    
    // Suite rouge 1-2-3 (exemple de suite valide)
    tiles[tileIndex++] = createSimpleTile(1, 0, 0);
    tiles[tileIndex++] = createSimpleTile(2, 0, 0);
    tiles[tileIndex++] = createSimpleTile(3, 0, 0);
    
    // Suite noire 10-11-12
    tiles[tileIndex++] = createSimpleTile(10, 2, 0);
    tiles[tileIndex++] = createSimpleTile(11, 2, 0);
    tiles[tileIndex++] = createSimpleTile(12, 2, 0);
    
    // Tuiles diverses pour compléter l'affichage
    tiles[tileIndex++] = createSimpleTile(5, 1, 0);  // 5 BLEU
    tiles[tileIndex++] = createSimpleTile(13, 3, 0); // 13 JAUNE
    tiles[tileIndex++] = createSimpleTile(4, 0, 0);  // 4 ROUGE
    tiles[tileIndex++] = createSimpleTile(0, 4, 1);  // JOKER
    
    int tileCount = tileIndex;
    
    // Boucle principale
    while (!WindowShouldClose()) {
        // Mettre à jour le temps pour les animations
        state.animationTime += GetFrameTime();
        
        // Récupérer la position de la souris
        Vector2 mousePos = GetMousePosition();
        
        // Détecter quelle tuile est survolée
        state.hoveredTile = -1;
        for (int i = 0; i < tileCount; i++) {
            int x = 100 + (i % 6) * 110;
            int y = 150 + (i / 6) * 120;
            
            if (CheckCollisionPointRec(mousePos, (Rectangle){x, y, 60, 80})) {
                state.hoveredTile = i;
                break;
            }
        }
        
        // Gestion des touches du clavier
        if (IsKeyPressed(KEY_RIGHT)) state.selectedTile = (state.selectedTile + 1) % tileCount;
        if (IsKeyPressed(KEY_LEFT)) state.selectedTile = (state.selectedTile - 1 + tileCount) % tileCount;
        if (IsKeyPressed(KEY_UP)) state.selectedTile = (state.selectedTile - 6 + tileCount) % tileCount;
        if (IsKeyPressed(KEY_DOWN)) state.selectedTile = (state.selectedTile + 6) % tileCount;
        if (IsKeyPressed(KEY_R)) state.selectedTile = 0;
        if (IsKeyPressed(KEY_I)) state.showInstructions = !state.showInstructions;
        
        // Gestion du clic de souris
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (state.hoveredTile != -1) {
                state.selectedTile = state.hoveredTile;
            }
        }
        
        // ===== DÉBUT DU DESSIN =====
        BeginDrawing();
        
        // 1. Fond avec dégradé
        drawBackground(&state);
        
        // 2. En-tête
        drawHeader(&state);
        
        // 3. Titre de la section principale
        DrawText("TUILES DE DEMONSTRATION", 100, 100, 28, (Color){220, 220, 255, 255});
        
        // 4. Afficher toutes les tuiles
        for (int i = 0; i < tileCount; i++) {
            int x = 100 + (i % 6) * 110;
            int y = 150 + (i / 6) * 120;
            
            int selected = (state.selectedTile == i);
            int hovered = (state.hoveredTile == i);
            
            drawSimpleTile(tiles[i], x, y, selected, hovered);
            
            // Numéro de la tuile en petit
            DrawText(TextFormat("%d", i+1), x + 5, y + 5, 12, 
                    selected ? YELLOW : (Color){200, 200, 200, 150});
        }
        
        // 5. Panneau de contrôle latéral
        drawControlPanel(&state, tiles, tileCount);
        
        // 6. Légende des couleurs
        DrawText("LEGENDE DES COULEURS:", 100, 550, 22, (Color){220, 180, 60, 255});
        
        const char* colorNames[] = {"ROUGE", "BLEU", "NOIR", "JAUNE", "JOKER"};
        Color colorDisplay[] = {
            (Color){180, 50, 50, 255},    // Rouge
            (Color){50, 100, 200, 255},   // Bleu
            (Color){40, 40, 40, 255},     // Noir
            (Color){200, 180, 50, 255},   // Jaune
            (Color){120, 120, 120, 255}   // Joker
        };
        
        for (int i = 0; i < 5; i++) {
            int x = 100 + i * 150;
            DrawRectangleRounded((Rectangle){x, 590, 30, 30}, 0.2, 10, colorDisplay[i]);
            // Bordure épaisse (2 pixels)
            for (int j = 0; j < 2; j++) {
                DrawRectangleRoundedLines((Rectangle){x - j, 590 - j, 30 + 2*j, 30 + 2*j}, 0.2, 10, WHITE);
            }
            DrawText(colorNames[i], x + 40, 595, 16, LIGHTGRAY);
        }
        
        // 7. Exemples de combinaisons valides
        DrawText("EXEMPLES DE COMBINAISONS VALIDES:", 100, 640, 22, (Color){180, 220, 180, 255});
        DrawText("Serie: 7-7-7 (meme valeur, couleurs differentes)", 120, 670, 16, LIGHTGRAY);
        DrawText("Suite: 1-2-3 (meme couleur, valeurs consecutives)", 120, 695, 16, LIGHTGRAY);
        DrawText("Joker: remplace n'importe quelle tuile", 120, 720, 16, LIGHTGRAY);
        
        // 8. Pied de page
        DrawRectangle(0, state.screenHeight - 40, state.screenWidth, 40, 
                     (Color){20, 20, 40, 255});
        DrawText("Developpe avec Raylib | Projet Algorithmique ISTY 2025-2026 | Equipe Rummikub", 
                 200, state.screenHeight - 30, 16, (Color){150, 150, 200, 255});
        
        // 9. Indicateur FPS
        DrawText(TextFormat("FPS: %d", GetFPS()), state.screenWidth - 100, 10, 18, GREEN);
        
        // ===== FIN DU DESSIN =====
        EndDrawing();
    }
    
    // Fermer la fenêtre
    CloseWindow();
}

// Ancienne fonction simple (version de secours)
void runMinimalGraphicalDemo() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    
    InitWindow(screenWidth, screenHeight, "Rummikub - Demo Simple");
    SetTargetFPS(60);
    
    SimpleTile demo_tiles[3];
    demo_tiles[0] = createSimpleTile(7, 0, 0);
    demo_tiles[1] = createSimpleTile(7, 1, 0);
    demo_tiles[2] = createSimpleTile(7, 3, 0);
    
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        DrawText("RUMMIKUB - Demo Simple", 200, 50, 40, BLUE);
        
        for (int i = 0; i < 3; i++) {
            drawSimpleTile(demo_tiles[i], 200 + i * 120, 200, 0, 0);
        }
        
        DrawText("Exemple de serie valide", 250, 350, 20, DARKGRAY);
        DrawText("Appuyez sur ECHAP pour quitter", 220, 500, 18, GRAY);
        
        EndDrawing();
    }
    
    CloseWindow();
}

// ============================================
// MENU GRAPHIQUE PRINCIPAL
// ============================================

// Structure pour un bouton
typedef struct {
    Rectangle rect;
    char text[50];
    Color normalColor;
    Color hoverColor;
    Color clickColor;
    int isHovered;
    int isClicked;
} Button;

// Créer un bouton
Button createButton(int x, int y, int width, int height, const char* text) {
    Button btn;
    btn.rect = (Rectangle){x, y, width, height};
    strncpy(btn.text, text, sizeof(btn.text)-1);
    btn.normalColor = (Color){60, 100, 180, 255};
    btn.hoverColor = (Color){80, 120, 200, 255};
    btn.clickColor = (Color){40, 80, 160, 255};
    btn.isHovered = 0;
    btn.isClicked = 0;
    return btn;
}

// Mettre à jour un bouton
void updateButton(Button* btn) {
    Vector2 mousePos = GetMousePosition();
    btn->isHovered = CheckCollisionPointRec(mousePos, btn->rect);
    
    if (btn->isHovered && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        btn->isClicked = 1;
    } else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        btn->isClicked = 0;
    }
}

// Dessiner un bouton avec effets améliorés
void drawButton(Button btn) {
    Color btnColor = btn.normalColor;
    float scale = 1.0f;
    
    if (btn.isClicked) {
        btnColor = btn.clickColor;
        scale = 0.95f;
    } else if (btn.isHovered) {
        btnColor = btn.hoverColor;
        scale = 1.05f;
    }
    
    // Effet d'ombre si survolé
    if (btn.isHovered || btn.isClicked) {
        DrawRectangleRounded((Rectangle){btn.rect.x + 3, btn.rect.y + 3, 
                                          btn.rect.width, btn.rect.height}, 
                             0.3, 10, (Color){0, 0, 0, 100});
    }
    
    // Rectangle principal avec effet de scale
    Rectangle scaledRect = {
        btn.rect.x + (btn.rect.width * (1 - scale)) / 2,
        btn.rect.y + (btn.rect.height * (1 - scale)) / 2,
        btn.rect.width * scale,
        btn.rect.height * scale
    };
    
    DrawRectangleRounded(scaledRect, 0.3, 10, btnColor);
    
    // Bordure avec effet de brillance
    Color borderColor = btn.isHovered ? (Color){255, 255, 255, 200} : WHITE;
    for (int i = 0; i < 2; i++) {
        Rectangle borderRect = {
            scaledRect.x - i, scaledRect.y - i, 
            scaledRect.width + 2*i, scaledRect.height + 2*i
        };
        DrawRectangleRoundedLines(borderRect, 0.3, 10, borderColor);
    }
    
    // Effet de brillance en haut du bouton
    if (btn.isHovered) {
        DrawRectangleGradientV(scaledRect.x, scaledRect.y, 
                              scaledRect.width, scaledRect.height / 3,
                              (Color){255, 255, 255, 30}, 
                              (Color){255, 255, 255, 0});
    }
    
    // Centrer le texte
    int fontSize = 22;
    int textWidth = MeasureText(btn.text, fontSize);
    int textX = btn.rect.x + (btn.rect.width - textWidth) / 2;
    int textY = btn.rect.y + (btn.rect.height - fontSize) / 2;
    
    // Ombre du texte
    DrawText(btn.text, textX + 1, textY + 1, fontSize, (Color){0, 0, 0, 100});
    DrawText(btn.text, textX, textY, fontSize, WHITE);
}

// Afficher les règles dans une fenêtre graphique
void showRulesWindow() {
    const int screenWidth = 900;
    const int screenHeight = 700;
    
    InitWindow(screenWidth, screenHeight, "Rummikub - Regles du Jeu");
    SetTargetFPS(60);
    
    float scrollOffset = 0;
    float animationTime = 0;
    
    while (!WindowShouldClose() && !IsKeyPressed(KEY_ESCAPE)) {
        animationTime += GetFrameTime();
        
        // Gestion du scroll avec la molette
        scrollOffset -= GetMouseWheelMove() * 20;
        if (scrollOffset < 0) scrollOffset = 0;
        if (scrollOffset > 400) scrollOffset = 400;
        
        BeginDrawing();
        
        // Fond avec dégradé
        DrawRectangleGradientV(0, 0, screenWidth, screenHeight,
                              (Color){20, 20, 40, 255},
                              (Color){10, 10, 25, 255});
        
        // En-tête
        DrawRectangle(0, 0, screenWidth, 80, (Color){30, 30, 60, 255});
        DrawText("REGLES DU RUMMIKUB", 200, 20, 50, (Color){220, 180, 60, 255});
        DrawText("Appuyez sur ECHAP pour fermer", 250, 60, 18, LIGHTGRAY);
        
        // Contenu scrollable
        int yPos = 100 - (int)scrollOffset;
        
        // But du jeu
        DrawText("BUT DU JEU", 50, yPos, 32, (Color){220, 180, 60, 255});
        yPos += 50;
        DrawText("Etre le premier joueur a poser toutes ses tuiles sur le plateau.", 
                 70, yPos, 20, WHITE);
        yPos += 40;
        
        // Combinaisons valides
        DrawText("COMBINAISONS VALIDES", 50, yPos, 32, (Color){180, 220, 180, 255});
        yPos += 50;
        
        DrawText("1. SERIE (Groupe)", 70, yPos, 24, (Color){200, 200, 255, 255});
        yPos += 35;
        DrawText("   - 3 ou 4 tuiles de meme valeur", 90, yPos, 18, LIGHTGRAY);
        yPos += 25;
        DrawText("   - Couleurs differentes (obligatoire)", 90, yPos, 18, LIGHTGRAY);
        yPos += 25;
        DrawText("   Exemple: 7 ROUGE, 7 BLEU, 7 JAUNE", 90, yPos, 18, (Color){150, 200, 150, 255});
        yPos += 50;
        
        DrawText("2. SUITE (Sequence)", 70, yPos, 24, (Color){200, 200, 255, 255});
        yPos += 35;
        DrawText("   - Minimum 3 tuiles consecutives", 90, yPos, 18, LIGHTGRAY);
        yPos += 25;
        DrawText("   - Meme couleur (obligatoire)", 90, yPos, 18, LIGHTGRAY);
        yPos += 25;
        DrawText("   Exemple: 1 ROUGE, 2 ROUGE, 3 ROUGE", 90, yPos, 18, (Color){150, 200, 150, 255});
        yPos += 50;
        
        // Premier tour
        DrawText("PREMIER TOUR", 50, yPos, 32, (Color){255, 180, 100, 255});
        yPos += 50;
        DrawText("Pour pouvoir poser des tuiles lors du premier tour,", 70, yPos, 20, WHITE);
        yPos += 30;
        DrawText("vous devez poser des combinaisons valides d'une valeur", 70, yPos, 20, WHITE);
        yPos += 30;
        DrawText("minimum de 30 points.", 70, yPos, 20, WHITE);
        yPos += 50;
        
        // Joker
        DrawText("LE JOKER", 50, yPos, 32, (Color){200, 100, 100, 255});
        yPos += 50;
        DrawText("Le joker peut remplacer n'importe quelle tuile.", 70, yPos, 20, WHITE);
        yPos += 30;
        DrawText("Si vous gardez un joker dans votre main a la fin", 70, yPos, 20, WHITE);
        yPos += 30;
        DrawText("de la partie, il compte pour 30 points negatifs.", 70, yPos, 20, WHITE);
        yPos += 50;
        
        // Actions
        DrawText("ACTIONS POSSIBLES", 50, yPos, 32, (Color){180, 180, 255, 255});
        yPos += 50;
        DrawText("• Piocher une tuile du sac", 70, yPos, 20, WHITE);
        yPos += 30;
        DrawText("• Poser des combinaisons valides sur le plateau", 70, yPos, 20, WHITE);
        yPos += 30;
        DrawText("• Modifier des combinaisons existantes", 70, yPos, 20, WHITE);
        yPos += 30;
        DrawText("• Remplacer un joker par la tuile qu'il represente", 70, yPos, 20, WHITE);
        yPos += 50;
        
        // Fin de partie
        DrawText("FIN DE PARTIE", 50, yPos, 32, (Color){255, 220, 100, 255});
        yPos += 50;
        DrawText("La partie se termine lorsqu'un joueur pose toutes ses tuiles.", 70, yPos, 20, WHITE);
        yPos += 30;
        DrawText("Les autres joueurs comptent les points de leurs tuiles restantes.", 70, yPos, 20, WHITE);
        yPos += 30;
        DrawText("Le joueur avec le moins de points gagne la manche.", 70, yPos, 20, WHITE);
        
        // Indicateur de scroll
        if (scrollOffset > 0) {
            DrawTriangle((Vector2){screenWidth - 30, 100},
                         (Vector2){screenWidth - 20, 90},
                         (Vector2){screenWidth - 10, 100},
                         YELLOW);
        }
        if (scrollOffset < 400) {
            DrawTriangle((Vector2){screenWidth - 30, screenHeight - 100},
                         (Vector2){screenWidth - 20, screenHeight - 90},
                         (Vector2){screenWidth - 10, screenHeight - 100},
                         YELLOW);
        }
        
        EndDrawing();
    }
    
    CloseWindow();
}

// Menu principal graphique amélioré
void showGraphicalMenu() {
    const int screenWidth = 1200;
    const int screenHeight = 800;
    
    InitWindow(screenWidth, screenHeight, "Rummikub - Menu Principal");
    SetTargetFPS(60);
    
    // Créer les boutons avec meilleur espacement
    Button buttons[5];
    int buttonStartY = 250;
    int buttonSpacing = 90;
    int buttonWidth = 400;
    int buttonHeight = 70;
    int buttonX = (screenWidth - buttonWidth) / 2;
    
    buttons[0] = createButton(buttonX, buttonStartY, buttonWidth, buttonHeight, "Nouvelle Partie");
    buttons[1] = createButton(buttonX, buttonStartY + buttonSpacing, buttonWidth, buttonHeight, "Charger Partie");
    buttons[2] = createButton(buttonX, buttonStartY + buttonSpacing * 2, buttonWidth, buttonHeight, "Scores");
    buttons[3] = createButton(buttonX, buttonStartY + buttonSpacing * 3, buttonWidth, buttonHeight, "Regles");
    buttons[4] = createButton(buttonX, buttonStartY + buttonSpacing * 4, buttonWidth, buttonHeight, "Quitter");
    
    // Améliorer les couleurs des boutons
    buttons[0].normalColor = (Color){80, 150, 80, 255};   // Vert pour nouvelle partie
    buttons[0].hoverColor = (Color){100, 180, 100, 255};
    buttons[1].normalColor = (Color){100, 120, 200, 255}; // Bleu pour charger
    buttons[1].hoverColor = (Color){120, 140, 220, 255};
    buttons[2].normalColor = (Color){200, 150, 80, 255};  // Orange pour scores
    buttons[2].hoverColor = (Color){220, 170, 100, 255};
    buttons[3].normalColor = (Color){180, 100, 180, 255}; // Violet pour règles
    buttons[3].hoverColor = (Color){200, 120, 200, 255};
    buttons[4].normalColor = (Color){180, 60, 60, 255};   // Rouge pour quitter
    buttons[4].hoverColor = (Color){200, 80, 80, 255};
    
    int selectedOption = -1;
    float animationTime = 0;
    
    while (!WindowShouldClose() && selectedOption == -1) {
        animationTime += GetFrameTime();
        
        // Mettre à jour les boutons
        for (int i = 0; i < 5; i++) {
            updateButton(&buttons[i]);
            
            if (buttons[i].isHovered && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                selectedOption = i;
            }
        }
        
        // Touches clavier
        if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1)) selectedOption = 0;
        if (IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_KP_2)) selectedOption = 1;
        if (IsKeyPressed(KEY_THREE) || IsKeyPressed(KEY_KP_3)) selectedOption = 2;
        if (IsKeyPressed(KEY_FOUR) || IsKeyPressed(KEY_KP_4)) selectedOption = 3;
        if (IsKeyPressed(KEY_FIVE) || IsKeyPressed(KEY_KP_5) || IsKeyPressed(KEY_ESCAPE)) selectedOption = 4;
        
        // ===== DESSIN =====
        BeginDrawing();
        
        // Fond avec dégradé amélioré
        DrawRectangleGradientV(0, 0, screenWidth, screenHeight,
                              (Color){25, 25, 50, 255},
                              (Color){15, 15, 35, 255});
        
        // Motif décoratif en arrière-plan
        for (int i = 0; i < 50; i++) {
            float time = animationTime + i * 0.2f;
            int x = (int)(sin(time) * 150 + (i % 10) * 120);
            int y = (int)(cos(time * 0.8f) * 80 + (i / 10) * 100);
            float size = 1.5f + sin(time * 3 + i) * 0.8f;
            float alpha = 20 + sin(time * 2) * 15;
            DrawCircle(x, y, size, (Color){255, 255, 255, (unsigned char)alpha});
        }
        
        // En-tête avec effet de brillance
        DrawRectangle(0, 0, screenWidth, 120, (Color){20, 20, 40, 200});
        DrawRectangleGradientV(0, 0, screenWidth, 120,
                              (Color){30, 30, 60, 255},
                              (Color){20, 20, 40, 255});
        
        // Ligne décorative animée sous l'en-tête
        float pulse = sin(animationTime * 2) * 0.3f + 0.7f;
        DrawRectangle(0, 120, screenWidth, 3, (Color){220, 180, 60, (unsigned char)(200 * pulse)});
        
        // Titre avec effet de pulsation amélioré
        float titlePulse = sin(animationTime * 2.5f) * 0.08f + 0.92f;
        int titleSize = (int)(75 * titlePulse);
        int titleX = (screenWidth - MeasureText("RUMMIKUB", titleSize)) / 2;
        
        // Ombre du titre
        DrawText("RUMMIKUB", titleX + 3, 23, titleSize, (Color){0, 0, 0, 150});
        // Titre principal
        DrawText("RUMMIKUB", titleX, 20, titleSize, (Color){220, 180, 60, 255});
        
        // Effet de brillance sur le titre (dégradé horizontal)
        DrawRectangleGradientH(titleX, 20, MeasureText("RUMMIKUB", titleSize), titleSize,
                               (Color){255, 255, 255, 0},
                               (Color){255, 255, 255, 30});
        
        // Sous-titre centré
        int subtitleX = (screenWidth - MeasureText("Projet Algorithmique - ISTY IATIC3", 22)) / 2;
        DrawText("Projet Algorithmique - ISTY IATIC3", subtitleX, 75, 22, (Color){180, 180, 220, 255});
        
        // Dessiner les boutons
        for (int i = 0; i < 5; i++) {
            drawButton(buttons[i]);
        }
        
        // Instructions améliorées
        int instructionY = screenHeight - 80;
        DrawRectangle(0, instructionY, screenWidth, 80, (Color){15, 15, 30, 200});
        DrawText("Utilisez la souris ou les touches 1-5 pour selectionner", 
                 (screenWidth - MeasureText("Utilisez la souris ou les touches 1-5 pour selectionner", 18)) / 2,
                 instructionY + 15, 18, LIGHTGRAY);
        
        // Afficher les raccourcis clavier à côté des boutons avec style amélioré
        const char* shortcuts[] = {"[1]", "[2]", "[3]", "[4]", "[5/ECHAP]"};
        for (int i = 0; i < 5; i++) {
            int shortcutX = buttonX - 80;
            int shortcutY = buttonStartY + i * buttonSpacing + (buttonHeight - 16) / 2;
            
            // Fond du raccourci
            DrawRectangleRounded((Rectangle){shortcutX - 5, shortcutY - 2, 50, 20}, 
                                 0.2, 5, (Color){40, 40, 60, 200});
            DrawText(shortcuts[i], shortcutX, shortcutY, 16, YELLOW);
        }
        
        // Version et informations
        DrawText("Version 1.0 - Decembre 2025", 20, screenHeight - 30, 16, (Color){150, 150, 200, 255});
        
        // FPS en haut à droite
        DrawText(TextFormat("FPS: %d", GetFPS()), screenWidth - 100, 10, 18, GREEN);
        
        EndDrawing();
    }
    
    CloseWindow();
    
    // Gérer la sélection
    if (selectedOption != -1) {
        printf("\n═══════════════════════════════════════\n");
        printf("SELECTION DU MENU GRAPHIQUE\n");
        printf("═══════════════════════════════════════\n");
        
        switch (selectedOption) {
            case 0: // Nouvelle partie
                printf("Option: Nouvelle Partie\n");
                printf("Lancement de la demonstration graphique...\n");
                runImprovedGraphicalDemo();
                break;
            case 1: // Charger
                printf("Option: Charger Partie\n");
                printf("Fonctionnalite a venir dans la version finale\n");
                break;
            case 2: // Scores
                printf("Option: Scores\n");
                printf("Meilleurs scores en cours de developpement\n");
                break;
            case 3: // Règles - Afficher dans une fenêtre graphique
                printf("Option: Regles\n");
                printf("Ouverture de la fenetre des regles...\n");
                showRulesWindow();
                break;
            case 4: // Quitter
                printf("Option: Quitter\n");
                printf("Retour au menu principal...\n");
                break;
        }
        
        printf("Appuyez sur Entree pour continuer...");
        clearInputBuffer();
    }
}
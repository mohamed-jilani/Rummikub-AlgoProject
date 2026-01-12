#include <stdio.h>
#include <math.h>  // AJOUTER pour sin/cos
#include "raylib.h"

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
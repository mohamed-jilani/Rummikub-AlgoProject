// tile_wrapper.h - Wrapper pour éviter le conflit entre TileColor (enum projet) et Color (Raylib)
#ifndef TILE_WRAPPER_H
#define TILE_WRAPPER_H

// Inclure raylib en premier
#include "raylib.h"
typedef Color RaylibColor;

// Undef les macros de Raylib qui entrent en conflit avec l'enum
#undef RED
#undef BLUE
#undef BLACK
#undef YELLOW

// Inclure les headers du projet (TileColor ne devrait plus entrer en conflit)
#include "../core/tile.h"
#include "../core/combination.h"
#include "../core/board.h"
#include "../core/player.h"

// Les valeurs de l'enum TileColor: RED=0, BLUE=1, BLACK=2, YELLOW=3, NO_COLOR=4
#define TILE_COLOR_RED 0
#define TILE_COLOR_BLUE 1
#define TILE_COLOR_BLACK 2
#define TILE_COLOR_YELLOW 3
#define TILE_COLOR_NO_COLOR 4

// Déclarations des fonctions de couleur (définies dans raylib_renderer.c)
RaylibColor getTileColor(Tile tile);
RaylibColor getTileTextColor(Tile tile);

#endif


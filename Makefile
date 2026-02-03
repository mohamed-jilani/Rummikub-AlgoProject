# Makefile - Version corrigée
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I./src
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Répertoires
SRC_DIR = src
CORE_DIR = $(SRC_DIR)/core
LOGIC_DIR = $(SRC_DIR)/logic
DATA_DIR = $(SRC_DIR)/data
GRAPHICS_DIR = $(SRC_DIR)/graphics
UTILS_DIR = $(SRC_DIR)/utils
IA_DIR = $(SRC_DIR)/ia

# Fichiers sources (AJOUTER graphics/)
SRC = $(SRC_DIR)/main.c \
      $(CORE_DIR)/tile.c \
      $(CORE_DIR)/player.c \
      $(CORE_DIR)/combination.c \
      $(CORE_DIR)/board.c \
      $(CORE_DIR)/game.c \
      $(LOGIC_DIR)/validation.c \
      $(LOGIC_DIR)/rules.c \
      $(LOGIC_DIR)/scoring.c \
      $(DATA_DIR)/file_manager.c \
      $(GRAPHICS_DIR)/graphics.c \
      $(GRAPHICS_DIR)/simple_graphics.c \
      $(GRAPHICS_DIR)/raylib_renderer.c \
      $(GRAPHICS_DIR)/raylib_game.c \
      $(GRAPHICS_DIR)/ui_components.c \
      $(GRAPHICS_DIR)/main_menu.c \
      $(DATA_DIR)/players_manager.c \
      $(DATA_DIR)/scores_manager.c \
      $(DATA_DIR)/save_manager.c \
      $(UTILS_DIR)/random.c \
      $(UTILS_DIR)/helpers.c \
      $(IA_DIR)/ai.c

# Fichiers objets
OBJ = $(SRC:.c=.o)

# Exécutable
EXEC = rummikub

# Règles principales
all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Règles utilitaires
clean:
	rm -f $(OBJ) $(EXEC) test_simple

run: all
	./$(EXEC)

# Compiler le test simple de l'interface graphique
test_simple: test_simple.c
	$(CC) $(CFLAGS) test_simple.c -o test_simple $(LDFLAGS)

# Compiler le test de l'interface Raylib (sans main.o pour éviter le conflit)
OBJ_NO_MAIN = $(filter-out src/main.o, $(OBJ))
test_raylib: test_raylib.c $(OBJ_NO_MAIN)
	$(CC) $(CFLAGS) test_raylib.c -o test_raylib $(OBJ_NO_MAIN) $(LDFLAGS)

# Compiler le test des nouvelles fonctions graphiques
test_new_functions: test_new_functions.c $(OBJ_NO_MAIN)
	$(CC) $(CFLAGS) test_new_functions.c -o test_new_functions $(OBJ_NO_MAIN) $(LDFLAGS)

# Compiler le test de validation
test_validation: test_validation.c $(OBJ_NO_MAIN)
	$(CC) $(CFLAGS) test_validation.c -o test_validation $(OBJ_NO_MAIN) $(LDFLAGS)

# Créer la structure des dossiers
init:
	mkdir -p $(GRAPHICS_DIR) assets data

.PHONY: all clean run init test_simple
# Makefile
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I./src
LDFLAGS = -lm

# Répertoires
SRC_DIR = src
CORE_DIR = $(SRC_DIR)/core
LOGIC_DIR = $(SRC_DIR)/logic
DATA_DIR = $(SRC_DIR)/data
UTILS_DIR = $(SRC_DIR)/utils
IA_DIR = $(SRC_DIR)/ia

# Fichiers sources
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
      $(UTILS_DIR)/random.c \
      $(UTILS_DIR)/helpers.c

# Fichiers objets
OBJ = $(SRC:.c=.o)

# Exécutable
EXEC = rummikub

# Règles
all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)

run: all
	./$(EXEC)

# Règle pour générer la structure
init:
	mkdir -p assets/fonts assets/tiles
	mkdir -p data
	mkdir -p src/core src/logic src/data src/graphics src/ia src/utils
	mkdir -p tests

.PHONY: all clean run init
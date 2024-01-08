# Variables
CC = gcc
CPPFLAGS = -Iinclude
CFLAGS = -Wall -Wextra -Werror -O2
DEBUG_FLAGS = -g -fsanitize=address

# Répertoires
SRC_DIR = ../src
OBJ_DIR = build

# Fichiers sources et objets
MAIN_SOURCE = $(SRC_DIR)/main.c
CJSON_SOURCE = $(SRC_DIR)/cJSON.c
STR_SOURCE = $(SRC_DIR)/str.C
JSON_FUNCTION_SOURCE = $(SRC_DIR)/json_function.c

MAIN_OBJECT = $(OBJ_DIR)/main.o
CJSON_OBJECT = $(OBJ_DIR)/cJSON.o
STR_OBJECT = $(OBJ_DIR)/str.o
JSON_FUNCTION_OBJECT = $(OBJ_DIR)/json_function.o

TARGET = mon_app

# Règle par défaut
all: CFLAGS += $(DEBUG_FLAGS)
all: directories $(TARGET)

# Création des répertoires
directories:
	mkdir -p $(OBJ_DIR)

# Règles de compilation pour les fichiers source
$(MAIN_OBJECT): $(MAIN_SOURCE)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(CJSON_OBJECT): $(CJSON_SOURCE)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(STR_OBJECT): $(STR_SOURCE)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(JSON_FUNCTION_OBJECT): $(JSON_FUNCTION_SOURCE)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Règle de lien pour l'application principale
$(TARGET): $(MAIN_OBJECT) $(CJSON_OBJECT) $(STR_OBJECT) $(JSON_FUNCTION_OBJECT)
	$(CC) $(CFLAGS) $^ -o $@

# Nettoyage sélectif
clean:
	rm -f $(MAIN_OBJECT) $(CJSON_OBJECT) $(STR_OBJECT) $(JSON_FUNCTION_OBJECT) $(TARGET)

.PHONY: all directories clean

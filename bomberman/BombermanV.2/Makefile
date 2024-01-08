# variables de compilations 
CC = gcc
CPPFLAGS = server/lib/cJSON.c
CFLAGS = -Wall -Wextra -Werror -O2 
DEBUG_FLAGS = -g 
TARGET = my_server

all: CFLAGS += $(DEBUG_FLAGS)
all:$(TARGET)

# Fichiers sources et objet
SRC_DIR = server/src
OBJ_DIR = server/build

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)


$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) -o $@ -c $< $(CFLAGS)

$(OBJ_DIR) :
	mkdir -p $(OBJ_DIR)


$(TARGET) : $(OBJ)
	$(CC) $(CPPFLAGS) -o $@ $^ $(CFLAGS)

# Règle pour le nettoyage des fichiers objets et de l'exécutable
clean :
	rm -rf $(OBJ_DIR) $(TARGET)


# commande pour lancer l'exécutable
exec:
	./$(TARGET)

# commande pour lancer l'exécutable avec valgrind
valgrind:
	valgrind --leak-check=full ./$(TARGET)

# variable spéciales
# $@ nom cible 
# $< nom de la première dépendance
# $^ liste dépendances 
# $? liste dépendances plus récentes que la cible
# $* nom ficher, sans son extension

# Nom de l’exécutable
NAME = prog

# Compilateur + options
CC = gcc
CFLAGS = -Wall -Wextra -I/usr/include -Iminilibx-linux -Iheaders -O3

# Dossiers
SRC_DIR = src
OBJ_DIR = obj

# Fichiers sources et objets
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

# --- Règles ---
all: $(OBJ_DIR) $(NAME)

# Crée le dossier obj si nécessaire
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Création de l'exécutable
$(NAME): $(OBJ)
	$(CC) $(OBJ) -Lminilibx-linux -lmlx_Linux -L/usr/lib -lXext -lX11 -lm -lz -o $(NAME)

# Compilation des fichiers .c en .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage des objets
clean:
	rm -f $(OBJ)

# Nettoyage complet
fclean: clean
	rm -f $(OBJ_DIR)/$(NAME)

re: fclean all


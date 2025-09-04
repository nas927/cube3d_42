# Nom de l’exécutable
NAME = prog

# Compilateur + options
CC = gcc
CFLAGS = -Wall -Wextra -I/usr/include -Iminilibx-linux -O3

# Fichiers sources et objets
SRC = main.c
OBJ = $(SRC:.c=.o)

# --- Règles ---
all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(OBJ) -Lminilibx-linux -lmlx_Linux -L/usr/lib -Iminilibx-linux -lXext -lX11 -lm -lz -o $(NAME)

# Règle générique (ta ligne)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all


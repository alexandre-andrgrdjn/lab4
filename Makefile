# Compiler
CC = gcc
CFLAGS = -Wall -Wextra -pthread

# Nom de l'exécutable
TARGET = airport_simulation

# Fichier source
SRC = airport_simulation.c

# Fichier objet
OBJ = $(SRC:.c=.o)

# Règle principale : compiler le programme
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage des fichiers objets et de l'exécutable
clean:
	rm -f $(OBJ) $(TARGET)

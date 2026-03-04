# Variables de compilation
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
# L'édition de liens (LDFLAGS) a besoin de ncurses
LDFLAGS = -lncurses

# Dossiers
SRCDIR = src
BINDIR = build

# On liste les sources et on définit les objets dans un dossier séparé (optionnel mais propre)
SRC = $(wildcard $(SRCDIR)/*.c)
OBJ = $(SRC:.c=.o)
EXEC = $(BINDIR)/ftl_rpg

all: $(BINDIR) $(EXEC)
	rm -f $(SRCDIR)/*.o
	@echo "Exécutable prêt et fichiers objets supprimés !"

$(BINDIR):
	mkdir -p $(BINDIR)

# C'est ICI que l'on ajoute LDFLAGS pour lier ncurses à l'exécutable
$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(LDFLAGS)

# La compilation des .o n'a pas besoin de -lncurses, juste des headers
$(SRCDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SRCDIR)/*.o $(EXEC)
	rm -rf $(BINDIR)

.PHONY: all clean
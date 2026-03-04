# Variables de compilation
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
# L'édition de liens (LDFLAGS) a besoin de ncurses
LDFLAGS = -lncurses

# Dossiers
SRCDIR = src
BINDIR = build

SRC = $(wildcard $(SRCDIR)/*.c)
OBJ = $(SRC:.c=.o)
EXEC = $(BINDIR)/ftl_rpg

all: $(BINDIR) $(EXEC)
	rm -f $(SRCDIR)/*.o
	@echo "Exécutable prêt et fichiers objets supprimés !"

$(BINDIR):
	mkdir -p $(BINDIR)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(LDFLAGS)

$(SRCDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SRCDIR)/*.o $(EXEC)
	rm -rf $(BINDIR)

.PHONY: all clean
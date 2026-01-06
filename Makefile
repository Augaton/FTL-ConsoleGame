# Variables de compilation
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
LDFLAGS = 

# Dossiers
SRCDIR = src
INCDIR = include
BINDIR = build

SRC = $(wildcard $(SRCDIR)/*.c)
OBJ = $(SRC:$(SRCDIR)/%.c=$(SRCDIR)/%.o)
EXEC = $(BINDIR)/ftl_rpg

all: $(BINDIR) $(EXEC)
	rm -f $(SRCDIR)/*.o
	@echo "Compilation terminée et fichiers objets nettoyés !"

$(BINDIR):
	mkdir -p $(BINDIR)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(LDFLAGS)

$(SRCDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(SRCDIR)/*.o $(BINDIR)

.PHONY: all clean

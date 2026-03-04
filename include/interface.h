#ifndef INTERFACE_H
#define INTERFACE_H

#include "vaisseau.h"

// Structure pour lier un texte à une valeur de retour
typedef struct {
    char *label;
    int id;
} OptionMenu;

// Initialisation/terminaison ncurses (une seule fois)
void initialiserNCurses(void);
void terminerNCurses(void);

// Affiche un menu et retourne l'ID de l'option choisie
int menuInteractif(const char *titre, OptionMenu options[], int nbOptions, Vaisseau *v);

// Affiche un dialogue simple avec message
void afficherDialogueSimple(const char *titre, const char *message);

// Menus de combat
int menuCombatAction(Vaisseau *v);
int menuChoixCible(int chanceCoque, int chanceSysteme, Vaisseau *v);
int menuChoixArme(int missiles, Vaisseau *v);

// Menu oui/non rapide
int demanderConfirmation(const char *titre, const char *question);

#endif
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

// Menus du magasin
int menuMagasinPrincipal(int ferraille, const char *promo, int pourcentPromo);
int menuMaintenance(int coqueMax, int coqueActuelle, int missiles, int carburant);
int menuUpgrades(int ferraille);
int menuServices(int ferraille);
int menuPersonnel(int ferraille);
int menuRecrutement(const char *nom1, const char *nom2, const char *nom3, int prix1, int prix2, int prix3);

// Menus du vaisseau (état)
int menuEtatVaisseauActions(void);
int menuMutation(const char *nomMembre, const char *roleActuel);

// Menu oui/non rapide
int demanderConfirmation(const char *titre, const char *question);

#endif
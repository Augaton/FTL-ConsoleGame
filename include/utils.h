#ifndef UTILS_H
#define UTILS_H

#include "vaisseau.h"

#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_RESET   "\x1b[0m"
#define COLOR_BOLD    "\x1b[1m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_BLUE    "\x1b[34m"

// Détection du système d'exploitation pour la pause
#ifdef _WIN32
    #include <windows.h>
    #define SLEEP_MS(ms) Sleep(ms)
#else
    #include <unistd.h>
    #define SLEEP_MS(ms) usleep((ms) * 1000)
#endif

void effacerEcran();
void afficherGameOver(Vaisseau *joueur);
void afficherVictoire(Vaisseau *joueur);
void attendreJoueur();

void sauvegarderPartie(Vaisseau *joueur);
int chargerPartie(Vaisseau *joueur);
void supprimerSauvegarde();
void finaliserEvenement(Vaisseau *joueur);

#endif
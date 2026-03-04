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
#define COLOR_WHITE   "\x1b[37m"
#define COLOR_BLACK   "\x1b[30m"

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
void afficherDestinationColoree(const char* destination);

void sauvegarderPartie(Vaisseau *joueur);
int chargerPartie(Vaisseau *joueur);
void supprimerSauvegarde();
void finaliserEvenement(Vaisseau *joueur);

int lireEntierSecurise(int min, int max);
int lireChoix(int defaut);

// ============================================================
// CONSTANTES DE JEU
// ============================================================

// --- Marchand ambulant ---
#define MARCHAND_COUT_CARBURANT   10
#define MARCHAND_COUT_MISSILES    15
#define MARCHAND_GAIN_CARBURANT    2
#define MARCHAND_GAIN_MISSILES     3

// --- Péage pirate ---
#define PEAGE_PIRATE_COUT         15

// --- Station mercenaire ---
#define MERCENAIRE_COUT           40
#define MERCENAIRE_PV            120

// --- Casino ---
#define CASINO_MISE_MIN           10
#define CASINO_MISE_MAX           50
#define CASINO_GAIN_PETIT         20
#define CASINO_GAIN_GROS         150
#define CASINO_CHANCE_PETIT       45   // % de gagner la petite mise
#define CASINO_CHANCE_GROS        25   // % de gagner la grosse mise

// --- Ermite ---
#define ERMITE_COUT_RENFORT       10
#define ERMITE_BONUS_COQUE_MAX     5

#endif
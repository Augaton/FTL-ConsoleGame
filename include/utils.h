#ifndef UTILS_H
#define UTILS_H

// Secteur final
#define DISTANCE_FINALE 20

#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_RESET   "\x1b[0m"
#define COLOR_BOLD    "\x1b[1m"


// Détection du système d'exploitation pour la pause
#ifdef _WIN32
    #include <windows.h>
    #define SLEEP_MS(ms) Sleep(ms)
#else
    #include <unistd.h>
    #define SLEEP_MS(ms) usleep((ms) * 1000)
#endif

// Fonction pour effacer l'écran (optionnel mais génial pour l'immersion)
void effacerEcran();

#endif
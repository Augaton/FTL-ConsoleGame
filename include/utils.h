#ifndef UTILS_H
#define UTILS_H

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
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

// Secteur final
#define DISTANCE_FINALE 20

#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_RESET   "\x1b[0m"
#define COLOR_BOLD    "\x1b[1m"

void effacerEcran() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

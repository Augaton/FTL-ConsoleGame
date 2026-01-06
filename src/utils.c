#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

void effacerEcran() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}
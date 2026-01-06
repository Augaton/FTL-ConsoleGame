#include "magasin.h"
#include "utils.h"
#include <stdio.h>

void ouvrirMagasin(Vaisseau *joueur) {
    int choix = 0;
    while (choix != 5) { // Option 5 pour quitter
        effacerEcran();
        printf("--- STATION COMMERCIALE ---\n");
        printf("Ferraille disponible : %d\n", joueur->ferraille);
        printf("Etat du vaisseau : Coque [%d/%d] | Bouclier Max [%d] | Missiles [%d]\n", 
                joueur->coque, joueur->coqueMax, joueur->bouclierMax, joueur->missiles);
        printf("---------------------------\n");
        printf("1. Reparer Coque (+5)       - 10 Ferrailles\n");
        printf("2. Acheter Missiles (+3)    - 15 Ferrailles\n");
        printf("3. Ameliorer Lasers (+1)    - 40 Ferrailles\n");
        printf("4. Ameliorer Bouclier (+1)  - 50 Ferrailles\n");
        printf("5. Quitter la station\n");
        printf("\nVotre choix : ");
        scanf("%d", &choix);

        switch(choix) {
            case 1: // Réparation
                if (joueur->ferraille >= 10 && joueur->coque < joueur->coqueMax) {
                    joueur->ferraille -= 10;
                    joueur->coque += 5;
                    if (joueur->coque > joueur->coqueMax) joueur->coque = joueur->coqueMax;
                    printf("Coque reparee !\n");
                } else {
                    printf("Action impossible (Ferraille insuffisante ou coque pleine).\n");
                }
                break;

            case 2: // Missiles
                if (joueur->ferraille >= 15) {
                    joueur->ferraille -= 15;
                    joueur->missiles += 3;
                    printf("Missiles charges !\n");
                } else {
                    printf("Pas assez de ferraille...\n");
                }
                break;

            case 3: // Lasers
                if (joueur->ferraille >= 40) {
                    joueur->ferraille -= 40;
                    joueur->armes += 1;
                    printf("Lasers ameliores ! Puissance : %d\n", joueur->armes);
                } else {
                    printf("Ferraille insuffisante.\n");
                }
                break;

            case 4: // Bouclier Max
                if (joueur->ferraille >= 50) {
                    joueur->ferraille -= 50;
                    joueur->bouclierMax += 1;
                    printf("Capacite du bouclier augmentee : %d\n", joueur->bouclierMax);
                } else {
                    printf("Ferraille insuffisante.\n");
                }
                break;

            case 5:
                printf("Depart de la station...\n");
                break;

            default:
                printf("Choix invalide.\n");
        }
        SLEEP_MS(1200); // Laisse le temps de lire
    }
}
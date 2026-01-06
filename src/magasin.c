#include "magasin.h"
#include "utils.h"
#include <stdio.h>

void ouvrirMagasin(Vaisseau *joueur) {
    int categorie = 0;
    while (categorie != 4) { // 4 pour quitter le magasin
        effacerEcran();
        printf("--- STATION COMMERCIALE ---\n");
        printf("Ferraille : %d | Coque : %d/%d | Missiles : %d\n", 
                joueur->ferraille, joueur->coque, joueur->coqueMax, joueur->missiles);
        printf("---------------------------\n");
        printf("1. [MAINTENANCE] (Reparations & Munitions)\n");
        printf("2. [UPGRADES]    (Armes, Boucliers, Moteurs)\n");
        printf("3. [SERVICES]    (Vendre du carburant, ect...)\n");
        printf("4. QUITTER\n");
        printf("\nChoisir categorie : ");
        scanf("%d", &categorie);

        if (categorie == 1) {
            // --- SOUS-MENU MAINTENANCE ---
            int choix = 0;
            printf("\n-- MAINTENANCE --\n");
            printf("1. Reparer Coque (+5)  - 10 Fer\n");
            printf("2. Acheter Missiles (+3) - 15 Fer\n");
            printf("3. Acheter Carburant (+?) - 5 Fer/Unités\n");
            printf("4. Retour\n");
            scanf("%d", &choix);

            if (choix == 1 && joueur->ferraille >= 10 && joueur->coque < joueur->coqueMax) {
                joueur->ferraille -= 10;
                joueur->coque = (joueur->coque + 5 > joueur->coqueMax) ? joueur->coqueMax : joueur->coque + 5;
                printf("Reparation OK.\n");
            } else if (choix == 2 && joueur->ferraille >= 15) {
                joueur->ferraille -= 15;
                joueur->missiles += 3;
                printf("Missiles recus.\n");
            } else if (choix == 3) {
                int quantite;
                printf("Quantite de carburant a acheter : ");
                scanf("%d", &quantite);
                int coutTotal = quantite * 5;
                if (joueur->ferraille >= coutTotal) {
                    joueur->ferraille -= coutTotal;
                    joueur->carburant += quantite;
                    printf("Carburant ajoute.\n");
                } else {
                    printf("Fond insuffisants pour cette quantite.\n");
                }
            }
        } 
        else if (categorie == 2) {
            // --- SOUS-MENU UPGRADES ---
            int choix = 0;
            printf("\n-- AMELIORATIONS --\n");
            printf("1. Laser +1 (Actuel: %d)     - 40 Fer\n", joueur->armes);
            printf("2. Bouclier +1 (Actuel: %d)  - 50 Fer\n", joueur->bouclierMax);
            printf("3. Moteurs +1 (Esquive)      - 30 Fer\n");
            printf("4. Retour\n");
            scanf("%d", &choix);

            if (choix == 1 && joueur->ferraille >= 40) {
                joueur->ferraille -= 40;
                joueur->armes++;
                printf("Puissance de feu augmentee !\n");
            } else if (choix == 2 && joueur->ferraille >= 50) {
                joueur->ferraille -= 50;
                joueur->bouclierMax++;
                printf("Generateur de bouclier ameliore !\n");
            } else if (choix == 3 && joueur->ferraille >= 30) {
                joueur->ferraille -= 30;
                joueur->moteurs++;
                printf("Moteurs pousses ! Esquive amelioree.\n");
            }
        }
        else if (categorie == 3) {
             printf("\n[SERVICE] Aucune promotion disponible actuellement.\n");
        }

        SLEEP_MS(1000);
    }
}
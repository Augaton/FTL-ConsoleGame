#include "magasin.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

void ouvrirMagasin(Vaisseau *joueur) {
    int categorie = 0;
    
    // --- GÉNÉRATION DU STOCK ALÉATOIRE POUR CETTE STATION ---
    int stockMissiles = (rand() % 4) + 2;     // 2 à 5 lots de missiles
    int stockCarburant = (rand() % 6) + 5;    // 5 à 10 unités de carburant
    int aVenduUpgrade = 0;                    // On ne peut acheter qu'une grosse amélioration par station

    while (categorie != 4) {
        effacerEcran();
        
        // --- HEADER MAGASIN ---
        printf(COLOR_GREEN "╔══════════════════════════════════════════════════════════╗\n");
        printf("║ " COLOR_BOLD "🛒 DOCK COMMERCIAL" COLOR_RESET COLOR_GREEN "          CRÉDITS: " COLOR_YELLOW "%-5d ⚓" COLOR_GREEN " ║\n", joueur->ferraille);
        printf("╠══════════════════════════════════════════════════════════╣" COLOR_RESET "\n");
        printf(COLOR_GREEN "║ " COLOR_CYAN "1. [MAINTENANCE]" COLOR_RESET "  Réparations & Munitions            " COLOR_GREEN "║\n");
        printf(COLOR_GREEN "║ " COLOR_RED "2. [UPGRADES]   " COLOR_RESET "  Systèmes du Vaisseau               " COLOR_GREEN "║\n");
        printf(COLOR_GREEN "║ " COLOR_YELLOW "3. [SERVICES]   " COLOR_RESET "  Marché noir & Recyclage            " COLOR_GREEN "║\n");
        printf(COLOR_GREEN "║ " COLOR_RESET "4. [QUITTER]    " COLOR_RESET "  Reprendre la navigation            " COLOR_GREEN "║\n");
        printf(COLOR_GREEN "╚══════════════════════════════════════════════════════════╝" COLOR_RESET "\n");
        
        printf("\n " COLOR_YELLOW "Choisir catégorie > " COLOR_RESET);
        if (scanf("%d", &categorie) != 1) {
            int c; while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }

        if (categorie == 1) {
            // --- SOUS-MENU MAINTENANCE ---
            int choix = 0;
            printf("\n" COLOR_CYAN "─── MAINTENANCE ───" COLOR_RESET "\n");
            printf("1. Réparer Coque (+5)  | 10 Fer. | (Besoin: %d)\n", joueur->coqueMax - joueur->coque);
            printf("2. Missiles (+3)       | 15 Fer. | Stock: %d\n", stockMissiles);
            printf("3. Carburant (x1)      | 05 Fer. | Stock: %d\n", stockCarburant);
            printf("4. Retour\n");
            printf(COLOR_YELLOW "> " COLOR_RESET);
            scanf("%d", &choix);

            if (choix == 1 && joueur->ferraille >= 10 && joueur->coque < joueur->coqueMax) {
                joueur->ferraille -= 10;
                joueur->coque = (joueur->coque + 5 > joueur->coqueMax) ? joueur->coqueMax : joueur->coque + 5;
                printf(COLOR_GREEN "✔ Réparation effectuée.\n" COLOR_RESET);
            } 
            else if (choix == 2 && stockMissiles > 0 && joueur->ferraille >= 15) {
                joueur->ferraille -= 15;
                joueur->missiles += 3;
                stockMissiles--;
                printf(COLOR_GREEN "✔ Missiles chargés.\n" COLOR_RESET);
            } 
            else if (choix == 3 && stockCarburant > 0 && joueur->ferraille >= 5) {
                joueur->ferraille -= 5;
                joueur->carburant += 1;
                stockCarburant--;
                printf(COLOR_GREEN "✔ Carburant ajouté.\n" COLOR_RESET);
            }
            else if (choix != 4) printf(COLOR_RED "✘ Action impossible (Fond/Stock/Max).\n" COLOR_RESET);
        } 
        else if (categorie == 2) {
            // --- SOUS-MENU UPGRADES ---
            int choix = 0;
            printf("\n" COLOR_RED "─── AMÉLIORATIONS ───" COLOR_RESET "\n");
            if (aVenduUpgrade) printf(COLOR_YELLOW "!! Une seule modification structurelle par escale !!\n" COLOR_RESET);
            
            printf("1. Laser +1 (Actuel: %-2d) | 40 Fer.\n", joueur->armes);
            printf("2. Shield +1 (Max: %-2d)   | 50 Fer.\n", joueur->bouclierMax);
            printf("3. Moteurs +1 (Esquive)    | 30 Fer.\n");
            printf("4. Retour\n");
            printf(COLOR_YELLOW "> " COLOR_RESET);
            scanf("%d", &choix);

            if (!aVenduUpgrade) {
                if (choix == 1 && joueur->ferraille >= 40) {
                    joueur->ferraille -= 40; joueur->armes++; aVenduUpgrade = 1;
                    printf(COLOR_GREEN "✔ Canons laser calibrés.\n" COLOR_RESET);
                } else if (choix == 2 && joueur->ferraille >= 50) {
                    joueur->ferraille -= 50; joueur->bouclierMax++; aVenduUpgrade = 1;
                    printf(COLOR_GREEN "✔ Nouveau noyau de bouclier installé.\n" COLOR_RESET);
                } else if (choix == 3 && joueur->ferraille >= 30) {
                    joueur->ferraille -= 30; joueur->moteurs++; aVenduUpgrade = 1;
                    printf(COLOR_GREEN "✔ Turbines optimisées.\n" COLOR_RESET);
                }
            } else if (choix != 4) printf(COLOR_RED "✘ Nos techniciens sont déjà occupés.\n" COLOR_RESET);
        }
        else if (categorie == 3) {
            // --- SOUS-MENU SERVICES ---
            int choix = 0;
            printf("\n" COLOR_YELLOW "─── SERVICES DU MARCHÉ NOIR ───" COLOR_RESET "\n");
            printf("1. Vendre du Carburant (+4 Ferraille / Unité)\n");
            printf("2. Retour\n");
            printf(COLOR_YELLOW "> " COLOR_RESET);
            scanf("%d", &choix);

            if (choix == 1) {
                if (joueur->carburant > 0) {
                    joueur->carburant--;
                    joueur->ferraille += 4;
                    printf(COLOR_GREEN "✔ Carburant vendu. +4 Ferraille.\n" COLOR_RESET);
                } else printf(COLOR_RED "✘ Vous n'avez plus de carburant !\n" COLOR_RESET);
            }
        }
        
        if (categorie == 4) sauvegarderPartie(joueur);
        if (categorie != 4) SLEEP_MS(1200);
    }
}
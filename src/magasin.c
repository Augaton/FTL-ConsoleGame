#include "magasin.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void ouvrirMagasin(Vaisseau *joueur) {
    int categorie = 0;
    int stockMissiles = (rand() % 4) + 2;
    int stockCarburant = (rand() % 6) + 5;
    int aVenduUpgrade = 0;

    while (categorie != 4) {
        effacerEcran();
        
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
            // --- MAINTENANCE (Inchangé sauf les vérifications) ---
            int choix = 0;
            printf("\n" COLOR_CYAN "─── MAINTENANCE ───" COLOR_RESET "\n");
            printf("1. Réparer Coque (+5)  | 10 Fer. | (Besoin: %d)\n", joueur->coqueMax - joueur->coque);
            printf("2. Missiles (+3)       | 15 Fer. | Stock: %d\n", stockMissiles);
            printf("3. Carburant (x1)      | 05 Fer. | Stock: %d\n", stockCarburant);
            printf("4. Retour\n > ");
            scanf("%d", &choix);

            if (choix == 1 && joueur->ferraille >= 10 && joueur->coque < joueur->coqueMax) {
                joueur->ferraille -= 10;
                joueur->coque = (joueur->coque + 5 > joueur->coqueMax) ? joueur->coqueMax : joueur->coque + 5;
                printf(COLOR_GREEN "✔ Coque réparée.\n" COLOR_RESET);
            } 
            else if (choix == 2 && stockMissiles > 0 && joueur->ferraille >= 15) {
                joueur->ferraille -= 15; joueur->missiles += 3; stockMissiles--;
            } 
            else if (choix == 3 && stockCarburant > 0 && joueur->ferraille >= 5) {
                joueur->ferraille -= 5; joueur->carburant += 1; stockCarburant--;
            }
        } 
        else if (categorie == 2) {
            // --- UPGRADES (Version Composants) ---
            int choix = 0;
            printf("\n" COLOR_RED "─── AMÉLIORATIONS DE RANG ───" COLOR_RESET "\n");
            if (aVenduUpgrade) printf(COLOR_YELLOW "!! Une seule modification par escale !!\n" COLOR_RESET);
            
            // On affiche le nom et le rang actuel
            printf("1. Upgrade %-15s (Mk %d) | 40 Fer.\n", joueur->systemeArme.nom, joueur->systemeArme.rang);
            printf("2. Upgrade %-15s (Mk %d) | 50 Fer.\n", joueur->systemeBouclier.nom, joueur->systemeBouclier.rang);
            printf("3. Moteurs (Esquive) (Lvl %-2d)         | 30 Fer.\n", joueur->moteurs);
            printf("4. Retour\n > ");
            scanf("%d", &choix);

            if (!aVenduUpgrade) {
                if (choix == 1 && joueur->ferraille >= 40) {
                    joueur->ferraille -= 40;
                    ameliorerArme(joueur); // Utilise ta fonction annexe
                    aVenduUpgrade = 1;
                } else if (choix == 2 && joueur->ferraille >= 50) {
                    joueur->ferraille -= 50;
                    ameliorerBouclier(joueur); // À créer ci-dessous
                    aVenduUpgrade = 1;
                } else if (choix == 3 && joueur->ferraille >= 30) {
                    joueur->ferraille -= 30;
                    joueur->moteurs++;
                    aVenduUpgrade = 1;
                    printf(COLOR_GREEN "✔ Moteurs optimisés.\n" COLOR_RESET);
                }
            }
        }
        else if (categorie == 3) {
            // --- SERVICES ---
            int choix = 0;
            printf("\n" COLOR_YELLOW "─── SERVICES DU MARCHÉ NOIR ───" COLOR_RESET "\n");
            printf("1. Vendre 1 Carburant (+4 Ferraille)\n");
            printf("2. Recycler Arme Actuelle (Gain : Rang * 10)\n");
            printf("3. Retour\n > ");
            scanf("%d", &choix);

            if (choix == 1 && joueur->carburant > 0) {
                joueur->carburant--; joueur->ferraille += 4;
            } else if (choix == 2) {
                int gain = joueur->systemeArme.rang * 10;
                joueur->ferraille += gain;
                // On remet une arme de base Mk 0
                strcpy(joueur->systemeArme.nom, "Laser Rouillé");
                joueur->systemeArme.rang = 0;
                joueur->systemeArme.efficacite = 1;
                printf(COLOR_GREEN "✔ Arme recyclée pour %d Ferraille.\n" COLOR_RESET, gain);
            }
        }
        
        if (categorie == 4) sauvegarderPartie(joueur);
        if (categorie != 4) SLEEP_MS(1200);
    }
}
// FONCTION ANNEXE

void ameliorerArme(Vaisseau *v) {
    v->systemeArme.rang++;
    v->systemeArme.efficacite += 2; 
    // On met à jour le nom dynamiquement
    snprintf(v->systemeArme.nom, 50, "Laser Pulse Mk %d", v->systemeArme.rang);
    printf(COLOR_GREEN "✔ Votre arme est maintenant : %s !\n" COLOR_RESET, v->systemeArme.nom);
}

void ameliorerBouclier(Vaisseau *v) {
    v->systemeBouclier.rang++;
    v->systemeBouclier.efficacite += 1; // +1 point de bouclier max
    v->bouclierActuel = v->systemeBouclier.efficacite; // On recharge à l'achat
    snprintf(v->systemeBouclier.nom, 50, "Bouclier Ion Mk %d", v->systemeBouclier.rang);
    printf(COLOR_GREEN "✔ Nouveau système installé : %s !\n" COLOR_RESET, v->systemeBouclier.nom);
}
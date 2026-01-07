#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "utils.h"
#include "vaisseau.h"
#include "combat.h"
#include "event.h"

// --- MAIN ---
int main() {
    srand(time(NULL));
    Vaisseau joueur;
    int choixMenu;

    printf(COLOR_CYAN "╔════════════════════════════════════════╗\n");
    printf("║           🚀 FTL Console 🚀            ║\n");
    printf("╚════════════════════════════════════════╝\n" COLOR_RESET);

    // --- LOGIQUE DE CHARGEMENT ---
    if (chargerPartie(&joueur)) {
        printf(COLOR_GREEN "\n[INFO] Une sauvegarde a été trouvée (Secteur %d).\n" COLOR_RESET, joueur.distanceParcourue);
        printf(COLOR_BLUE "1. Continuer la mission\n");
        printf("2. Nouvelle partie (Écrase la sauvegarde)" COLOR_RESET);
        printf(COLOR_YELLOW "\n> " COLOR_RESET);
        scanf("%d", &choixMenu);
        while(getchar() != '\n'); // Nettoyer le buffer après scanf

        if (choixMenu == 1) {
            if (strcmp(joueur.secteurActuel, "REPOS") != 0 && strlen(joueur.secteurActuel) > 0) {
                printf(COLOR_YELLOW "\n[REPRISE] Retour au secteur : %s\n" COLOR_RESET, joueur.secteurActuel);
                SLEEP_MS(1000);
                executerEvenement(&joueur, joueur.secteurActuel);
                
                strcpy(joueur.secteurActuel, "REPOS");
                sauvegarderPartie(&joueur);
            }
        }
    } else {
        choixMenu = 2; // Pas de sauvegarde, donc nouvelle partie d'office
    }

    if (choixMenu == 2) {
        // Initialisation d'un nouveau vaisseau
        strcpy(joueur.nom, "");
        joueur.coque = 20;
        joueur.coqueMax = 20;
        joueur.bouclier = 2;
        joueur.bouclierMax = 5;
        joueur.armes = 2;
        joueur.missiles = 3;
        joueur.ferraille = 20;
        joueur.carburant = 10;
        joueur.distanceParcourue = 0;
        joueur.moteurs = 0;

        printf(COLOR_BLUE "\nCommandant, nommez votre vaisseau" COLOR_RESET);
        printf(COLOR_YELLOW "\n> " COLOR_RESET);
        fgets(joueur.nom, sizeof(joueur.nom), stdin);
        joueur.nom[strcspn(joueur.nom, "\n")] = 0;
        
        sauvegarderPartie(&joueur);
    }

    // --- BOUCLE PRINCIPALE DU JEU ---
    while (joueur.coque > 0 && joueur.distanceParcourue < DISTANCE_FINALE) {
        menuVoyage(&joueur); 
    }

    // --- GESTION DE LA FIN ---
    if (joueur.coque <= 0) {
        afficherGameOver(&joueur);
        supprimerSauvegarde();
    } 
    else if (joueur.distanceParcourue >= DISTANCE_FINALE) {
        afficherVictoire(&joueur);
        supprimerSauvegarde();
    }

    printf("\n--- FIN DE LA PARTIE ---\n");
    return 0;
}
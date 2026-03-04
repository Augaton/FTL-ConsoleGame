#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "utils.h"
#include "vaisseau.h"
#include "combat.h"
#include "event.h"

unsigned int genererSeedDepuisTexte(const char *str);

unsigned int genererSeedDepuisTexte(const char *str) {
    unsigned int hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

// --- MAIN ---
int main() {
    srand(time(NULL));
    Vaisseau joueur;
    int choixMenu;

    printf(COLOR_CYAN "╔════════════════════════════════════════╗\n");
    printf("║           🚀 FTL Console 🚀            ║\n");
    printf("║                V 1.0.0                 ║\n");
    printf("╚════════════════════════════════════════╝\n" COLOR_RESET);

    // --- LOGIQUE DE DÉMARRAGE ---
    int chargementReussi = 0;

    if (chargerPartie(&joueur)) {
        printf(COLOR_GREEN "\n[INFO] Sauvegarde trouvée (Secteur %d - %s).\n" COLOR_RESET, 
               joueur.distanceParcourue, joueur.nom);
        printf(COLOR_BLUE "1. Continuer la mission\n");
        printf("2. Nouvelle partie (Écrase la sauvegarde)" COLOR_RESET);
        printf(COLOR_YELLOW "\n> " COLOR_RESET);
        
        scanf("%d", &choixMenu);
        while(getchar() != '\n'); // Nettoyage buffer

        if (choixMenu == 1) {
            chargementReussi = 1;
            // Gestion de la reprise après événement
            if (strcmp(joueur.secteurActuel, "REPOS") != 0 && strlen(joueur.secteurActuel) > 0) {
                printf(COLOR_YELLOW "\n[REPRISE] Retour au secteur : %s\n" COLOR_RESET, joueur.secteurActuel);
                SLEEP_MS(1000);
                executerEvenement(&joueur, joueur.secteurActuel);
                strcpy(joueur.secteurActuel, "REPOS");
                sauvegarderPartie(&joueur);
            }
        }
    }

    // Si pas de sauvegarde ou choix "Nouvelle Partie"
    if (!chargementReussi) {
        // APPEL DE TA NOUVELLE FONCTION PROPRE
        initialiserNouvellePartie(&joueur);
    }

    // --- BOUCLE PRINCIPALE DU JEU ---
    while (joueur.coque > 0 && joueur.distanceParcourue < joueur.distanceObjectif) {
        
        // BOSS FINAL
        if (joueur.distanceParcourue == joueur.distanceObjectif - 1) {
            printf(COLOR_RED "\n[ALERTE] Signature thermique massive détectée...\n" COLOR_RESET);
            Vaisseau boss = genererBossFinal();
            SLEEP_MS(2000);
            lancerCombat(&joueur, &boss); 
            
            if (joueur.coque > 0) joueur.distanceParcourue++; 
        } 
        // SECTEURS NORMAUX
        else {
            menuVoyage(&joueur);
        }
    }

    // --- ÉCRANS DE FIN ---
    if (joueur.coque <= 0) {
        afficherGameOver(&joueur);
        supprimerSauvegarde();
    } 
    else if (joueur.distanceParcourue >= joueur.distanceObjectif) {
        afficherVictoire(&joueur);
        supprimerSauvegarde();
    }

    printf("\n--- FIN DE LA TRANSMISSION ---\n");
    return 0;
}


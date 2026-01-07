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
        joueur.missiles = 3;
        joueur.ferraille = 20;
        joueur.carburant = 10;
        joueur.distanceParcourue = 0;
        joueur.moteurs = 0;

        // IMPORTANT
        joueur.ennemiPresent = 0;       
        joueur.ennemiCoqueActuelle = 0;

        joueur.chargeFTL = 0;
        joueur.maxchargeFTL = 3;

        // Initialisation arme
        strcpy(joueur.systemeArme.nom, "Laser Burst");
        joueur.systemeArme.rang = 1;
        joueur.systemeArme.efficacite = 2;

        // Initialisation bouclier
        strcpy(joueur.systemeBouclier.nom, "Generateur Ionique");
        joueur.systemeBouclier.rang = 1;
        joueur.systemeBouclier.efficacite = 2; 
        joueur.bouclierActuel = 2;

        printf(COLOR_BLUE "\nCommandant, nommez votre vaisseau" COLOR_RESET);
        printf(COLOR_YELLOW "\n> " COLOR_RESET);
        fgets(joueur.nom, sizeof(joueur.nom), stdin);
        joueur.nom[strcspn(joueur.nom, "\n")] = 0;


        printf("\n" COLOR_CYAN "--- CONFIGURATION DE LA MISSION ---" COLOR_RESET "\n");
        printf("1. Raid rapide (10 secteurs)\n");
        printf("2. Mission standard (20 secteurs)\n");
        printf("3. Longue expédition (40 secteurs)\n");
        printf("4. Personnalisé\n");
        printf(COLOR_YELLOW "> " COLOR_RESET);
        
        int mode;
        scanf("%d", &mode);
        
        if (mode == 1) joueur.distanceObjectif = 10;
        else if (mode == 2) joueur.distanceObjectif = 20;
        else if (mode == 3) joueur.distanceObjectif = 40;
        else {
            printf("Entrez le nombre de secteurs : ");
            scanf("%d", &joueur.distanceObjectif);
        }

        printf("\n" COLOR_CYAN "--- CONFIGURATION DE LA SEED ---" COLOR_RESET "\n");
        printf("1. Random\n");
        printf("2. Personalisée\n");
        printf(COLOR_YELLOW "> " COLOR_RESET);
        
        int seedMode;
        scanf("%d", &seedMode);
        
        if (seedMode == 1) {
            joueur.seedSecteur = genererSeedDepuisTexte(joueur.nom) ^ (unsigned int)time(NULL);
        } else {
            printf("Entrez votre seed personnalisée\n");
            printf(COLOR_YELLOW "> " COLOR_RESET);

            unsigned int seedInput;
            scanf("%u", &seedInput);
            joueur.seedSecteur = seedInput;
        }

        printf(COLOR_GREEN "Seed definie : %u\n" COLOR_RESET, joueur.seedSecteur);
        SLEEP_MS(1000);

        sauvegarderPartie(&joueur);
    }

    // --- BOUCLE PRINCIPALE DU JEU ---
    while (joueur.coque > 0 && joueur.distanceParcourue < joueur.distanceObjectif) {
        
        // Cas du DERNIER secteur : Le Boss
        if (joueur.distanceParcourue == joueur.distanceObjectif - 1) {
            printf(COLOR_RED "\n[ALERTE] Signature thermique massive détectée...\n" COLOR_RESET);

            Vaisseau boss = genererBossFinal();
            SLEEP_MS(2000);
            lancerCombat(&joueur, &boss); 
            
            if (joueur.coque > 0) {
                joueur.distanceParcourue++; 
            }
        } 
        // Secteurs normaux
        else {
            menuVoyage(&joueur);
        }
    }

    // --- GESTION DE LA FIN ---
    if (joueur.coque <= 0) {
        afficherGameOver(&joueur);
        supprimerSauvegarde();
    } 
    else if (joueur.distanceParcourue >= joueur.distanceObjectif) {
        afficherVictoire(&joueur);
        supprimerSauvegarde();
    }

    printf("\n--- FIN DE LA PARTIE ---\n");
    return 0;
}


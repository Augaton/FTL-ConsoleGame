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
        printf("\n[INFO] Une sauvegarde a été trouvée (Secteur %d).\n", joueur.distanceParcourue);
        printf("1. Continuer la mission\n");
        printf("2. Nouvelle partie (Écrase la sauvegarde)\n");
        printf("Choix : ");
        scanf("%d", &choixMenu);
        while(getchar() != '\n'); // Nettoyer le buffer après scanf
    } else {
        choixMenu = 2; // Pas de sauvegarde, donc nouvelle partie d'office
    }

    if (choixMenu == 2) {
        // Initialisation d'un nouveau vaisseau
        strcpy(joueur.nom, "");
        joueur.coque = 20;
        joueur.coqueMax = 20;
        joueur.bouclier = 1;
        joueur.bouclierMax = 1;
        joueur.armes = 2;
        joueur.missiles = 3;
        joueur.ferraille = 20;
        joueur.carburant = 10;
        joueur.distanceParcourue = 0;
        joueur.moteurs = 0;

        printf("\nCommandant, nommez votre vaisseau : ");
        fgets(joueur.nom, sizeof(joueur.nom), stdin);
        joueur.nom[strcspn(joueur.nom, "\n")] = 0;
        
        sauvegarderPartie(&joueur); // Créer le fichier initial
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
    else if (joueur.distanceParcourue >= 20) {
        afficherVictoire(&joueur);
        supprimerSauvegarde();
    }

    printf("\n--- FIN DE LA PARTIE ---\n");
    return 0;
}
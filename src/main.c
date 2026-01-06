#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "utils.h"
#include "vaisseau.h"
#include "combat.h"
#include "event.h"

#define DISTANCE_FINALE 20

// --- MAIN ---
int main() {
    srand(time(NULL));

    Vaisseau joueur = {
        "",  // nom
        20,  // coque
        20,  // coqueMax
        1,   // bouclier (actuel)
        1,   // bouclierMax
        2,   // armes
        3,   // missiles
        20,  // ferraille
        10,  // carburant
        0    // distanceParcourue
    };

    printf("--- INITIALISATION DU SYSTEME ---\n");
    printf("Commandant, comment se nomme votre vaisseau ? : ");

    getchar();
    fgets(joueur.nom, sizeof(joueur.nom), stdin);
    joueur.nom[strcspn(joueur.nom, "\n")] = 0;

    while (joueur.coque > 0) {
        if (joueur.distanceParcourue >= DISTANCE_FINALE) {
            printf("\nVous avez atteint le coeur du secteur ennemi...\n");
            SLEEP_MS(2000);
            
            // Combat final
            Vaisseau boss = genererBossFinal();
            SLEEP_MS(3000);
            
            while (joueur.coque > 0 && boss.coque > 0) {
                tourCombat(&joueur, &boss);
            }
            
            if (joueur.coque > 0) {
                effacerEcran();
                printf("\n******************************************\n");
                printf("* VICTOIRE ! LE FLAGSHIP EST DETRUIT   *\n");
                printf("* VOUS AVEZ SAUVE LA FEDERATION     *\n");
                printf("******************************************\n");
            }
            break; // Fin du jeu
        }
        
        // Sinon, on continue le voyage normalement
        menuVoyage(&joueur); 
    }

    printf("\n--- FIN DE LA PARTIE ---\n");
    return 0;
}
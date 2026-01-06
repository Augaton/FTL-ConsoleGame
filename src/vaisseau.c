#include <stdio.h>
#include "vaisseau.h"
#include "utils.h"

void afficherVaisseau(Vaisseau *v) {
    effacerEcran();
    printf(COLOR_CYAN "╔══════════════════════════════════════════════════════════╗\n");
    printf("║          RAPPORT TECHNIQUE DU VAISSEAU : %-15s ║\n", v->nom);
    printf("╚══════════════════════════════════════════════════════════╝" COLOR_RESET "\n\n");

    printf(COLOR_BOLD "--- SYSTEMES OFFENSIFS ---" COLOR_RESET "\n");
    printf(" [LASERS]  Niveau %d (Dégâts : %d)\n", v->armes - 1, v->armes);
    printf(" [MISSILES] %d ogives en réserve\n\n", v->missiles);

    printf(COLOR_BOLD "--- SYSTEMES DE DEFENSE ---" COLOR_RESET "\n");
    printf(" [BOUCLIER] Capacité Max : %d\n", v->bouclierMax);
    printf(" [MOTEURS]  Niveau %d (Esquive : %d%%)\n\n", v->moteurs, 10 + (v->moteurs * 5));

    printf(COLOR_BOLD "--- LOGISTIQUE ---" COLOR_RESET "\n");
    printf(" [FERRAILLE] %d unités (monnaie)\n", v->ferraille);
    printf(" [CARBURANT] %d cellules de saut\n", v->carburant);
    printf(" [SECTEUR]   %d sur 20 parcourus\n", v->distanceParcourue);

    printf("\n" COLOR_YELLOW "==========================================================\n");
    printf("       [ Appuyez sur ENTREE pour quitter le journal ]\n");
    printf("==========================================================" COLOR_RESET "\n");
    

    getchar(); 
}
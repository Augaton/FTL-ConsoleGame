#include <stdio.h>
#include "vaisseau.h"
#include "utils.h"

void afficherVaisseau(Vaisseau *v) {
    effacerEcran();
    // En-tête stylisée
    printf(COLOR_CYAN "╔══════════════════════════════════════════════════════════╗\n");
    printf("║ " COLOR_BOLD "📊 LOGS TECHNIQUES" COLOR_RESET COLOR_CYAN " : %-35s ║\n", v->nom);
    printf("╠══════════════════════════════════════════════════════════╣" COLOR_RESET "\n\n");

    // --- BLOC OFFENSIF ---
    printf(COLOR_RED "  [ OFFENSE ]" COLOR_RESET "\n");
    printf("  ├─ " COLOR_BOLD "LASERS   " COLOR_RESET ": Niv.%-2d ─> 💥 Dégâts: %-2d | 🎯 Crit: %d%%\n", 
            v->armes - 1, v->armes, 10 + (v->moteurs * 2));
    printf("  └─ " COLOR_BOLD "MISSILES " COLOR_RESET ": %02d unités disponibles\n\n", v->missiles);

    // --- BLOC DÉFENSIF ---
    printf(COLOR_CYAN "  [ DÉFENSE ]" COLOR_RESET "\n");
    printf("  ├─ " COLOR_BOLD "BOUCLIER " COLOR_RESET ": Capacité max de %d unités\n", v->bouclierMax);
    printf("  └─ " COLOR_BOLD "MOTEURS  " COLOR_RESET ": Niv.%-2d ─> 🛡️  Esquive: %d%%\n\n", 
            v->moteurs, 10 + (v->moteurs * 5));

    // --- BLOC LOGISTIQUE ---
    printf(COLOR_YELLOW "  [ LOGISTIQUE ]" COLOR_RESET "\n");
    printf("  ├─ " COLOR_BOLD "FERRAILLE" COLOR_RESET ": %-5d ⚓ (Crédits actuels)\n", v->ferraille);
    printf("  ├─ " COLOR_BOLD "CARBURANT" COLOR_RESET ": %-5d ⚡ (Unités de saut)\n", v->carburant);
    printf("  └─ " COLOR_BOLD "DISTANCE " COLOR_RESET ": Sector %d/%d 🚩\n", v->distanceParcourue, v->distanceObjectif);

    // Pied de page interactif
    printf("\n" COLOR_CYAN "╚══════════════════════════════════════════════════════════╝" COLOR_RESET "\n");
    printf(COLOR_BOLD "         [ Appuyez sur ENTREE pour fermer ]" COLOR_RESET);
    
    // Nettoyage et attente
    int c; while ((c = getchar()) != '\n' && c != EOF); 
}
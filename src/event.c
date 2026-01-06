#include "event.h"
#include "combat.h"
#include "utils.h"
#include "magasin.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_RESET   "\x1b[0m"
#define COLOR_BOLD    "\x1b[1m"

void menuVoyage(Vaisseau *joueur) {
    int continuerMenu = 1; // Variable de contrôle

    while (continuerMenu && joueur->coque > 0) {
        int choix = 0;
        effacerEcran();
        
printf(COLOR_CYAN "╔══════════════════════════════════════════════════════════╗\n");
        printf("║  COMMANDANT: %-15s | SECTEUR: %02d/20     ║\n", joueur->nom, joueur->distanceParcourue);
        printf("╚══════════════════════════════════════════════════════════╝" COLOR_RESET "\n\n");

        // Barre de Coque avec dégradé de couleur
        printf(" STATUS COQUE   : ");
        int barres = (joueur->coque * 20) / joueur->coqueMax;
        
        // Couleur dynamique selon la vie
        if (barres > 10) printf(COLOR_GREEN);
        else if (barres > 5) printf(COLOR_YELLOW);
        else printf(COLOR_RED);

        for(int i=0; i<20; i++) {
            if(i < barres) printf("█"); // Caractère plein
            else printf("░");           // Caractère ombré
        }
        printf(" %d/%d" COLOR_RESET "\n", joueur->coque, joueur->coqueMax);

        // Boucliers stylisés (Hexagones ou Cercles)
        printf(" SYSTEME SHIELD : ");
        for(int i=0; i < joueur->bouclierMax; i++) {
            if(i < joueur->bouclier) printf(COLOR_CYAN "⬢ " COLOR_RESET); // Hexagone plein
            else printf(COLOR_RED "⬡ " COLOR_RESET);                     // Hexagone vide
        }
        
        printf("\n\n" COLOR_BOLD "--- INVENTAIRE ---" COLOR_RESET "\n");
        printf(" [⚡] Carburant : %-2d | [⚓] Ferraille : %-3d | [🚀] Missiles : %-2d\n", 
                joueur->carburant, joueur->ferraille, joueur->missiles);


        printf("\n\n" COLOR_BOLD "--- AVENTURE ---" COLOR_RESET "\n");
        printf("\n PROGRESSION ");
        for(int i=1; i<=20; i++) {
            if(i < joueur->distanceParcourue) printf(COLOR_GREEN "=" COLOR_RESET);
            else if(i == joueur->distanceParcourue) printf(COLOR_BOLD ">" COLOR_RESET);
            else printf(COLOR_CYAN "·" COLOR_RESET);
        }
        printf(" [FINAL]\n");

        printf(COLOR_CYAN "--------------------------------------\n\n" COLOR_RESET);
        printf(COLOR_BOLD "1. Sauter vers le prochain secteur (-1 Carburant)\n");
        printf("2. Consulter le journal de bord (Stats)\n");
        printf("3. Quitter le jeu\n" COLOR_RESET);
        printf("\nVotre choix : ");
        if (scanf("%d", &choix) != 1) {
            int c; while ((c = getchar()) != '\n' && c != EOF);
            printf(COLOR_RED "\n[ERREUR] Saisie invalide !" COLOR_RESET);
            SLEEP_MS(1000);
            continue;
        }

        // Nettoyage du buffer après une saisie réussie pour éviter les sauts de ligne fantômes
        int c; while ((c = getchar()) != '\n' && c != EOF);

        if (choix == 1) {
            const char* baliseA = inspecterBalise();
            const char* baliseB = inspecterBalise();
            int choixSaut;

            printf("\n--- CARTE STELLAIRE ---\n");
            printf("1. %s\n2. %s\nChoix : ", baliseA, baliseB);
            scanf("%d", &choixSaut);

            const char* destination = (choixSaut == 1) ? baliseA : baliseB;

            if (strcmp(destination, "Nebuleuse (Inconnu - Gratuit)") != 0) {
                if (joueur->carburant > 0) joueur->carburant--;
                else {
                    printf(COLOR_RED "\n[DERIVE] Pas de carburant ! Coque -5.\n" COLOR_RESET);
                    joueur->coque -= 5;
                }
            }

            joueur->distanceParcourue++;
            executerEvenement(joueur, destination);

            if (joueur->coque > 0) {
                printf("\n[Appuyez sur Entree pour stabiliser le vaisseau]");
                int c; while ((c = getchar()) != '\n' && c != EOF); getchar();
            }
        }
        else if (choix == 2) {
            afficherVaisseau(joueur);
        }
        else if (choix == 3) {
            printf("Fermeture des systemes...\n");
            joueur->coque = 0;
            continuerMenu = 0;
        }
    }
}

const char* inspecterBalise() {
    int r = rand() % 100;
    if (r < 10) return "Nebuleuse (Inconnu - Gratuit)";
    if (r < 40) return "Signal Hostile (Combat)";
    if (r < 55) return "Station Commerciale (Magasin)";
    if (r < 80) return "Signal de Detresse";
    return "Secteur Vide";
}

void executerEvenement(Vaisseau *joueur, const char* type) {
    const char* evenementFinal = type;

    if (strcmp(type, "Nebuleuse (Inconnu - Gratuit)") == 0) {
        printf("\n[SYSTEME] Entree dans la nebuleuse. Capteurs brouilles...\n");
        int r = rand() % 100;
        if (r < 50) evenementFinal = "Signal Hostile (Combat)";
        else if (r < 80) evenementFinal = "Signal de Detresse";
        else evenementFinal = "Secteur Vide";
    }

    if (strcmp(evenementFinal, "Signal Hostile (Combat)") == 0) {
        lancerCombat(joueur);
    } else if (strcmp(evenementFinal, "Station Commerciale (Magasin)") == 0) {
        ouvrirMagasin(joueur);
    } else if (strcmp(evenementFinal, "Signal de Detresse") == 0) {
        evenementDeresse(joueur);
    } else {
        printf("\n[INFO] La zone est deserte.\n");
    }
}
void evenementDeresse(Vaisseau *joueur) {
    int choix;
    printf("\n[SIGNAL DE DETRESSE] Un transporteur civil est en panne.\n");
    printf("1. Aider (70%% chance de succes)\n");
    printf("2. Ignorer\n");
    printf("Votre decision : ");
    scanf("%d", &choix);

    if (choix == 1) {
        if ((rand() % 100) < 70) {
            joueur->ferraille += 20;
            printf("Succes ! +20 Ferraille.\n");
        } else {
            joueur->coque -= 3;
            printf("Piege ! L'explosion vous inflige 3 degats.\n");
        }
    } else {
        printf("Vous passez votre chemin.\n");
    }
}
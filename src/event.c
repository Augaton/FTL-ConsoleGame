#include "event.h"
#include "combat.h"
#include "utils.h"
#include "magasin.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void menuVoyage(Vaisseau *joueur) {
    int continuerMenu = 1;

    while (continuerMenu && joueur->coque > 0) {
        int choix = 0;
        effacerEcran();
        
        // --- HEADER COCKPIT ---
        printf(COLOR_CYAN "╔══════════════════════════════════════════════════════════╗\n");
        printf("║ " COLOR_BOLD "%-18s" COLOR_RESET COLOR_CYAN "CONSOLE DE NAVIGATION   SECTEUR: %02d/20 ║\n", joueur->nom, joueur->distanceParcourue);
        printf("╠══════════════════════════════════════════════════════════╣" COLOR_RESET "\n");

        // --- SECTION ÉTAT (Coque & Shield sur la même ligne pour gagner de la place) ---
        int barres = (joueur->coque * 20) / joueur->coqueMax;
        printf(COLOR_CYAN "║ " COLOR_RESET "COQUE: ");
        if (barres > 10) printf(COLOR_GREEN); else if (barres > 5) printf(COLOR_YELLOW); else printf(COLOR_RED);
        for(int i=0; i<20; i++) printf(i < barres ? "█" : "░");
        printf(COLOR_RESET " %02d/%02d  ", joueur->coque, joueur->coqueMax);
        
        printf(COLOR_CYAN "SHIELD: " COLOR_RESET);
        for(int i=0; i < joueur->bouclierMax; i++) {
            printf(i < joueur->bouclier ? COLOR_CYAN "⬢ " : COLOR_RED "⬡ ");
        }
        // Petit ajustement d'espaces pour fermer le cadre selon le nombre de boucliers
        int espacesRestants = 14 - (joueur->bouclierMax * 2);
        for(int i=0; i<espacesRestants; i++) printf(" ");
        printf(COLOR_CYAN "║\n");

        // --- SECTION INVENTAIRE (Style Grille) ---
        printf("╠══════════════════════════════════════════════════════════╣\n");
        printf("║ " COLOR_YELLOW "⚡ " COLOR_RESET "CARBURANT: %-3d  " COLOR_YELLOW "⚓ " COLOR_RESET "FERRAILLE: %-4d  " COLOR_YELLOW "🚀 " COLOR_RESET "MISSILES: %-3d " COLOR_CYAN " ║\n", 
                joueur->carburant, joueur->ferraille, joueur->missiles);

        // --- SECTION PROGRESSION ---
        printf("╠══════════════════════════════════════════════════════════╣\n");
        printf("║ PROGRESSION: ");
        for(int i=1; i<=20; i++) {
            if(i < joueur->distanceParcourue) printf(COLOR_GREEN "═");
            else if(i == joueur->distanceParcourue) printf(COLOR_BOLD COLOR_YELLOW "✈");
            else printf(COLOR_RESET "·");
        }
        printf(COLOR_CYAN " [DESTINATION]          ║\n");
        printf("╚══════════════════════════════════════════════════════════╝" COLOR_RESET "\n\n");

        // --- MENU ACTIONS ---
        printf(COLOR_BOLD "  [1]" COLOR_RESET " ENGAGER LE SAUT SPATIAL\n");
        printf(COLOR_BOLD "  [2]" COLOR_RESET " CONSULTER LES LOGS TECHNIQUES\n");
        printf(COLOR_BOLD "  [3]" COLOR_RESET " ABANDONNER LA MISSION\n");
        printf("\n" COLOR_CYAN " COMMANDE > " COLOR_RESET);

        if (scanf("%d", &choix) != 1) {
            int c; while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }
        int c; while ((c = getchar()) != '\n' && c != EOF); // Nettoyage systématique

        if (choix == 1) {
             lancerSequenceDeSaut(joueur); 
        }
        else if (choix == 2) {
            afficherVaisseau(joueur);
        }
        else if (choix == 3) {

            printf(COLOR_CYAN "Fermeture des systemes...\n" COLOR_RESET);
            joueur->coque = 0;
            continuerMenu = 0;

        } 
    }
}

void lancerSequenceDeSaut(Vaisseau *joueur) {
    const char* baliseA = inspecterBalise();
    const char* baliseB = inspecterBalise();
    int choixSaut;

    printf("\n" COLOR_YELLOW "─── CALCUL DES TRAJECTOIRES FTL ───" COLOR_RESET "\n");
    printf(" A- %-30s\n", baliseA);
    printf(" B- %-30s\n", baliseB);
    printf("\n Destination (1 ou 2) : ");
    scanf("%d", &choixSaut);

    const char* destination = (choixSaut == 1) ? baliseA : baliseB;

    // Logique carburant...
    if (strcmp(destination, "Nebuleuse (Inconnu - Gratuit)") != 0) {
        if (joueur->carburant > 0) joueur->carburant--;
        else {
            printf(COLOR_RED "\n[ALERTE] Panne de carburant ! Dérive critique...\n" COLOR_RESET);
            joueur->coque -= 5;
            SLEEP_MS(1500);
        }
    }

    printf(COLOR_CYAN "\nSaut FTL en cours" COLOR_RESET);
    for(int i=0; i<3; i++) { printf(COLOR_CYAN "." COLOR_RESET); fflush(stdout); SLEEP_MS(300); }
    
    joueur->distanceParcourue++;
    executerEvenement(joueur, destination);
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
        lancerEvenementAleatoire(joueur);
    } else {
        printf("\n[INFO] La zone est deserte.\n");
    }
}

void lancerEvenementAleatoire(Vaisseau *joueur) {
    int typeEv = rand() % 6; // On définit 6 types d'événements

    switch(typeEv) {
        case 0: evenementMarchandAmbulant(joueur); break;
        case 1: evenementEpaveDerivante(joueur); break;
        case 2: evenementPluieAsteroides(joueur); break;
        case 3: evenementCapsuleSurvie(joueur); break;
        case 4: evenementAnomalieSpatiale(joueur); break;
        case 5: evenementDeresse(joueur); break;
    }
}

// LISTE DES ÉVÉNEMENTS

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

void evenementEpaveDerivante(Vaisseau *joueur) {
    int choix;
    printf("\n" COLOR_CYAN "[DECOUVERTE]" COLOR_RESET " Une carcasse de croiseur flotte ici.\n");
    printf("1. Envoyer une équipe de récupération (Risqué)\n");
    printf("2. Scanner à distance (Sûr mais faible gain)\n");
    printf("Choix : ");
    scanf("%d", &choix);

    if (choix == 1) {
        int r = rand() % 100;
        if (r < 60) {
            int gain = 30 + (rand() % 20);
            printf(COLOR_GREEN "Succès ! Vos hommes ramènent %d Ferraille." COLOR_RESET "\n", gain);
            joueur->ferraille += gain;
        } else {
            printf(COLOR_RED "Désastre ! Une fuite de plasma blesse l'équipage. Coque -4." COLOR_RESET "\n");
            joueur->coque -= 4;
        }
    } else {
        printf("Le scan révèle quelques débris : +5 Ferraille.\n");
        joueur->ferraille += 5;
    }
}

void evenementPluieAsteroides(Vaisseau *joueur) {
    printf("\n" COLOR_YELLOW "[ALERTE]" COLOR_RESET " Vous traversez un champ d'astéroïdes instable !\n");
    
    int chanceEsquive = 40 + (joueur->moteurs * 10); // Plus de moteurs = plus de survie
    if ((rand() % 100) < chanceEsquive) {
        printf(COLOR_GREEN "Manoeuvre parfaite ! Vous slalomez entre les rochers." COLOR_RESET "\n");
    } else {
        printf(COLOR_RED "CHOC ! Un astéroïde percute le flanc gauche. Coque -3." COLOR_RESET "\n");
        joueur->coque -= 3;
    }
}

void evenementAnomalieSpatiale(Vaisseau *joueur) {
    printf("\n" COLOR_BOLD COLOR_CYAN "[ANOMALIE]" COLOR_RESET " Le vaisseau est aspiré dans une faille chromatique...\n");
    printf("La réalité semble se distordre autour de vous.\n");
    for(int i=0; i<3; i++) { printf("."); fflush(stdout); SLEEP_MS(600); }
    printf("\n");

    int r = rand() % 100;

    if (r < 25) { 
        // EFFET 1 : Surcharge positive (Ton code original)
        joueur->bouclierMax += 1;
        joueur->bouclier = joueur->bouclierMax;
        printf(COLOR_YELLOW "✨ SURCHARGE : Les molécules de la coque se densifient. Bouclier Max +1 !" COLOR_RESET "\n");
    } 
    else if (r < 50) {
        // EFFET 2 : Saut Temporel (Recul ou Avance)
        int saut = (rand() % 2 == 0) ? 1 : -1;
        joueur->distanceParcourue += saut;
        if (joueur->distanceParcourue < 0) joueur->distanceParcourue = 0;
        
        if (saut > 0)
            printf(COLOR_GREEN "⏩ SAUT TEMPOREL : La faille vous a projeté vers l'avant ! (Secteur +1)" COLOR_RESET "\n");
        else
            printf(COLOR_RED "⏪ DISTORSION : Le temps s'inverse... Vous avez reculé ! (Secteur -1)" COLOR_RESET "\n");
    }
    else if (r < 75) {
        // EFFET 3 : Mutation des Ressources
        printf(COLOR_CYAN "💎 ALCHIMIE : La ferraille en soute vibre et se transforme..." COLOR_RESET "\n");
        int gain = (rand() % 15) + 5;
        joueur->ferraille += gain;
        joueur->carburant += 1;
        printf("Vous trouvez %d Ferraille et 1 Carburant matérialisés dans les couloirs.\n", gain);
    }
    else {
        // EFFET 4 : Choc de vide (Négatif)
        printf(COLOR_RED "⚠️  REJET : La faille se referme violemment sur votre moteur FTL !" COLOR_RESET "\n");
        joueur->coque -= 4;
        joueur->carburant = (joueur->carburant > 0) ? joueur->carburant - 1 : 0;
        printf("Dégâts structurels subis (-4 Coque) et perte d'une unité de carburant.\n");
    }
}

void evenementCapsuleSurvie(Vaisseau *joueur) {
    printf("\n" COLOR_CYAN "[SIGNAL]" COLOR_RESET " Une capsule de survie dérive. Elle semble dater de la dernière guerre.\n");
    printf("1. L'ouvrir avec précaution\n");
    printf("2. La recycler à distance (Sûr mais peu rentable)\n");
    printf("Choix : ");
    int choix;
    scanf("%d", &choix);

    if (choix == 1) {
        int r = rand() % 100;
        if (r < 40) {
            printf(COLOR_GREEN "✨ MIRACLE : Un ingénieur était à l'intérieur ! Il répare vos systèmes. (+5 Coque)" COLOR_RESET "\n");
            joueur->coque += 5;
            if (joueur->coque > joueur->coqueMax) joueur->coque = joueur->coqueMax;
        } else if (r < 80) {
            int ferraille = 15 + (rand() % 15);
            printf(COLOR_YELLOW "BUTIN : La capsule contenait des fournitures militaires. (+%d Ferraille)" COLOR_RESET "\n", ferraille);
            joueur->ferraille += ferraille;
        } else {
            printf(COLOR_RED "PIÈGE : La capsule contenait un drone de sécurité fou ! Il s'auto-détruit. (-4 Coque)" COLOR_RESET "\n");
            joueur->coque -= 4;
        }
    } else {
        printf("Vous broyez la capsule : +5 Ferraille.\n");
        joueur->ferraille += 5;
    }
}

void evenementMarchandAmbulant(Vaisseau *joueur) {
    printf("\n" COLOR_YELLOW "[COMMERCE]" COLOR_RESET " Un marchand Jawa vous hèle sur les ondes.\n");
    printf("\"Besoin de quelque chose, étranger ?\"\n");
    printf("1. Acheter 2 Carburant (10 Ferraille)\n");
    printf("2. Acheter 3 Missiles (15 Ferraille)\n");
    printf("3. Ignorer\n");
    printf("Choix : ");
    int choix;
    scanf("%d", &choix);

    if (choix == 1) {
        if (joueur->ferraille >= 10) {
            joueur->ferraille -= 10;
            joueur->carburant += 2;
            printf(COLOR_GREEN "Transaction réussie." COLOR_RESET "\n");
        } else printf(COLOR_RED "Pas assez de ferraille !" COLOR_RESET "\n");
    } 
    else if (choix == 2) {
        if (joueur->ferraille >= 15) {
            joueur->ferraille -= 15;
            joueur->missiles += 3;
            printf(COLOR_GREEN "Transaction réussie." COLOR_RESET "\n");
        } else printf(COLOR_RED "Pas assez de ferraille !" COLOR_RESET "\n");
    }
    else {
        printf("Le marchand s'éloigne en maugréant.\n");
    }
}
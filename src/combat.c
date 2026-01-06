#include "combat.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void afficherEtatCombat(Vaisseau *joueur, Vaisseau *ennemi) {
    effacerEcran();
    
    // On définit des largeurs fixes pour chaque colonne
    // Nom du joueur (15) + vs (4) + Nom ennemi (20) = 39 caractères de contenu
    printf(COLOR_CYAN "╔══════════════════════════════════════════════════╗\n");
    
    // Ligne des Noms
    printf("║ " COLOR_RESET "%-17s " COLOR_YELLOW "vs" COLOR_RESET "  %-26s " COLOR_CYAN "║\n", 
           joueur->nom, ennemi->nom);
    
    // Ligne des Coques
    char coqueJ[20], coqueE[20];
    sprintf(coqueJ, "Coque: %d/%d", joueur->coque, joueur->coqueMax);
    sprintf(coqueE, "Coque: %d/%d", ennemi->coque, ennemi->coqueMax);
    
    printf("║ " COLOR_RED "%-22s " COLOR_RESET "| " COLOR_RED "%-23s " COLOR_CYAN "║\n", 
           coqueJ, coqueE);
    
    // Ligne des Boucliers
    char shieldJ[20], shieldE[20];
    sprintf(shieldJ, "Shield: %d/%d", joueur->bouclier, joueur->bouclierMax);
    sprintf(shieldE, "Shield: %d/%d", ennemi->bouclier, ennemi->bouclierMax);
    
    printf("║ " COLOR_CYAN "%-22s " COLOR_RESET "| " COLOR_CYAN "%-23s " COLOR_CYAN "║\n", 
           shieldJ, shieldE);
    
    printf("╚══════════════════════════════════════════════════╝" COLOR_RESET "\n");
}

void lancerCombat(Vaisseau *joueur) {
    Vaisseau ennemi = genererEnnemi(joueur->distanceParcourue);
    
    SLEEP_MS(1500);

    while (joueur->coque > 0 && ennemi.coque > 0) {
        tourCombat(joueur, &ennemi);
    }

    if (joueur->coque > 0) {
        int gain = (rand() % 20) + 15;
        printf(COLOR_GREEN "\nVICTOIRE ! " COLOR_RESET "Vous recuperez " COLOR_YELLOW "%d Ferraille" COLOR_RESET ".\n", gain);
        joueur->ferraille += gain;
    } else {
        printf(COLOR_RED "\n[DETRUITE] Votre vaisseau se desintegre dans le vide...\n" COLOR_RESET);
    }
    SLEEP_MS(2000);
}

void tourCombat(Vaisseau *joueur, Vaisseau *ennemi) {
    int choixAction, choixArme;
    afficherEtatCombat(joueur, ennemi);

    printf("\n--- VOTRE TOUR ---\n");
    printf("1. ATTAQUER\n");
    printf("2. RECHARGER BOUCLIERS (Regen +2 a +4)\n");
    printf("Choix : ");
    scanf("%d", &choixAction);

    if (choixAction == 1) {
        // --- SOUS-MENU ATTAQUE ---
        printf("\n--- CHOIX DE L'ARME ---\n");
        printf("1. Canon Laser (Peut percer le bouclier)\n");
        printf("2. Missile (Ignore bouclier | Munitions: %d)\n", joueur->missiles);
        printf("Choix : ");
        scanf("%d", &choixArme);

        printf("\nFeu !");
        SLEEP_MS(600);

        if (checkEsquive(15, joueur)) { // Ici 15 représente l'esquive ennemie de base
            printf("\nL'ennemi a esquive votre tir !\n");
        } else {
            if (choixArme == 2 && joueur->missiles > 0) {
                joueur->missiles--;
                int degatsFinaux = calculerDegats(3 + (joueur->distanceParcourue / 5), joueur->moteurs);
                ennemi->coque -= degatsFinaux;
                printf("\n" COLOR_RED "MISSILE : Impact direct sur la coque (-%d) !" COLOR_RESET "\n", degatsFinaux);
            } 
            else {
                int degatsTotal = calculerDegats(joueur->armes, joueur->moteurs);
                
                if (ennemi->bouclier > 0) {
                    if (ennemi->bouclier >= degatsTotal) {
                        ennemi->bouclier -= degatsTotal;
                        printf(COLOR_CYAN "\nLASER : Le bouclier ennemi absorbe tout (-%d)." COLOR_RESET "\n", degatsTotal);
                    } else {
                        int surplus = degatsTotal - ennemi->bouclier;
                        printf(COLOR_YELLOW "\nLASER : Bouclier percé ! " COLOR_RESET);
                        printf("Le bouclier absorbe %d et la " COLOR_RED "coque subit %d !" COLOR_RESET "\n", ennemi->bouclier, surplus);
                        ennemi->bouclier = 0;
                        ennemi->coque -= surplus;
                    }
                } else {
                    ennemi->coque -= degatsTotal;
                    printf("\nLASER : Impact direct sur la coque (" COLOR_RED "-%d" COLOR_RESET ") !\n", degatsTotal);
                }
            }
        }
        
        // Recharge automatique légère après l'attaque
        if (joueur->bouclier < joueur->bouclierMax) {
            joueur->bouclier += 1;
            printf("[SYSTEME] Recharge automatique du bouclier (+1).\n");
        }

    } else if (choixAction == 2) {
        int regen = (rand() % 3) + 2;
        joueur->bouclier += regen;
        if (joueur->bouclier > joueur->bouclierMax) joueur->bouclier = joueur->bouclierMax;
        printf("\n[MANOEUVRE] Vous déviez l'énergie aux boucliers (" COLOR_CYAN "+%d" COLOR_RESET ") !\n", regen);
    }

    SLEEP_MS(1500);

    // --- CONTRE-ATTAQUE ENNEMIE ---
    if (ennemi->coque > 0) {
        printf("\nL'ennemi riposte...");
        SLEEP_MS(800);

        if (checkEsquive(10, joueur)) {
            printf("\nESQUIVE ! Le tir ennemi vous manque.\n");
        } else {
            int degatsEnnemi;
            
            // L'ennemi utilise un missile s'il en a
            if (ennemi->missiles > 0) {
                printf("\n" COLOR_RED "L'ENNEMI LANCE UN MISSILE !" COLOR_RESET);
                degatsEnnemi = calculerDegats(3 + (ennemi->distanceParcourue / 5), ennemi->moteurs);
                joueur->coque -= degatsEnnemi;
                ennemi->missiles--;
                printf(" Impact direct sur votre coque (-%d) !\n", degatsEnnemi);
            } 
            else {
                degatsEnnemi = calculerDegats(ennemi->armes, ennemi->moteurs);
                
                if (joueur->bouclier > 0) {
                    if (joueur->bouclier >= degatsEnnemi) {
                        joueur->bouclier -= degatsEnnemi;
                        printf(COLOR_CYAN "\nVotre bouclier encaisse tout ! (-%d)" COLOR_RESET "\n", degatsEnnemi);
                    } else {
                        int surplus = degatsEnnemi - joueur->bouclier;
                        printf(COLOR_YELLOW "\nALERTE : Bouclier surchargé ! " COLOR_RESET);
                        printf(COLOR_RED "%d dégâts ont traversé jusqu'à la coque !" COLOR_RESET "\n", surplus);
                        joueur->bouclier = 0;
                        joueur->coque -= surplus;
                    }
                } else {
                    joueur->coque -= degatsEnnemi;
                    printf("\n" COLOR_RED "ALERTE ! Votre coque est touchée de plein fouet (-%d) !" COLOR_RESET "\n", degatsEnnemi);
                }
            }
        }
        SLEEP_MS(1500);
    }
}
bool checkEsquive(int chanceEsquive, Vaisseau *joueur) {
    int chanceTotale = chanceEsquive + (joueur->moteurs * 5); // varible ChanceEsquive de base + 5% par niveau de moteur
    if ((rand() % 100) < chanceTotale) {
        printf("ESQUIVE ! Les moteurs ont permis d'eviter le tir.\n");
        return true; // Le tir rate
    }
    return false; // Le tir touche
}

void rechargerBoucliers(Vaisseau *v) {
    if (v->bouclier < v->bouclierMax) {
        v->bouclier++; 
        printf("[SYSTEME] Bouclier regenere : %d/%d\n", v->bouclier, v->bouclierMax);
    }
}

Vaisseau genererEnnemi(int secteur) {
    Vaisseau ennemi;

    char *nomsEnnemis[] = {
        "Le Rogue One",
        "L'Etoile du Vide", 
        "Pilleur de Ferraille",
        "Nebuleuse Noire", 
        "L'Impitoyable", 
        "Vortex Sombre",
        "Chasseur de Primes", 
        "Le Fer a Repasser", 
        "Hacker Solaire",
        "Destructeur K-9", 
        "Le Faucon de Plomb", 
        "Silence Eternel",
        "Faucon Milénium",
        "Le Fer à Repasser"
    };
    int nbNoms = 14; // Nombre total de noms dans la liste
    strcpy(ennemi.nom, nomsEnnemis[rand() % nbNoms]);

    // Logique d'apparition du Capital Ship (Rare et seulement après secteur 10)
    int chanceCapital = rand() % 100;
    
    if (secteur >= 10 && chanceCapital < 25) { // 25% de chance
        strcpy(ennemi.nom, "CAPITAL SHIP REBELLE");
        printf(COLOR_RED "\n[ALERTE] SIGNATURE MASSIVE : %s détecté !" COLOR_RESET "\n", ennemi.nom);
        ennemi.coqueMax = 35 + secteur;
        ennemi.armes = 3 + (secteur / 5);
        ennemi.bouclierMax = 3;
        ennemi.missiles = 5;
    }


    int type = rand() % 3; // 3 types d'ennemis

    if (type == 0) { // Éclaireur (Rapide mais fragile)
        ennemi.coqueMax = 6 + secteur;
        ennemi.armes = 1 + (secteur / 4);
        ennemi.bouclierMax = 0 + (secteur / 5);
        ennemi.moteurs = 2 + (secteur / 5);
        printf("\n[SCAN] Contact visuel : %s", ennemi.nom);
        printf("\n[SCAN] Type : " COLOR_RED "ECLAIREUR" COLOR_RESET " (Faible coque, rapide)\n");
    } 
    else if (type == 1) { // Chasseur (Équilibré)
        ennemi.coqueMax = 10 + secteur;
        ennemi.armes = 1 + (secteur / 3);
        ennemi.bouclierMax = 1 + (secteur / 4);
        ennemi.missiles = (secteur > 3) ? 1 : 0; // Les chasseurs ont des missiles après le secteur 3
        printf("\n[SCAN] Contact visuel : %s", ennemi.nom);
        printf("\n[SCAN] Type : " COLOR_RED "CHASSEUR" COLOR_RESET " (Equilibre)\n");
    } 
    else { // Croiseur (Lourd et blindé)
        ennemi.coqueMax = 15 + secteur;
        ennemi.armes = 1 + (secteur / 5);
        ennemi.bouclierMax = 2 + (secteur / 4);
        printf("\n[SCAN] Contact visuel : %s", ennemi.nom);
        printf("\n[SCAN] Type : " COLOR_RED "CROISEUR" COLOR_RESET " (Blindage lourd)\n");
    }

    ennemi.coque = ennemi.coqueMax;
    ennemi.bouclier = ennemi.bouclierMax;
    ennemi.missiles = (secteur > 5) ? 2 : 0; // Les ennemis ont des missiles après le secteur 5
    
    return ennemi;
}

Vaisseau genererBossFinal() {
    Vaisseau boss;
    strcpy(boss.nom, "STARK ONE");
    
    boss.coqueMax = 50;
    boss.coque = 50;
    boss.bouclierMax = 4;
    boss.bouclier = 4;
    boss.armes = 4;
    boss.missiles = 10;
    
printf(COLOR_RED "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    printf("!!! " COLOR_YELLOW "ALERTE : SIGNATURE MASSIVE DETECTEE" COLOR_RED " !!!\n");
    printf("!!!      " COLOR_BOLD "LE VAISSEAU MERE EST ICI" COLOR_RESET COLOR_RED "        !!!\n");
    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" COLOR_RESET "\n");
    
    return boss;
}

int calculerDegats(int puissanceArme, int niveauMoteur) {
    int chanceCritique = 10 + (niveauMoteur * 2); // 10% + 2% par niveau moteur
    int r = rand() % 100;

    if (r < chanceCritique) {
        printf(COLOR_YELLOW " !!! COUP CRITIQUE !!! " COLOR_RESET "\n");
        return puissanceArme * 2; // Dégâts doublés
    }

    return puissanceArme;
}
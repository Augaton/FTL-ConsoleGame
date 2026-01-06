#include "combat.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void afficherEtatCombat(Vaisseau *joueur, Vaisseau *ennemi) {
    effacerEcran();
    printf("==================================================\n");
    printf("   VOUS          vs      %s\n", ennemi->nom);
    printf("   Coque: %d/%d      |   Coque: %d/%d\n", 
            joueur->coque, joueur->coqueMax, ennemi->coque, ennemi->coqueMax);
    printf("   Bouclier: %d/%d   |   Bouclier: %d/%d\n", 
            joueur->bouclier, joueur->bouclierMax, ennemi->bouclier, ennemi->bouclierMax);
    printf("==================================================\n");
}

void lancerCombat(Vaisseau *joueur) {
    Vaisseau ennemi = genererEnnemi(joueur->distanceParcourue);
    
    SLEEP_MS(1500);

    while (joueur->coque > 0 && ennemi.coque > 0) {
        tourCombat(joueur, &ennemi);
    }

    if (joueur->coque > 0) {
        int gain = (rand() % 20) + 15;
        if (strcmp(ennemi.nom, "CAPITAL SHIP REBELLE") == 0) {
            gain += 50; 
            joueur->missiles += 5; 
            printf("Incroyable ! Vous avez pille les reserves du Capital Ship.\n");
        }
        joueur->ferraille += gain;
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
        printf("1. Canon Laser (Bloque par bouclier)\n");
        printf("2. Missile (Ignore bouclier | Munitions: %d)\n", joueur->missiles);
        printf("Choix : ");
        scanf("%d", &choixArme);

        printf("\nFeu !");
        SLEEP_MS(600);

        if (checkEsquive(15, joueur)) {
            printf("\nL'ennemi a esquive votre tir !\n");
        } else {
            if (choixArme == 2 && joueur->missiles > 0) {
                joueur->missiles--;
                ennemi->coque -= 3;
                printf("\nMISSILE : Impact direct sur la coque (-3) !\n");
            } else {
                if (ennemi->bouclier > 0) {
                    ennemi->bouclier -= joueur->armes;
                    if (ennemi->bouclier < 0) ennemi->bouclier = 0;
                    printf("\nLASER : Le bouclier ennemi absorbe le choc.\n");
                } else {
                    ennemi->coque -= joueur->armes;
                    printf("\nLASER : Impact sur la coque (-%d) !\n", joueur->armes);
                }
            }
        }
        // Recharge bonus
        if (joueur->bouclier < joueur->bouclierMax) {
            joueur->bouclier += 1;
            printf("[SYSTEME] Recharge automatique du bouclier (+1).\n");
        }

    } else if (choixAction == 2) {
        // --- ACTION DE RECHARGE ---
        int regen = (rand() % 3) + 2;
        joueur->bouclier += regen;
        if (joueur->bouclier > joueur->bouclierMax) {
            joueur->bouclier = joueur->bouclierMax;
        }
        printf("\n[MANOEUVRE] Vous deviez l'energie aux boucliers (+%d) !\n", regen);
    }

    SLEEP_MS(1500);

    // --- CONTRE-ATTAQUE ENNEMIE ---
    if (ennemi->coque > 0) {
        printf("\nL'ennemi riposte...");
        SLEEP_MS(800);

        if (checkEsquive(10, joueur)) {
            printf("\nESQUIVE ! Le tir ennemi vous manque.\n");
        } else {
            if (ennemi->missiles > 0 && joueur->bouclier > 0) {
                printf("\nL'ENNEMI LANCE UN MISSILE !");
                joueur->coque -= 3;
                ennemi->missiles--;
            } else {
                if (joueur->bouclier > 0) {
                    joueur->bouclier -= ennemi->armes;
                    if (joueur->bouclier < 0) joueur->bouclier = 0;
                    printf("\nVotre bouclier encaisse l'attaque !\n");
                } else {
                    joueur->coque -= ennemi->armes;
                    printf("\nALERTE ! Votre coque est touchee (-%d) !\n", ennemi->armes);
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
        printf("\n[ALERTE] Un Capital Ship est sorti de l'hyperespace !\n");
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
        printf("\n[SCAN] Type : ECLAIREUR (Faible coque, rapide)\n");
    } 
    else if (type == 1) { // Chasseur (Équilibré)
        ennemi.coqueMax = 10 + secteur;
        ennemi.armes = 1 + (secteur / 3);
        ennemi.bouclierMax = 1 + (secteur / 4);
        ennemi.missiles = (secteur > 3) ? 1 : 0; // Les chasseurs ont des missiles après le secteur 3
        printf("\n[SCAN] Contact visuel : %s", ennemi.nom);
        printf("\n[SCAN] Type : CHASSEUR (Equilibre)\n");
    } 
    else { // Croiseur (Lourd et blindé)
        ennemi.coqueMax = 15 + secteur;
        ennemi.armes = 1 + (secteur / 5);
        ennemi.bouclierMax = 2 + (secteur / 4);
        printf("\n[SCAN] Contact visuel : %s", ennemi.nom);
        printf("\n[SCAN] Type : CROISEUR (Blindage lourd)\n");
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
    
    printf("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    printf("!!! ALERTE : SIGNATURE MASSIVE DETECTEE !!!\n");
    printf("!!!      LE VAISSEAU MERE EST ICI        !!!\n");
    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    
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
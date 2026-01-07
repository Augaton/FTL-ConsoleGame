#include "combat.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void afficherEtatCombat(Vaisseau *joueur, Vaisseau *ennemi) {
    effacerEcran();
    
    // On définit des largeurs fixes pour chaque colonne
    // Nom du joueur (15) + vs (4) + Nom ennemi (20) = 39 caractères de contenu
    printf(COLOR_CYAN "╔══════════════════════════════════════════════════╗\n");
    
    // Ligne des Noms
    printf("║ " COLOR_RESET "%-18s " COLOR_YELLOW "vs" COLOR_RESET "  %-25s " COLOR_CYAN "║\n", 
           joueur->nom, ennemi->nom);
    
    // Ligne des Coques
    char coqueJ[20], coqueE[20];
    sprintf(coqueJ, "Coque: %d/%d", joueur->coque, joueur->coqueMax);
    sprintf(coqueE, "Coque: %d/%d", ennemi->coque, ennemi->coqueMax);
    
    printf("║ " COLOR_RED "%-22s " COLOR_RESET "| " COLOR_RED "%-23s " COLOR_CYAN "║\n", 
           coqueJ, coqueE);
    
    // Ligne des Boucliers
    char shieldJ[25], shieldE[25];

    // On affiche le bouclier actuel sur le max (qui est l'efficacite du systeme)
    sprintf(shieldJ, "Shield: %d/%d", joueur->bouclierActuel, joueur->systemeBouclier.efficacite);
    sprintf(shieldE, "Shield: %d/%d", ennemi->bouclierActuel, ennemi->systemeBouclier.efficacite);

    printf("║ " COLOR_CYAN "%-22s " COLOR_RESET "| " COLOR_CYAN "%-23s " COLOR_CYAN "║\n", 
        shieldJ, shieldE);
    
    printf("╚══════════════════════════════════════════════════╝" COLOR_RESET "\n");
}

void lancerCombat(Vaisseau *joueur, Vaisseau *ennemi) {
    SLEEP_MS(1500);

    // 1. GESTION DU CONTACT
    if (joueur->ennemiPresent && joueur->ennemiCoqueActuelle > 0) {
        ennemi->coque = joueur->ennemiCoqueActuelle;
        printf(COLOR_YELLOW "\n[REPRISE] Contact maintenu : %s (%d/%d)" COLOR_RESET "\n",
               ennemi->nom, ennemi->coque, ennemi->coqueMax);
    } else {
        joueur->ennemiPresent = 1;
        joueur->ennemiCoqueActuelle = ennemi->coque;
        printf("\n" COLOR_RED "[ALERTE]" COLOR_RESET " Contact visuel : %s", ennemi->nom);
        sauvegarderPartie(joueur);
    }
    
    SLEEP_MS(1000);
    srand((unsigned int)time(NULL));

    // 2. BOUCLE DE COMBAT
    while (joueur->coque > 0 && ennemi->coque > 0) {
        tourCombat(joueur, ennemi);
        joueur->ennemiCoqueActuelle = ennemi->coque;
        sauvegarderPartie(joueur);
    }

    // 3. ISSUE DU COMBAT
    if (joueur->coque > 0) {
        int gain = (rand() % 20) + 15;
        printf(COLOR_GREEN "\nVICTOIRE ! " COLOR_RESET "Le %s est detruit.\n", ennemi->nom);
        printf("Recuperation de " COLOR_YELLOW "%d Ferraille" COLOR_RESET ".\n", gain);
        joueur->ferraille += gain;

        // --- SYSTEME DE LOOT D'EQUIPEMENT ---
        // 15% de chance de loot si l'ennemi a un meilleur rang
        if ((rand() % 100) < 15 && ennemi->systemeArme.rang > joueur->systemeArme.rang) {
            printf(COLOR_MAGENTA "\n[SCANNER] Technologie superieure detectee !" COLOR_RESET);
            printf("\nVous remplacez votre arme par : " COLOR_CYAN "%s (Mk %d)" COLOR_RESET "\n", 
                   ennemi->systemeArme.nom, ennemi->systemeArme.rang);
            joueur->systemeArme = ennemi->systemeArme;
            SLEEP_MS(1000);
        }

        joueur->ennemiPresent = 0;
        joueur->ennemiCoqueActuelle = 0;
        sauvegarderPartie(joueur);
    } else {
        printf(COLOR_RED "\n[CRITIQUE] Votre vaisseau se desintegre...\n" COLOR_RESET);
    }
    SLEEP_MS(2000);
}

void tourCombat(Vaisseau *joueur, Vaisseau *ennemi) {
    int choixAction, choixArme;
    afficherEtatCombat(joueur, ennemi);

    printf(COLOR_CYAN "\n--- VOTRE TOUR ---\n" COLOR_RESET);
    printf(COLOR_YELLOW "1. ATTAQUER\n" COLOR_RESET);
    printf(COLOR_BLUE "2. RECHARGER BOUCLIERS (Regen +2 a +4)\n" COLOR_RESET);
    printf(COLOR_YELLOW "> " COLOR_RESET);
    scanf("%d", &choixAction);

    if (choixAction == 1) {
        // --- SOUS-MENU ATTAQUE ---
        printf(COLOR_BLUE "\n--- CHOIX DE L'ARME ---\n" COLOR_RESET);
        printf(COLOR_YELLOW "1. Canon Laser (Peut percer le bouclier)\n");
        printf(COLOR_RED "2. Missile (Ignore bouclier | Munitions: %d)\n" COLOR_RESET, joueur->missiles);
        printf(COLOR_YELLOW "> " COLOR_RESET);
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
                int degatsBase = joueur->systemeArme.efficacite;
                int degatsTotal = calculerDegats(degatsBase, joueur->moteurs);

                if (ennemi->bouclierActuel > 0) {
                    if (ennemi->bouclierActuel >= degatsTotal) {
                        ennemi->bouclierActuel -= degatsTotal;
                        printf(COLOR_CYAN "\nLASER : Le bouclier ennemi absorbe tout (-%d)." COLOR_RESET "\n", degatsTotal);
                    } else {
                        int surplus = degatsTotal - ennemi->bouclierActuel;
                        printf(COLOR_YELLOW "\nLASER : Bouclier percé ! " COLOR_RESET);
                        printf("Le bouclier absorbe %d et la " COLOR_RED "coque subit %d !" COLOR_RESET "\n", ennemi->bouclierActuel, surplus);
                        ennemi->bouclierActuel = 0;
                        ennemi->coque -= surplus;
                    }
                } else {
                    ennemi->coque -= degatsTotal;
                    printf("\nLASER : Impact direct sur la coque (" COLOR_RED "-%d" COLOR_RESET ") !\n", degatsTotal);
                }
            }
        }
        
        // Recharge automatique légère après l'attaque
        if (joueur->bouclierActuel < joueur->systemeBouclier.efficacite) {
            joueur->bouclierActuel += 1;
            printf("[SYSTEME] Recharge automatique du bouclier (+1).\n");
        }

    } else if (choixAction == 2) {
        int regen = (rand() % 3) + 2;
        joueur->bouclierActuel += regen;
        if (joueur->bouclierActuel > joueur->systemeBouclier.efficacite) joueur->bouclierActuel = joueur->systemeBouclier.efficacite;
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
                int degatsBase = joueur->systemeArme.efficacite;
                int degatsEnnemi = calculerDegats(degatsBase, joueur->moteurs);

                if (joueur->bouclierActuel > 0) {
                    if (joueur->bouclierActuel >= degatsEnnemi) {
                        joueur->bouclierActuel -= degatsEnnemi;
                        printf(COLOR_CYAN "\nVotre bouclier encaisse tout ! (-%d)" COLOR_RESET "\n", degatsEnnemi);
                    } else {
                        int surplus = degatsEnnemi - joueur->bouclierActuel;
                        printf(COLOR_YELLOW "\nALERTE : Bouclier surchargé ! " COLOR_RESET);
                        printf(COLOR_RED "%d dégâts ont traversé jusqu'à la coque !" COLOR_RESET "\n", surplus);
                        joueur->bouclierActuel = 0;
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
    if (v->bouclierActuel < v->systemeBouclier.efficacite) {
        v->bouclierActuel++; 
        printf("[SYSTEME] Bouclier regenere : %d/%d\n", v->bouclierActuel, v->systemeBouclier.efficacite);
    }
}

Vaisseau genererEnnemi(int secteur, unsigned int seed) {
    Vaisseau ennemi;
    srand(seed);

    char *nomsEnnemis[] = {"Rogue One", "Etoile du Vide", "Pilleur", "Vortex Sombre", "Hacker Solaire"};
    strcpy(ennemi.nom, nomsEnnemis[rand() % 5]);

    // --- Calcul du Rang global de l'ennemi (Secteur 1-10 -> Mk1, 10-20 -> Mk2, etc.) ---
    int rangEnnemi = 1 + (secteur / 10);
    if (rangEnnemi > 5) rangEnnemi = 5; // On cap à Mk V

    // Logique Capital Ship
    if (secteur >= 10 && (rand() % 100) < 25) {
        strcpy(ennemi.nom, "CAPITAL SHIP REBELLE");
        ennemi.coqueMax = 35 + secteur;
        ennemi.moteurs = 1;
        
        // Équipement lourd
        sprintf(ennemi.systemeArme.nom, "Batterie Laser Mk %d", rangEnnemi + 1);
        ennemi.systemeArme.rang = rangEnnemi + 1;
        ennemi.systemeArme.efficacite = 4 + (secteur / 5);

        sprintf(ennemi.systemeBouclier.nom, "Bouclier de Flotte Mk %d", rangEnnemi);
        ennemi.systemeBouclier.rang = rangEnnemi;
        ennemi.systemeBouclier.efficacite = 3;
    } else {
        // Types classiques (Eclaireur, Chasseur, Croiseur)
        int type = rand() % 3;
        ennemi.moteurs = (type == 0) ? 2 : 1; 
        ennemi.coqueMax = 10 + secteur;

        // Génération automatique du nom d'équipement
        sprintf(ennemi.systemeArme.nom, "Laser Ennemi Mk %d", rangEnnemi);
        ennemi.systemeArme.rang = rangEnnemi;
        ennemi.systemeArme.efficacite = 1 + (secteur / 4);

        sprintf(ennemi.systemeBouclier.nom, "Deflecteur Mk %d", rangEnnemi);
        ennemi.systemeBouclier.rang = rangEnnemi;
        ennemi.systemeBouclier.efficacite = secteur / 5;
    }

    ennemi.coque = ennemi.coqueMax;
    ennemi.bouclierActuel = ennemi.systemeBouclier.efficacite;
    ennemi.missiles = (secteur > 5) ? 2 : 0;
    
    return ennemi;
}

Vaisseau genererBossFinal() {
    
    Vaisseau boss;
    strcpy(boss.nom, "DESTROYEUR STELLAIRE");
    boss.coque = 100;
    boss.coqueMax = 100;
    boss.moteurs = 5;
    boss.missiles = 99;

    // Arme Unique
    strcpy(boss.systemeArme.nom, "CANON APOCALYPSE");
    boss.systemeArme.rang = 99;
    boss.systemeArme.efficacite = 10;

    // Bouclier Unique
    strcpy(boss.systemeBouclier.nom, "FORTERESSE IONIQUE");
    boss.systemeBouclier.rang = 10;
    boss.systemeBouclier.efficacite = 5;
    boss.bouclierActuel = 5;
    
    // Initialise TOUT le reste à 0 pour éviter les chiffres bizarres
    boss.moteurs = 5;
    boss.distanceParcourue = 0; 
    boss.ferraille = 0;
    
    
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
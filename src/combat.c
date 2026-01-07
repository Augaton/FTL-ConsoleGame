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
    char shieldJ[20], shieldE[20];
    sprintf(shieldJ, "Shield: %d/%d", joueur->bouclier, joueur->bouclierMax);
    sprintf(shieldE, "Shield: %d/%d", ennemi->bouclier, ennemi->bouclierMax);
    
    printf("║ " COLOR_CYAN "%-22s " COLOR_RESET "| " COLOR_CYAN "%-23s " COLOR_CYAN "║\n", 
           shieldJ, shieldE);
    
    printf("╚══════════════════════════════════════════════════╝" COLOR_RESET "\n");
}

void lancerCombat(Vaisseau *joueur, Vaisseau *ennemi) {
    SLEEP_MS(1500);

    // 1. GESTION DU CONTACT ET PERSISTANCE
    if (joueur->ennemiPresent && joueur->ennemiCoqueActuelle > 0) {
        ennemi->coque = joueur->ennemiCoqueActuelle;
        printf(COLOR_YELLOW "\n[REPRISE] Contact maintenu avec : %s (%d/%d)" COLOR_RESET "\n",
               ennemi->nom, ennemi->coque, ennemi->coqueMax);
    }
    else
     {
        joueur->ennemiPresent = 1;
        joueur->ennemiCoqueActuelle = ennemi->coque;
        
        printf("\n[SCAN] Contact visuel : %s", ennemi->nom);
        sauvegarderPartie(joueur);
    }
    SLEEP_MS(1000);

    srand((unsigned int)time(NULL));

    // 2. BOUCLE DE COMBAT
    while (joueur->coque > 0 && ennemi->coque > 0) {
        tourCombat(joueur, ennemi);

        // Mise à jour constante de la sauvegarde pour éviter la triche (Alt+F4)
        joueur->ennemiCoqueActuelle = ennemi->coque;
        sauvegarderPartie(joueur);
    }

    // 3. ISSUE DU COMBAT
    if (joueur->coque > 0) {
        // Victoire
        int gain = (rand() % 20) + 15;
        printf(COLOR_GREEN "\nVICTOIRE ! " COLOR_RESET "Le %s est en morceaux.\n", ennemi->nom);
        printf("Vous recuperez " COLOR_YELLOW "%d Ferraille" COLOR_RESET " dans les debris.\n", gain);
        
        joueur->ferraille += gain;
        joueur->ennemiPresent = 0; // TRÈS IMPORTANT : On clôture le combat
        joueur->ennemiCoqueActuelle = 0;
        
        sauvegarderPartie(joueur);
    } else {
        // Défaite
        printf(COLOR_RED "\n[DETRUITE] Votre vaisseau se desintegre dans le vide...\n" COLOR_RESET);
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

Vaisseau genererEnnemi(int secteur, unsigned int seed) {
    Vaisseau ennemi;
    srand(seed); // Fixe l'aléatoire pour ce combat précis

    char *nomsEnnemis[] = {
        "Le Rogue One", "L'Etoile du Vide", "Pilleur de Ferraille",
        "Nebuleuse Noire", "L'Impitoyable", "Vortex Sombre",
        "Chasseur de Primes", "Le Fer a Repasser", "Hacker Solaire",
        "Destructeur K-9", "Le Faucon de Plomb", "Silence Eternel",
        "Faucon Milénium"
    };
    int nbNoms = 13;
    strcpy(ennemi.nom, nomsEnnemis[rand() % nbNoms]);

    // Logique Capital Ship
    int chanceCapital = rand() % 100;
    if (secteur >= 10 && chanceCapital < 25) {
        strcpy(ennemi.nom, "CAPITAL SHIP REBELLE");
        ennemi.coqueMax = 35 + secteur;
        ennemi.armes = 3 + (secteur / 5);
        ennemi.bouclierMax = 3;
        ennemi.moteurs = 1;
        ennemi.missiles = 5;
    } else {
        // Types classiques
        int type = rand() % 3;
        if (type == 0) { // ECLAIREUR
            ennemi.coqueMax = 6 + secteur;
            ennemi.armes = 1 + (secteur / 4);
            ennemi.bouclierMax = 0 + (secteur / 5);
            ennemi.moteurs = 2 + (secteur / 5);
        } 
        else if (type == 1) { // CHASSEUR
            ennemi.coqueMax = 10 + secteur;
            ennemi.armes = 1 + (secteur / 3);
            ennemi.bouclierMax = 1 + (secteur / 4);
            ennemi.moteurs = 1;
        } 
        else { // CROISEUR
            ennemi.coqueMax = 15 + secteur;
            ennemi.armes = 1 + (secteur / 5);
            ennemi.bouclierMax = 2 + (secteur / 4);
            ennemi.moteurs = 0;
        }
    }

    ennemi.coque = ennemi.coqueMax;
    ennemi.bouclier = ennemi.bouclierMax;
    ennemi.missiles = (secteur > 5) ? 2 : 0;
    
    return ennemi;
}

Vaisseau genererBossFinal() {
    
    Vaisseau boss;
    strcpy(boss.nom, "DESTROYEUR STELLAIRE");
    boss.coque = 100;
    boss.coqueMax = 100;
    boss.bouclier = 4;
    boss.bouclierMax = 4;
    boss.armes = 10;
    boss.missiles = 99;
    
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
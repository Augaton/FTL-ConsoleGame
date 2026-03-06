#include "combat.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "interface.h"


void afficherEtatCombat(Vaisseau *joueur, Vaisseau *ennemi) {
    effacerEcran();

    char playerName[15];
    if (strlen(joueur->nom) > 14) {
        strncpy(playerName, joueur->nom, 14);
        playerName[11] = '.'; playerName[12] = '.'; playerName[13] = '.';
        playerName[14] = '\0';
    } else {
        strcpy(playerName, joueur->nom);
    }
    
    printf(COLOR_CYAN "╔══════════════════════════════════════════════════╗\n");

    printf("║ " COLOR_RESET "%-15s " COLOR_YELLOW "vs" COLOR_RESET "  %-15s " COLOR_MAGENTA, 
        playerName, ennemi->nom);

    afficherBarreFTL(ennemi->chargeFTL);

    printf(COLOR_CYAN "   ║\n");
    
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

    // 2. BOUCLE DE COMBAT
    while (joueur->coque > 0 && ennemi->coque > 0) {
        tourCombat(joueur, ennemi);
        joueur->ennemiCoqueActuelle = ennemi->coque;
        sauvegarderPartie(joueur);
    }

    // 3. ISSUE DU COMBAT
    if (joueur->coque > 0) {
        int gain = (rand() % 20) + 15;

        // si fuite
        if (ennemi->chargeFTL >= ennemi->maxchargeFTL) {
            printf(COLOR_YELLOW "\nL'ennemi a sauté dans l'hyper-espace... Vous avez perdu votre proie." COLOR_RESET "\n");
            SLEEP_MS(800);
            return;
        }

        if (joueur->chargeFTL >= joueur->maxchargeFTL) {
            printf(COLOR_GREEN "SAUT EFFECTUÉ ! Vous avez échappé au combat." COLOR_RESET "\n");
            SLEEP_MS(800);
            return;
        }


        // Si tuer pendant la fuite de l'ennemie, petit bonus
        if (ennemi->chargeFTL > 0) {
            int bonus = gain / 2; // +50% de bonus
            gain += bonus;
            printf(COLOR_MAGENTA "\n[INTERCEPTION] Réacteur FTL ennemi surchargé ! Explosion massive !" COLOR_RESET);
            printf(COLOR_YELLOW "\nBonus de récupération : +%d Ferraille\n" COLOR_RESET, bonus);
        }

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
        attendreJoueur();
    } else {
        printf(COLOR_RED "\n[CRITIQUE] Votre vaisseau se desintegre...\n" COLOR_RESET);
    }
    SLEEP_MS(2000);
}

void tourCombat(Vaisseau *joueur, Vaisseau *ennemi) {
    int choixAction, choixArme, choixCible;
    int tourFini = 0; 
    int estBossFinal = (strcmp(ennemi->nom, "DESTROYEUR STELLAIRE") == 0);

    // REINITIALISER LES LOGS POUR CE TOUR
    reinitialiserLogsCombat();

    // RESET DEBUFFS DEBUT DE TOUR
    if (joueur->debuffMoteur > 0) joueur->debuffMoteur--;
    if (joueur->debuffArme > 0) joueur->debuffArme--;

    do {
        // Affichage d'état (printf) - terminer ncurses avant
        terminerNCurses();
        afficherEtatCombat(joueur, ennemi);
        
        if (ennemi->debuffArme > 0) printf(COLOR_RED "[INFO] Armes ennemies endommagees (Dégâts réduits)\n" COLOR_RESET);
        if (ennemi->debuffMoteur > 0) printf(COLOR_RED "[INFO] Moteurs ennemis HS (Esquive nulle)\n" COLOR_RESET);

        // Réinitialiser ncurses pour le menu
        initialiserNCurses();
        choixAction = menuCombatAction(joueur, ennemi);

        // --- 1. ATTAQUER ---
        if (choixAction == 1) {
            // Calculs Probabilités
            int baseEsquive = (ennemi->debuffMoteur > 0) ? 0 : 5; 
            int esquiveEnnemiBase = baseEsquive + (ennemi->moteurs * 5);

            int esquiveFinaleCoque = esquiveEnnemiBase - joueur->precision;
            if (esquiveFinaleCoque < 0) esquiveFinaleCoque = 0;
            int chanceCoque = 100 - esquiveFinaleCoque;
            if (chanceCoque > 100) chanceCoque = 100;

            int esquiveFinaleSysteme = (esquiveEnnemiBase + 20) - joueur->precision;
            if (esquiveFinaleSysteme < 0) esquiveFinaleSysteme = 0;
            int chanceSysteme = 100 - esquiveFinaleSysteme;
            if (chanceSysteme < 0) chanceSysteme = 0;

            // Menu Cible (ncurses)
            choixCible = menuChoixCible(chanceCoque, chanceSysteme, joueur, ennemi);
            if (choixCible == 0) continue; 

            // Menu Arme (ncurses)
            choixArme = menuChoixArme(joueur->missiles, joueur, ennemi);
            if (choixArme == 0) continue;

            // Afficher résultat du tir (printf)
            terminerNCurses(); 

            // EXECUTION TIR
            ajouterLogCombat("Feu!");
            printf(COLOR_BOLD COLOR_RED "\nFeu !" COLOR_RESET);
            SLEEP_MS(600);

            int malusVisee = (choixCible == 2 || choixCible == 3) ? 20 : 0;

            int bonusPrecision = getBonusPrecision(joueur);
            
            // On soustrait le bonus soldat à l'esquive ennemie
            int esquivePourCalcul = baseEsquive + (ennemi->moteurs * 5) + malusVisee - bonusPrecision; 
            
            if (bonusPrecision > 0) {
                printf("[SOLDAT] Visée assistée : Esquive ennemie -%d%%\n", bonusPrecision);
            }
            
            if (checkEsquive(esquivePourCalcul, joueur)) { 
                ajouterLogCombat("Esquive! Tir échoué");
                printf(COLOR_RED "\nLe tir passe a cote de la cible visée !\n" COLOR_RESET);
                SLEEP_MS(800);
            } else {
                int degats = joueur->systemeArme.efficacite;

                // --- BONUS SOLDAT (Dégâts) ---
                int bonusSoldatDmg = getBonusDegats(joueur); // Dégât brut
                int bonusSoldatCrit = getBonusCritique(joueur); // % Critique (Nouveau)

                
                if (bonusSoldatDmg > 0) {
                    char logMsg[60];
                    sprintf(logMsg, "[SOLDAT] Tir +%d", bonusSoldatDmg);
                    ajouterLogCombat(logMsg);
                    degats += bonusSoldatDmg;
                    printf(COLOR_RED "[SOLDAT] Tir optimisé ! (+%d Dégât)\n" COLOR_RESET, bonusSoldatDmg);
                }

                int estTouche = 0;

                if (choixArme == 2 && joueur->missiles > 0) {
                    joueur->missiles--;
                    degats += calculerDegats(3 + (joueur->distanceParcourue/5), joueur->moteurs, bonusSoldatCrit);
                    estTouche = 1;
                    char logMsg[60];
                    sprintf(logMsg, "MISSILE Impact! -%d", degats);
                    ajouterLogCombat(logMsg);
                    printf("\n" COLOR_RED "MISSILE : Impact direct ! (-%d)" COLOR_RESET "\n", degats);
                } else if (choixArme == 2) {
                    ajouterLogCombat("Pas assez de missiles!");
                    printf("Click... Plus de missiles !\n");
                } else {
                    if (ennemi->bouclierActuel >= degats) {
                        ennemi->bouclierActuel -= degats;
                        ajouterLogCombat("Bouclier ennemi OK");
                        printf(COLOR_CYAN "\nLe bouclier ennemi absorbe le tir.\n" COLOR_RESET);
                    } else {
                        degats = degats - ennemi->bouclierActuel;
                        ennemi->bouclierActuel = 0;
                        estTouche = 1;
                        char logMsg2[60];
                        sprintf(logMsg2, "Coque! -%d PV", degats);
                        ajouterLogCombat(logMsg2);
                        printf(COLOR_RED "\nCoque touchée ! (-%d)\n" COLOR_RESET, degats);
                    }
                }

                if (estTouche) {
                    ennemi->coque -= degats;
                    if (choixCible == 2) {
                        ennemi->debuffArme = 2;
                        ajouterLogCombat("ARMES ENNEMIES OK!");
                        printf(COLOR_MAGENTA ">>> ARMES ENNEMIES ENDOMMAGEES ! <<<\n" COLOR_RESET);
                    }
                    if (choixCible == 3) {
                        ennemi->debuffMoteur = 2;
                        ajouterLogCombat("MOTEURS ENNEMI HS!");
                        printf(COLOR_MAGENTA ">>> MOTEURS ENNEMIS CRITIQUES ! <<<\n" COLOR_RESET);
                    }
                    
                    // --- XP SOLDAT ---
                    for(int i=0; i<3; i++) {
                        if(joueur->equipage[i].role == ROLE_SOLDAT && joueur->equipage[i].estVivant) {
                            gagnerXP(&joueur->equipage[i], 15);
                            SLEEP_MS(300);
                        }
                    }
                }
            }
            
            SLEEP_MS(1000);
            
            int bouclierMaxReel = joueur->systemeBouclier.efficacite + getBonusCapaciteBouclier(joueur);

            // Recharge automatique
            if (joueur->bouclierActuel < bouclierMaxReel) { // Utilise la nouvelle limite
                joueur->bouclierActuel++;
                ajouterLogCombat("[SYSTEME] +1 Bouclier");
                printf(COLOR_BLUE "[SYSTEME] Recharge passive : +1 Bouclier.\n" COLOR_RESET);

                // Check du bonus "Double Recharge"
                int chanceBonus = getBonusRechargeBouclier(joueur); // Nouveau nom
                if (chanceBonus > 0 && (rand()%100 < chanceBonus) && joueur->bouclierActuel < bouclierMaxReel) {
                    joueur->bouclierActuel++;
                    ajouterLogCombat("[INGENIEUR] +1 Bonus!");
                    printf(COLOR_CYAN "[INGENIEUR] Surcharge des condensateurs ! (+1 Extra)\n" COLOR_RESET);
                    
                    // XP Ingénieur
                    for(int i=0; i<3; i++) {
                        if(joueur->equipage[i].role == ROLE_INGENIEUR) gagnerXP(&joueur->equipage[i], 15);
                    }
                }
            }

            if (tenterReparationAutomatique(joueur)) {
                ajouterLogCombat("[INGENIEUR] +1 Coque!");
                printf(COLOR_GREEN "[INGENIEUR] \"J'ai colmaté une brèche !\" (+1 Coque)\n" COLOR_RESET);
                SLEEP_MS(800);
            }


            tourFini = 1;
        }
        
        // --- 2. RECHARGER BOUCLIERS ---
        else if (choixAction == 2) {
            terminerNCurses();
            int regen = (rand() % 3) + 2; 
            int avant = joueur->bouclierActuel;
            joueur->bouclierActuel += regen;
            if (joueur->bouclierActuel > joueur->systemeBouclier.efficacite) {
                joueur->bouclierActuel = joueur->systemeBouclier.efficacite;
            }
            int gainReel = joueur->bouclierActuel - avant;
            char logMsg[60];
            sprintf(logMsg, "Recharge +%d [%d/%d]", gainReel, joueur->bouclierActuel, joueur->systemeBouclier.efficacite);
            ajouterLogCombat(logMsg);
            printf(COLOR_BLUE "\n[MANOEUVRE] Energie détournée vers les boucliers !\n" COLOR_RESET);
            printf("Récupération de " COLOR_CYAN "+%d barres" COLOR_RESET " (Total: %d/%d)\n", 
                gainReel, joueur->bouclierActuel, joueur->systemeBouclier.efficacite);
            
            // --- BONUS INGENIEUR (Actif) ---
            int chanceBonus = getBonusRechargeBouclier(joueur);
            if (chanceBonus > 0 && (rand()%100 < chanceBonus) && joueur->bouclierActuel < joueur->systemeBouclier.efficacite) {
                joueur->bouclierActuel++;
                ajouterLogCombat("[INGENIEUR] +1 Bonus!");
                printf(COLOR_CYAN "[INGENIEUR] Surcharge des boucliers ! (+1 Extra)\n" COLOR_RESET);
                for(int i=0; i<3; i++) {
                    if(joueur->equipage[i].role == ROLE_INGENIEUR) gagnerXP(&joueur->equipage[i], 20);
                }
            }

            joueur->bouclierActuel += regen;
            
            // NOUVEAU CALCUL DU MAX
            int maxTotal = joueur->systemeBouclier.efficacite + getBonusCapaciteBouclier(joueur);
            
            if (joueur->bouclierActuel > maxTotal) {
                joueur->bouclierActuel = maxTotal;
            }

            SLEEP_MS(1000);
            initialiserNCurses();
            tourFini = 1;
        }
        
        // --- 3. FUITE ---
        else if (choixAction == 3) {
            terminerNCurses();
            if (estBossFinal) {
                ajouterLogCombat("ERREUR: Champ FTL bloque!");
                printf(COLOR_RED "\n[ERREUR] Le vaisseau mère génère un champ inhibiteur ! Saut FTL impossible !\n" COLOR_RESET);
                SLEEP_MS(2000);
                initialiserNCurses();
            } else {
                joueur->chargeFTL++;
                char logMsg[60];
                sprintf(logMsg, "FTL Charge [%d/%d]", joueur->chargeFTL, joueur->maxchargeFTL);
                ajouterLogCombat(logMsg);
                printf(COLOR_YELLOW "\nChargement FTL...\n" COLOR_RESET);

                // --- BONUS PILOTE (Vitesse FTL) ---
                int chanceTurbo = getBonusVitesseFTL(joueur);
                if (chanceTurbo > 0 && (rand() % 100) < chanceTurbo) {
                    joueur->chargeFTL++;
                    ajouterLogCombat("[PILOTE] Trajet +1!");
                    printf(COLOR_CYAN "[PILOTE] Calcul de trajectoire optimisé ! (+1 Charge Extra)\n" COLOR_RESET);
                    
                    // XP Pilote pour la fuite réussie
                    for(int i=0; i<3; i++) {
                        if(joueur->equipage[i].role == ROLE_PILOTE) gagnerXP(&joueur->equipage[i], 10);
                    }
                }
                
                if (joueur->chargeFTL >= joueur->maxchargeFTL) { ennemi->coque = 0; return; }
                initialiserNCurses();
                tourFini = 1;
            }
        } 
        
        // --- 4. ANALYSER ---
        else if (choixAction == 4) {
            terminerNCurses();
            analyserEnnemi(joueur, ennemi);
            attendreJoueur();
            initialiserNCurses();
        }

    } while(!tourFini);

    SLEEP_MS(500);

    // --- TOUR DE L'ENNEMI ---
    if (ennemi->coque > 0) {
        ajouterLogCombat("\n--- RIPOSTE ENNEMI ---");
        printf("\nL'ennemi riposte...");
        SLEEP_MS(800);

        // --- BONUS PILOTE (Esquive) ---
        int bonusPilote = getBonusEsquive(joueur);
        int esquiveJoueur = 10 + (joueur->moteurs * 5) + bonusPilote;

        if (bonusPilote > 0) {
            char logMsg[60];
            sprintf(logMsg, "[PILOTE] Esquive +%d", bonusPilote);
            ajouterLogCombat(logMsg);
            printf(COLOR_CYAN "[PILOTE] Manoeuvre evasive ! (+%d%% Esquive)\n" COLOR_RESET, bonusPilote);
        }
        
        // Fuite Ennemie si PV bas
        if (ennemi->coque < (ennemi->coqueMax * 0.3) && !estBossFinal && ennemi->debuffMoteur == 0) {
             ennemi->chargeFTL++;
             ajouterLogCombat("Ennemi charge FTL!");
             printf(COLOR_RED "\n[ALERTE] L'ennemi charge son FTL pour fuir !\n" COLOR_RESET);
             if (ennemi->chargeFTL >= ennemi->maxchargeFTL) {
                 ennemi->coque = 0; return;
             }
             goto fin_tour_ennemi;
        }
        
        // --- RÉSOLUTION TIR ENNEMI ---
        if (checkEsquive(esquiveJoueur, joueur)) {
            ajouterLogCombat("ESQUIVE! Tir ennemi evite");
            printf(COLOR_GREEN "\nESQUIVE ! Vous évitez le tir.\n" COLOR_RESET);

            // --- XP PILOTE ---
            for(int i=0; i<3; i++) {
                if(joueur->equipage[i].role == ROLE_PILOTE && joueur->equipage[i].estVivant) {
                    gagnerXP(&joueur->equipage[i], 15);
                }
            }
        } else {
            // L'ennemi touche
            int degatsEntrants = ennemi->systemeArme.efficacite;
            
            if (ennemi->debuffArme > 0) {
                degatsEntrants = degatsEntrants / 2;
                ajouterLogCombat("Armes ennemi endommages");
                printf(COLOR_YELLOW "\n[CHANCE] Les armes endommagées de l'ennemi tirent faiblement !\n" COLOR_RESET);
            }
            
            degatsEntrants = calculerDegats(degatsEntrants, ennemi->moteurs, 0);

            if (joueur->bouclierActuel >= degatsEntrants) {
                joueur->bouclierActuel -= degatsEntrants;
                char logMsg[60];
                sprintf(logMsg, "Bouclier -%d", degatsEntrants);
                ajouterLogCombat(logMsg);
                printf(COLOR_CYAN "\nBouclier tient bon (-%d).\n" COLOR_RESET, degatsEntrants);
            } else {
                int surplus = degatsEntrants - joueur->bouclierActuel;
                joueur->bouclierActuel = 0;
                joueur->coque -= surplus;
                char logMsg2[60];
                sprintf(logMsg2, "Coque touchee! -%d", surplus);
                ajouterLogCombat(logMsg2);
                printf(COLOR_RED "\nALERTE ! Dégâts coque subis: -%d\n" COLOR_RESET, surplus);

                // --- DÉGÂTS SUR L'ÉQUIPAGE ---
                subirDegatsEquipage(joueur);
            }
        }
    }

    fin_tour_ennemi:
    if (ennemi->debuffArme > 0) ennemi->debuffArme--;
    if (ennemi->debuffMoteur > 0) ennemi->debuffMoteur--;
    
    SLEEP_MS(1500);
}

bool checkEsquive(int chanceEsquive, Vaisseau *attaquant) {
    int esquiveFinale = chanceEsquive - attaquant->precision;
    if (esquiveFinale < 0) esquiveFinale = 0;
    if (esquiveFinale > 80) esquiveFinale = 80;

    if ((rand() % 100) < esquiveFinale) {
        printf(COLOR_YELLOW "⚡ MANOEUVRE : La cible esquive le tir !\n" COLOR_RESET);
        return true;
    }
    return false;
}

void rechargerBoucliers(Vaisseau *v) {
    if (v->bouclierActuel < v->systemeBouclier.efficacite) {
        v->bouclierActuel++; 
        printf("[SYSTEME] Bouclier regenere : %d/%d\n", v->bouclierActuel, v->systemeBouclier.efficacite);
        SLEEP_MS(300);
    }
}

Vaisseau genererEnnemi(int secteur, unsigned int seed) {
    Vaisseau ennemi;
    unsigned int seedUnique = seed ^ (secteur * 2654435761u);
    srand(seedUnique);

    // --- GÉNÉRATEUR DE NOMS PROCÉDURAL ---
    char *prefixe[] = {"Vortex", "Spectre", "Chasseur", "Eclat", "Ombre", "Lame", "Titan", "Pilleur", "Comete", "Nebula"};
    char *suffixe[] = {"Solaire", "du Vide", "de Plasma", "Eternel", "de Sang", "de Fer", "Spectral", "du Chaos", "des Glaces"};

    // Calcul automatique du nombre d'éléments
    int nPre = sizeof(prefixe) / sizeof(prefixe[0]);
    int nSuf = sizeof(suffixe) / sizeof(suffixe[0]);

    sprintf(ennemi.nom, "%s %s", prefixe[rand() % nPre], suffixe[rand() % nSuf]);

    int rangEnnemi = 1 + (secteur / 10);
    if (rangEnnemi > 5) rangEnnemi = 5; 

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
        ennemi.moteurs = 1 + (secteur / 4);
        if (ennemi.moteurs > 5) ennemi.moteurs = 5;

        if (rand() % 100 < 30) ennemi.moteurs++;

        ennemi.coqueMax = rand() % 10 + secteur;

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
    ennemi.chargeFTL = 0;
    ennemi.maxchargeFTL = 3;
    ennemi.distanceParcourue = secteur;
    ennemi.precision = rand() % 5 + (secteur / 10);

    ennemi.debuffArme = 0;
    ennemi.debuffMoteur = 0;
    
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

int calculerDegats(int puissanceArme, int niveauMoteur, int bonusCritiqueSoldat) {
    // Base critique : 10% + 2% par moteur + Bonus Soldat
    int chanceCritique = 10 + (niveauMoteur * 2) + bonusCritiqueSoldat;
    
    int r = rand() % 100;
    if (r < chanceCritique) {
        printf(COLOR_YELLOW " !!! COUP CRITIQUE (Chance: %d%%) !!! " COLOR_RESET "\n", chanceCritique);
        return puissanceArme * 2;
    }
    return puissanceArme;
}
void analyserEnnemi(Vaisseau *joueur, Vaisseau *ennemi) {
    int base = (ennemi->debuffMoteur > 0) ? 0 : 5; 
    int esquiveBase = base + (ennemi->moteurs * 5);
    
    // Calcul réel avec la précision du joueur
    int esquiveReelle = esquiveBase - joueur->precision;
    if (esquiveReelle < 0) esquiveReelle = 0;

    printf("\n" COLOR_GREEN "==== [ SCANNER TACTIQUE ] ====" COLOR_RESET "\n");
    printf("Cible : " COLOR_BOLD "%-20s" COLOR_RESET "\n", ennemi->nom);
    printf("------------------------------\n");
    
    printf("OFFENSE  : " COLOR_RED "%-18s" COLOR_RESET " (Puissance: %d)\n", 
           ennemi->systemeArme.nom, ennemi->systemeArme.efficacite);
    
    if (ennemi->debuffArme > 0) {
         printf(COLOR_RED "           [!] SYSTÈME ENDOMMAGÉ : Dégâts réduits de 50%% (%d tours)\n" COLOR_RESET, ennemi->debuffArme);
    }

    printf("DEFENSE  : " COLOR_CYAN "%-18s" COLOR_RESET " (Bouclier: %d)\n", 
           ennemi->systemeBouclier.nom, ennemi->systemeBouclier.efficacite);
    
    printf("AGILITE  : " COLOR_YELLOW "Moteurs Lvl.%d" COLOR_RESET "\n", ennemi->moteurs);
    
    if (ennemi->debuffMoteur > 0) {
        printf(COLOR_RED "           [!] MOTEURS HS : Cible immobilisée (%d tours)\n" COLOR_RESET, ennemi->debuffMoteur);
        printf(COLOR_GREEN "           >> CHANCE DE TOUCHER : 100%% (Cible facile)\n" COLOR_RESET);
    } else {
        printf("           Esquive Cible : %d%% " COLOR_GREEN "(Votre Précision: -%d%%)" COLOR_RESET "\n", 
               esquiveBase, joueur->precision);
        printf("           " COLOR_BOLD "CHANCE DE TOUCHER : %d%%" COLOR_RESET "\n", 100 - esquiveReelle);
    }

    if (ennemi->systemeArme.rang > joueur->systemeArme.rang || 
        ennemi->systemeBouclier.rang > joueur->systemeBouclier.rang) {
        printf("\n" COLOR_MAGENTA "[!] SIGNAL DETECTE : Technologie supérieure récupérable !" COLOR_RESET "\n");
    } else {
        printf("\n" COLOR_WHITE "[i] Info : Pas de technologie notable à bord." COLOR_RESET "\n");
    }
    printf(COLOR_GREEN "==============================" COLOR_RESET "\n");
}
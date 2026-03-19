#include "combat.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int normaliserDifficulte(int d) {
    if (d < DIFFICULTE_FACILE || d > DIFFICULTE_DIFFICILE) return DIFFICULTE_NORMALE;
    return d;
}

static const char *libelleDifficulte(int d) {
    d = normaliserDifficulte(d);
    if (d == DIFFICULTE_FACILE) return "Facile";
    if (d == DIFFICULTE_DIFFICILE) return "Difficile";
    return "Normal";
}

static void construireBarre(char *out, size_t outSize, int valeur, int maxValeur, int largeur, char plein, char vide) {
    if (outSize == 0) return;
    if (maxValeur <= 0) maxValeur = 1;
    if (valeur < 0) valeur = 0;
    if (valeur > maxValeur) valeur = maxValeur;

    int remplis = (valeur * largeur) / maxValeur;
    if (remplis < 0) remplis = 0;
    if (remplis > largeur) remplis = largeur;

    int pos = 0;
    if (pos < (int)outSize - 1) out[pos++] = '[';
    for (int i = 0; i < largeur && pos < (int)outSize - 1; i++) {
        out[pos++] = (i < remplis) ? plein : vide;
    }
    if (pos < (int)outSize - 1) out[pos++] = ']';
    out[pos] = '\0';
}

static const char *etatCoqueTexte(int coque, int coqueMax) {
    if (coqueMax <= 0) return "CRITIQUE";
    float ratio = (float)coque / (float)coqueMax;
    if (ratio > 0.65f) return "STABLE";
    if (ratio > 0.30f) return "ENDOMMAGE";
    return "CRITIQUE";
}

static void tronquerTexte(char *dest, size_t destSize, const char *src, size_t maxVisible) {
    if (destSize == 0) return;
    if (src == NULL) src = "";

    size_t n = strlen(src);
    if (n <= maxVisible) {
        snprintf(dest, destSize, "%s", src);
        return;
    }

    if (maxVisible < 3) {
        snprintf(dest, destSize, "%.*s", (int)maxVisible, src);
        return;
    }

    size_t cut = maxVisible - 3;
    snprintf(dest, destSize, "%.*s...", (int)cut, src);
}

static void imprimerLigneBox(const char *content) {
    printf("| %-76.76s |\n", content);
}

static void imprimerLigneCombatColonnes(const char *gauche, const char *droite) {
    char ligne[128];
    snprintf(ligne, sizeof(ligne), " %-36.36s | %-36.36s", gauche, droite);
    imprimerLigneBox(ligne);
}

static void appliquerScalingDifficulteEnnemi(Vaisseau *joueur, Vaisseau *ennemi, int estBossFinal) {
    int d = normaliserDifficulte(joueur->difficulte);

    if (d == DIFFICULTE_FACILE) {
        ennemi->coqueMax = (ennemi->coqueMax * 85) / 100;
        if (ennemi->coqueMax < 5) ennemi->coqueMax = 5;
        if (ennemi->coque > ennemi->coqueMax) ennemi->coque = ennemi->coqueMax;
        ennemi->systemeArme.efficacite = (ennemi->systemeArme.efficacite * 85) / 100;
        if (ennemi->systemeArme.efficacite < 1) ennemi->systemeArme.efficacite = 1;
        if (!estBossFinal && ennemi->moteurs > 0) ennemi->moteurs--;
    } else if (d == DIFFICULTE_DIFFICILE) {
        ennemi->coqueMax = (ennemi->coqueMax * 120) / 100;
        if (ennemi->coqueMax < 8) ennemi->coqueMax = 8;
        ennemi->coque = ennemi->coqueMax;
        ennemi->systemeArme.efficacite = (ennemi->systemeArme.efficacite * 120) / 100;
        if (ennemi->systemeArme.efficacite < 2) ennemi->systemeArme.efficacite = 2;
        if (!estBossFinal) ennemi->moteurs++;
        if (ennemi->moteurs > 7) ennemi->moteurs = 7;
    }
}

void afficherEtatCombat(Vaisseau *joueur, Vaisseau *ennemi) {
    effacerEcran();

    char playerName[32], ennemiName[32];
    tronquerTexte(playerName, sizeof(playerName), joueur->nom, 28);
    tronquerTexte(ennemiName, sizeof(ennemiName), ennemi->nom, 28);

    int bouclierMaxJoueur = joueur->systemeBouclier.efficacite + getBonusCapaciteBouclier(joueur);
    if (bouclierMaxJoueur < 0) bouclierMaxJoueur = 0;

    char coqueJBar[20], coqueEBar[20], shieldJBar[20], shieldEBar[20], ligne[160];
    char gauche[64], droite[64];
    char armeJ[32], armeE[32];
    construireBarre(coqueJBar, sizeof(coqueJBar), joueur->coque, joueur->coqueMax, 12, '#', '-');
    construireBarre(coqueEBar, sizeof(coqueEBar), ennemi->coque, ennemi->coqueMax, 12, '#', '-');
    construireBarre(shieldJBar, sizeof(shieldJBar), joueur->bouclierActuel, bouclierMaxJoueur, 10, '+', '.');
    construireBarre(shieldEBar, sizeof(shieldEBar), ennemi->bouclierActuel, ennemi->systemeBouclier.efficacite, 10, '+', '.');
    tronquerTexte(armeJ, sizeof(armeJ), joueur->systemeArme.nom, 30);
    tronquerTexte(armeE, sizeof(armeE), ennemi->systemeArme.nom, 30);

    char ftlBar[16] = {0};
    int p = 0;
    ftlBar[p++] = '[';
    for (int i = 0; i < ennemi->maxchargeFTL && p < (int)sizeof(ftlBar) - 2; i++) {
        ftlBar[p++] = (i < ennemi->chargeFTL) ? '#' : '-';
    }
    ftlBar[p++] = ']';
    ftlBar[p] = '\0';

    printf(COLOR_CYAN "+------------------------------------------------------------------------------+\n" COLOR_RESET);
    snprintf(ligne, sizeof(ligne), "COMBAT TACTIQUE   [Difficulte: %s]", libelleDifficulte(joueur->difficulte));
    imprimerLigneBox(ligne);
    printf(COLOR_CYAN "+------------------------------------------------------------------------------+\n" COLOR_RESET);

    snprintf(gauche, sizeof(gauche), "ALLIE: %-28.28s", playerName);
    snprintf(droite, sizeof(droite), "HOSTILE: %-28.28s", ennemiName);
    imprimerLigneCombatColonnes(gauche, droite);

    snprintf(gauche, sizeof(gauche), "Coque    %-14s %2d/%-2d %-9s",
             coqueJBar, joueur->coque, joueur->coqueMax, etatCoqueTexte(joueur->coque, joueur->coqueMax));
    snprintf(droite, sizeof(droite), "Coque    %-14s %2d/%-2d %-9s",
             coqueEBar, ennemi->coque, ennemi->coqueMax, etatCoqueTexte(ennemi->coque, ennemi->coqueMax));
    imprimerLigneCombatColonnes(gauche, droite);

    snprintf(gauche, sizeof(gauche), "Bouclier %-14s %2d/%-2d",
             shieldJBar, joueur->bouclierActuel, bouclierMaxJoueur);
    snprintf(droite, sizeof(droite), "Bouclier %-14s %2d/%-2d",
             shieldEBar, ennemi->bouclierActuel, ennemi->systemeBouclier.efficacite);
    imprimerLigneCombatColonnes(gauche, droite);

    snprintf(gauche, sizeof(gauche), "Arme %-31.31s", armeJ);
    snprintf(droite, sizeof(droite), "Arme %-31.31s", armeE);
    imprimerLigneCombatColonnes(gauche, droite);

    snprintf(ligne, sizeof(ligne), "FTL ennemi: %-8s", ftlBar);
    imprimerLigneBox(ligne);

    printf(COLOR_CYAN "+------------------------------------------------------------------------------+\n" COLOR_RESET);
    snprintf(ligne, sizeof(ligne), "Ressources  Ferraille: %-4d  Missiles: %-3d  Precision: %-3d",
             joueur->ferraille, joueur->missiles, joueur->precision);
    imprimerLigneBox(ligne);
    printf(COLOR_CYAN "+------------------------------------------------------------------------------+\n" COLOR_RESET);
}

void lancerCombat(Vaisseau *joueur, Vaisseau *ennemi) {
    SLEEP_MS(1500);
    int estBossFinal = (strcmp(ennemi->nom, "DESTROYEUR STELLAIRE") == 0);

    // 1. GESTION DU CONTACT
    if (joueur->ennemiPresent && joueur->ennemiCoqueActuelle > 0) {
        ennemi->coque = joueur->ennemiCoqueActuelle;
        printf(COLOR_YELLOW "\n[REPRISE] Contact maintenu : %s (%d/%d)" COLOR_RESET "\n",
               ennemi->nom, ennemi->coque, ennemi->coqueMax);
    } else {
        appliquerScalingDifficulteEnnemi(joueur, ennemi, estBossFinal);
        joueur->ennemiPresent = 1;
        joueur->ennemiCoqueActuelle = ennemi->coque;
        printf("\n" COLOR_RED "[ALERTE]" COLOR_RESET " Contact visuel : %s | Difficulte: %s\n",
               ennemi->nom, libelleDifficulte(joueur->difficulte));
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

    // RESET DEBUFFS DEBUT DE TOUR
    if (joueur->debuffMoteur > 0) joueur->debuffMoteur--;
    if (joueur->debuffArme > 0) joueur->debuffArme--;

    do {
        afficherEtatCombat(joueur, ennemi);
        
        if (ennemi->debuffArme > 0) printf(COLOR_RED "[INFO] Armes ennemies endommagees (Dégâts réduits)\n" COLOR_RESET);
        if (ennemi->debuffMoteur > 0) printf(COLOR_RED "[INFO] Moteurs ennemis HS (Esquive nulle)\n" COLOR_RESET);
        if (joueur->debuffArme > 0) printf(COLOR_YELLOW "[INFO] Vos armes sont perturbees (puissance reduite)\n" COLOR_RESET);
        if (joueur->debuffMoteur > 0) printf(COLOR_YELLOW "[INFO] Vos moteurs sont instables (esquive reduite)\n" COLOR_RESET);

        // --- MENU ACTIONS ---
        const char *optionsAction[] = {
            "ATTAQUER -> Tirer sur coque/systemes",
            "RECHARGER SHIELD -> Stabiliser la defense",
            "TENTER LA FUITE -> Charger le FTL",
            "ANALYSER -> Scanner tactique detaille"
        };
        choixAction = lireMenuInteractif(COLOR_CYAN "\nVOTRE TOUR" COLOR_RESET,
                                         optionsAction,
                                         4,
                                         1,
                                         0);

        // --- 1. ATTAQUER ---
        if (choixAction == 1) {
            // Calculs Probabilités Affichage
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
            if (chanceSysteme > 100) chanceSysteme = 100;

            // Menu Cible
            char cible1[96], cible2[96], cible3[96];
            snprintf(cible1, sizeof(cible1), "Coque centrale [%d%%] Degats directs", chanceCoque);
            snprintf(cible2, sizeof(cible2), "Systeme armes [%d%%] Debuff puissance ennemie", chanceSysteme);
            snprintf(cible3, sizeof(cible3), "Systeme moteurs [%d%%] Debuff esquive ennemie", chanceSysteme);
            const char *optionsCible[] = { cible1, cible2, cible3 };
            choixCible = lireMenuInteractif(COLOR_BLUE "\nCIBLAGE DES ARMES" COLOR_RESET,
                                            optionsCible,
                                            3,
                                            1,
                                            1);
            if (choixCible == 0) continue; 

            // Menu Arme
            char arme2[96];
            snprintf(arme2, sizeof(arme2), "Missile (stock actuel: %d)", joueur->missiles);
            const char *optionsArme[] = {
                "Canon Laser (attaque standard)",
                arme2
            };
            choixArme = lireMenuInteractif(COLOR_BLUE "\nSELECTION D'ARME" COLOR_RESET,
                                           optionsArme,
                                           2,
                                           1,
                                           1);
            if (choixArme == 0) continue; 

            // EXECUTION TIR
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
                printf(COLOR_RED "\nLe tir passe a cote de la cible visée !\n" COLOR_RESET);
                SLEEP_MS(800);
            } else {
                int degats = joueur->systemeArme.efficacite;

                if (joueur->debuffArme > 0) {
                    degats = (degats + 1) / 2;
                    if (degats < 1) degats = 1;
                    printf(COLOR_YELLOW "[PERTURBATION] Vos armes perdent de la puissance ce tour.\n" COLOR_RESET);
                }

                // --- BONUS SOLDAT (Dégâts) ---
                int bonusSoldatDmg = getBonusDegats(joueur); // Dégât brut
                int bonusSoldatCrit = getBonusCritique(joueur); // % Critique (Nouveau)

                
                if (bonusSoldatDmg > 0) {
                    degats += bonusSoldatDmg;
                    printf(COLOR_RED "[SOLDAT] Tir optimisé ! (+%d Dégât)\n" COLOR_RESET, bonusSoldatDmg);
                }

                int estTouche = 0;

                if (choixArme == 2 && joueur->missiles > 0) {
                    joueur->missiles--;
                    degats += calculerDegats(3 + (joueur->distanceParcourue/5), joueur->moteurs, bonusSoldatCrit);
                    estTouche = 1;
                    printf("\n" COLOR_RED "MISSILE : Impact direct ! (-%d)" COLOR_RESET "\n", degats);
                } else if (choixArme == 2) {
                    printf("Click... Plus de missiles !\n");
                } else {
                    if (ennemi->bouclierActuel >= degats) {
                        ennemi->bouclierActuel -= degats;
                        printf(COLOR_CYAN "\nLe bouclier ennemi absorbe le tir.\n" COLOR_RESET);
                    } else {
                        degats = degats - ennemi->bouclierActuel;
                        ennemi->bouclierActuel = 0;
                        estTouche = 1;
                        printf(COLOR_RED "\nCoque touchée ! (-%d)\n" COLOR_RESET, degats);
                    }
                }

                if (estTouche) {
                    ennemi->coque -= degats;
                    if (choixCible == 2) {
                        ennemi->debuffArme = 2;
                        printf(COLOR_MAGENTA ">>> ARMES ENNEMIES ENDOMMAGEES ! <<<\n" COLOR_RESET);
                    }
                    if (choixCible == 3) {
                        ennemi->debuffMoteur = 2;
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
                printf(COLOR_BLUE "[SYSTEME] Recharge passive : +1 Bouclier.\n" COLOR_RESET);

                // Check du bonus "Double Recharge"
                int chanceBonus = getBonusRechargeBouclier(joueur); // Nouveau nom
                if (chanceBonus > 0 && (rand()%100 < chanceBonus) && joueur->bouclierActuel < bouclierMaxReel) {
                    joueur->bouclierActuel++;
                    printf(COLOR_CYAN "[INGENIEUR] Surcharge des condensateurs ! (+1 Extra)\n" COLOR_RESET);
                    
                    // XP Ingénieur
                    for(int i=0; i<3; i++) {
                        if(joueur->equipage[i].role == ROLE_INGENIEUR) gagnerXP(&joueur->equipage[i], 15);
                    }
                }
            }

            if (tenterReparationAutomatique(joueur)) {
                printf(COLOR_GREEN "[INGENIEUR] \"J'ai colmaté une brèche !\" (+1 Coque)\n" COLOR_RESET);
                SLEEP_MS(800);
            }


            tourFini = 1;
        }
        
        // --- 2. RECHARGER BOUCLIERS ---
        else if (choixAction == 2) {
            int regen = (rand() % 3) + 2; 
            int avant = joueur->bouclierActuel;
            joueur->bouclierActuel += regen;
            if (joueur->bouclierActuel > joueur->systemeBouclier.efficacite) {
                joueur->bouclierActuel = joueur->systemeBouclier.efficacite;
            }
            int gainReel = joueur->bouclierActuel - avant;

            printf(COLOR_BLUE "\n[MANOEUVRE] Energie détournée vers les boucliers !\n" COLOR_RESET);
            printf("Récupération de " COLOR_CYAN "+%d barres" COLOR_RESET " (Total: %d/%d)\n", 
                gainReel, joueur->bouclierActuel, joueur->systemeBouclier.efficacite);
            
            // --- BONUS INGENIEUR (Actif) ---
            int chanceBonus = getBonusRechargeBouclier(joueur);
            if (chanceBonus > 0 && (rand()%100 < chanceBonus) && joueur->bouclierActuel < joueur->systemeBouclier.efficacite) {
                joueur->bouclierActuel++;
                printf(COLOR_CYAN "[INGENIEUR] Surcharge des boucliers ! (+1 Extra)\n" COLOR_RESET);
                for(int i=0; i<3; i++) {
                    if(joueur->equipage[i].role == ROLE_INGENIEUR) gagnerXP(&joueur->equipage[i], 20);
                }
            }

            // NOUVEAU CALCUL DU MAX
            int maxTotal = joueur->systemeBouclier.efficacite + getBonusCapaciteBouclier(joueur);
            
            if (joueur->bouclierActuel > maxTotal) {
                joueur->bouclierActuel = maxTotal;
            }

            SLEEP_MS(1000);
            tourFini = 1;
        }
        
        // --- 3. FUITE ---
        else if (choixAction == 3) {
            if (estBossFinal) {
                printf(COLOR_RED "\n[ERREUR] Le vaisseau mère génère un champ inhibiteur ! Saut FTL impossible !\n" COLOR_RESET);
                SLEEP_MS(2000);
            } else {
                if (joueur->debuffMoteur > 0 && (rand() % 100) < 35) {
                    printf(COLOR_RED "\n[MOTEURS] Instabilite moteur: le chargement FTL echoue ce tour.\n" COLOR_RESET);
                    SLEEP_MS(1000);
                    tourFini = 1;
                    continue;
                }

                joueur->chargeFTL++;
                printf(COLOR_YELLOW "\nChargement FTL...\n" COLOR_RESET);

                // --- BONUS PILOTE (Vitesse FTL) ---
                int chanceTurbo = getBonusVitesseFTL(joueur);
                if (chanceTurbo > 0 && (rand() % 100) < chanceTurbo) {
                    joueur->chargeFTL++;
                    printf(COLOR_CYAN "[PILOTE] Calcul de trajectoire optimisé ! (+1 Charge Extra)\n" COLOR_RESET);
                    
                    // XP Pilote pour la fuite réussie
                    for(int i=0; i<3; i++) {
                        if(joueur->equipage[i].role == ROLE_PILOTE) gagnerXP(&joueur->equipage[i], 10);
                    }
                }
                
                if (joueur->chargeFTL >= joueur->maxchargeFTL) { ennemi->coque = 0; return; }
                tourFini = 1;
            }
        } 
        
        // --- 4. ANALYSER ---
        else if (choixAction == 4) {
            analyserEnnemi(joueur, ennemi);
            attendreJoueur();
        }

    } while(!tourFini);

    SLEEP_MS(500);

    // --- TOUR DE L'ENNEMI ---
    if (ennemi->coque > 0) {
        int d = normaliserDifficulte(joueur->difficulte);
        int chanceFuite = (d == DIFFICULTE_FACILE) ? 70 : (d == DIFFICULTE_DIFFICILE ? 30 : 50);
        int bonusPrecisionIA = (d == DIFFICULTE_FACILE) ? -8 : (d == DIFFICULTE_DIFFICILE ? 8 : 0);
        int chanceRechargeIA = (d == DIFFICULTE_FACILE) ? 28 : (d == DIFFICULTE_DIFFICILE ? 12 : 20);
        int chanceSabotage = (d == DIFFICULTE_FACILE) ? 6 : (d == DIFFICULTE_DIFFICILE ? 22 : 12);
        int multiDegats = (d == DIFFICULTE_FACILE) ? 85 : (d == DIFFICULTE_DIFFICILE ? 120 : 100);

        printf("\nL'ennemi evalue la situation tactique...");
        SLEEP_MS(800);

        // --- BONUS PILOTE (Esquive) ---
        int bonusPilote = getBonusEsquive(joueur);
        int esquiveJoueur = 10 + (joueur->moteurs * 5) + bonusPilote;
        if (joueur->debuffMoteur > 0) {
            esquiveJoueur -= 20;
            if (esquiveJoueur < 0) esquiveJoueur = 0;
        }

        if (bonusPilote > 0) {
            printf(COLOR_CYAN "[PILOTE] Manoeuvre evasive ! (+%d%% Esquive)\n" COLOR_RESET, bonusPilote);
        }

        // Fuite ennemie selon le risque
        if (!estBossFinal && ennemi->debuffMoteur == 0 && ennemi->coque * 100 <= ennemi->coqueMax * 30 && (rand() % 100) < chanceFuite) {
            ennemi->chargeFTL++;
            printf(COLOR_RED "\n[ALERTE] L'ennemi charge son FTL pour fuir !\n" COLOR_RESET);
            if (ennemi->chargeFTL >= ennemi->maxchargeFTL) {
                ennemi->coque = 0; return;
            }
            goto fin_tour_ennemi;
        }

        // Decision defensive: recharge bouclier
        if (ennemi->bouclierActuel < ennemi->systemeBouclier.efficacite && (rand() % 100) < chanceRechargeIA) {
            int recharge = 1 + ((d == DIFFICULTE_DIFFICILE && (rand() % 100) < 30) ? 1 : 0);
            ennemi->bouclierActuel += recharge;
            if (ennemi->bouclierActuel > ennemi->systemeBouclier.efficacite) {
                ennemi->bouclierActuel = ennemi->systemeBouclier.efficacite;
            }
            printf(COLOR_BLUE "\n[IA] L'ennemi redirige l'energie vers ses boucliers (+%d).\n" COLOR_RESET, recharge);
            goto fin_tour_ennemi;
        }

        // --- RÉSOLUTION TIR ENNEMI ---
            if (checkEsquive(esquiveJoueur - bonusPrecisionIA, ennemi)) {
            printf(COLOR_GREEN "\nESQUIVE ! Vous évitez le tir.\n" COLOR_RESET);

            for(int i=0; i<3; i++) {
                if(joueur->equipage[i].role == ROLE_PILOTE && joueur->equipage[i].estVivant) {
                    gagnerXP(&joueur->equipage[i], 15);
                }
            }
        } else {
            int degatsEntrants = ennemi->systemeArme.efficacite;
            
            if (ennemi->debuffArme > 0) {
                degatsEntrants = degatsEntrants / 2;
                printf(COLOR_YELLOW "\n[CHANCE] Les armes endommagées de l'ennemi tirent faiblement !\n" COLOR_RESET);
            }

            degatsEntrants = (degatsEntrants * multiDegats) / 100;
            if (degatsEntrants < 1) degatsEntrants = 1;
            degatsEntrants = calculerDegats(degatsEntrants, ennemi->moteurs, 0);

            if (joueur->bouclierActuel >= degatsEntrants) {
                joueur->bouclierActuel -= degatsEntrants;
                printf(COLOR_CYAN "\nBouclier tient bon (-%d).\n" COLOR_RESET, degatsEntrants);
            } else {
                int surplus = degatsEntrants - joueur->bouclierActuel;
                joueur->bouclierActuel = 0;
                joueur->coque -= surplus;
                printf(COLOR_RED "\nALERTE ! Dégâts coque subis: -%d\n" COLOR_RESET, surplus);

                // Occasionnellement, l'IA cible vos systèmes sur les difficultés élevées.
                if ((rand() % 100) < chanceSabotage) {
                    if ((rand() % 2) == 0) {
                        joueur->debuffArme = 2;
                        printf(COLOR_RED "[IA] Vos armes sont touchées: puissance réduite pour 2 tours.\n" COLOR_RESET);
                    } else {
                        joueur->debuffMoteur = 2;
                        printf(COLOR_RED "[IA] Vos moteurs sont touchés: esquive réduite pour 2 tours.\n" COLOR_RESET);
                    }
                }

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
    int esquiveFinale = chanceEsquive;
    if (attaquant != NULL) {
        esquiveFinale -= attaquant->precision;
    }
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

    printf("\n" COLOR_GREEN "╔════════════════════ SCANNER TACTIQUE ════════════════════╗\n" COLOR_RESET);
    printf(" Cible        : " COLOR_BOLD "%-28s" COLOR_RESET "\n", ennemi->nom);
    printf(" Arme         : " COLOR_RED "%-28s" COLOR_RESET " Puissance: %d\n",
           ennemi->systemeArme.nom, ennemi->systemeArme.efficacite);
    printf(" Bouclier     : " COLOR_CYAN "%-28s" COLOR_RESET " Charge max: %d\n",
           ennemi->systemeBouclier.nom, ennemi->systemeBouclier.efficacite);
    printf(" Moteurs      : " COLOR_YELLOW "Niveau %d" COLOR_RESET "\n", ennemi->moteurs);

    if (ennemi->debuffArme > 0) {
        printf(" Statut armes : " COLOR_RED "ENDOMMAGEES (%d tours restants)" COLOR_RESET "\n", ennemi->debuffArme);
    } else {
        printf(" Statut armes : " COLOR_GREEN "OPERATIONNELLES" COLOR_RESET "\n");
    }

    if (ennemi->debuffMoteur > 0) {
        printf(" Statut moteur: " COLOR_RED "HS (%d tours restants)" COLOR_RESET "\n", ennemi->debuffMoteur);
        printf(" Precision    : " COLOR_GREEN "Chance de toucher: 100%%" COLOR_RESET "\n");
    } else {
        int chanceToucher = 100 - esquiveReelle;
        if (chanceToucher < 0) chanceToucher = 0;
        if (chanceToucher > 100) chanceToucher = 100;
        printf(" Esquive cible: %d%% (precision joueur: -%d%%)\n", esquiveBase, joueur->precision);
        printf(" Precision    : " COLOR_BOLD "%d%% de chance de toucher" COLOR_RESET "\n", chanceToucher);
    }

    if (ennemi->systemeArme.rang > joueur->systemeArme.rang || 
        ennemi->systemeBouclier.rang > joueur->systemeBouclier.rang) {
        printf("\n" COLOR_MAGENTA " [!] Technologie superieure detectee: loot possible" COLOR_RESET "\n");
    } else {
        printf("\n" COLOR_WHITE " [i] Aucun module notable a recuperer" COLOR_RESET "\n");
    }
    printf(COLOR_GREEN "╚═══════════════════════════════════════════════════════════╝" COLOR_RESET "\n");
}
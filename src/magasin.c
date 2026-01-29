#include "magasin.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Macro pour l'inflation
#define PRIX_INFLATION(base, fatigue) ((base) + ((fatigue) * 10))

// --- STRUCTURE LOCALE POUR LES RECRUES ---
typedef struct {
    char nom[30];
    TypeRole role;
    int pv;
    int pvMax;
    int prix;
} Candidat;

void ouvrirMagasin(Vaisseau *joueur) {
    int categorie = 0;
    
    // --- 1. GÉNÉRATION DÉTERMINISTE ---
    // On utilise XOR (^) au lieu de (+) pour mélanger les bits, 
    // et on multiplie par de grands nombres premiers pour casser la linéarité.
    unsigned int seedMagasin = (joueur->seedSecteur ^ 0x5DEECE66D) * ((joueur->distanceParcourue + 123) * 65537);
    srand(seedMagasin);
    
    // "Chauffage" du générateur : on tire quelques nombres dans le vide
    // pour s'éloigner de la graine initiale.
    int chauffe = (rand() % 10) + 5;
    for(int k=0; k < chauffe; k++) rand(); 

    // --- 2. GÉNÉRATION DES STOCKS ET PROMOS ---
    int stockMissiles = (rand() % 4) + 2;
    int stockCarburant = (rand() % 6) + 5;

    // Promo
    int idPromo = (rand() % 5) + 1; 
    int pourcentPromo = (rand() % 31) + 20;

    // --- GÉNÉRATION DES CANDIDATS ---
    Candidat recrues[3];
    // Note : Tu as 23 noms dans ta liste, j'ai corrigé le modulo (23 au lieu de 21)
    char *noms[] = {"Mercredi", "Krog","Zorp", "Lyla", "Brack", "Neo", "Kait", 
                    "Jinx", "Rook", "Mordin", "Vex", "Lydia", "Hélydia", "Ismael", 
                    "Tara", "Orin", "Sable", "Dax", "Vera", "Kiro", "Zane", "Mira", "Ryn"};
    int nbNoms = 23; 

    for(int i=0; i<3; i++) {
        strcpy(recrues[i].nom, noms[rand() % nbNoms]); // Modulo corrigé
        recrues[i].role = (rand() % 3); 
        recrues[i].pvMax = 100;
        
        if (i == 1) recrues[i].pv = 30 + (rand() % 40); 
        else recrues[i].pv = 100;

        recrues[i].prix = (50 * recrues[i].pv) / recrues[i].pvMax;
    }

    // --- RETOUR À L'ALÉATOIRE NORMAL ---
    srand(time(NULL));
    
    int nombreAchats = 0;
    const int SEUIL_AVANT_INFLATION = 2; 

    while (categorie != 5) { // Quitter est passé à 5
        effacerEcran();

        int fatigueIngenieur = 0;
        if (nombreAchats >= SEUIL_AVANT_INFLATION) {
            fatigueIngenieur = nombreAchats - SEUIL_AVANT_INFLATION + 1;
        }
        
        printf(COLOR_GREEN "╔══════════════════════════════════════════════════════════╗\n");
        printf("║ " COLOR_BOLD "🛒 DOCK COMMERCIAL" COLOR_RESET COLOR_GREEN "          CRÉDITS: " COLOR_YELLOW "%-7d ⚓" COLOR_GREEN "          ║\n", joueur->ferraille);
        printf("╠══════════════════════════════════════════════════════════╣" COLOR_RESET "\n");

        if (fatigueIngenieur > 0) {
            printf("║ " COLOR_RED "⚠ TARIFS MAJORÉS : Demande élevée (+%-2d fer/article)" COLOR_GREEN "      ║\n", fatigueIngenieur * 10);
        } else {
            int reste = SEUIL_AVANT_INFLATION - nombreAchats;
            if (reste > 0) printf("║ " COLOR_CYAN "ℹ OFFRE : Encore %-2d achat(s) à prix coûtant !" COLOR_GREEN "            ║\n", reste);
            else printf("║ " COLOR_YELLOW "⚠ ATTENTION : Prochain achat déclenche l'inflation." COLOR_GREEN "     ║\n");
        }
        
        // Affichage Promo
        char nomPromo[30];
        if (idPromo == 1) strcpy(nomPromo, "ARMES");
        else if (idPromo == 2) strcpy(nomPromo, "BOUCLIERS");
        else if (idPromo == 3) strcpy(nomPromo, "MOTEURS");
        else if (idPromo == 4) strcpy(nomPromo, "COQUE");
        else strcpy(nomPromo, "VISÉE");

        printf("║ " COLOR_MAGENTA "★ PROMO FLASH : -%d%% sur : %-20s" COLOR_GREEN "           ║\n", pourcentPromo, nomPromo);
        
        printf("╠══════════════════════════════════════════════════════════╣" COLOR_RESET "\n");
        printf(COLOR_GREEN "║ " COLOR_CYAN "1. [MAINTENANCE]" COLOR_RESET "  Réparations & Munitions                " COLOR_GREEN "║\n");
        printf(COLOR_GREEN "║ " COLOR_RED "2. [UPGRADES]   " COLOR_RESET "  Systèmes du Vaisseau                   " COLOR_GREEN "║\n");
        printf(COLOR_GREEN "║ " COLOR_YELLOW "3. [SERVICES]   " COLOR_RESET "  Marché noir & Recyclage                " COLOR_GREEN "║\n");
        printf(COLOR_GREEN "║ " COLOR_MAGENTA "4. [PERSONNEL]  " COLOR_RESET "  Recrutement & Gestion RH               " COLOR_GREEN "║\n");
        printf(COLOR_GREEN "║ " COLOR_RESET "5. [QUITTER]    " COLOR_RESET "  Reprendre la navigation                " COLOR_GREEN "║\n");
        printf(COLOR_GREEN "╚══════════════════════════════════════════════════════════╝" COLOR_RESET "\n");
        
        printf("\n " COLOR_YELLOW "Choisir catégorie > " COLOR_RESET);
        if (scanf("%d", &categorie) != 1) {
            int c; while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }

        // --- 1. MAINTENANCE ---
        if (categorie == 1) {
            int choix = 0;
            printf("\n" COLOR_CYAN "─── MAINTENANCE ───" COLOR_RESET "\n");
            printf("1. Réparer Coque (+5)  | 10 Fer. | (Besoin: %d)\n", joueur->coqueMax - joueur->coque);
            printf("2. Missiles (+3)       | 15 Fer. | Stock: %d\n", stockMissiles);
            printf("3. Carburant (x1)      | 05 Fer. | Stock: %d\n", stockCarburant);
            printf("4. Retour\n > ");
            scanf("%d", &choix);

            if (choix == 1 && joueur->ferraille >= 10 && joueur->coque < joueur->coqueMax) {
                joueur->ferraille -= 10;
                joueur->coque = (joueur->coque + 5 > joueur->coqueMax) ? joueur->coqueMax : joueur->coque + 5;
                printf(COLOR_GREEN "✔ Coque réparée.\n" COLOR_RESET);
            } 
            else if (choix == 2 && stockMissiles > 0 && joueur->ferraille >= 15) {
                joueur->ferraille -= 15; joueur->missiles += 3; stockMissiles--;
            } 
            else if (choix == 3 && stockCarburant > 0 && joueur->ferraille >= 5) {
                joueur->ferraille -= 5; joueur->carburant += 1; stockCarburant--;
            }
        } 
        
        // --- 2. UPGRADES ---
        else if (categorie == 2) {
            int choix = 0;
            printf("\n" COLOR_RED "─── ATELIER D'INGÉNIERIE ───" COLOR_RESET "\n");

            // 1. PRIX BASE
            int baseArme = 20 * (joueur->systemeArme.rang + 1);
            int baseBouclier = 25 * (joueur->systemeBouclier.rang + 1);
            int baseMoteur = 15 * (joueur->moteurs + 1);
            int baseCoque = 25 + (joueur->coqueMax / 3); 
            int baseVisee = 20 + (joueur->precision);

            // 2. PROMO
            int isPromo[6] = {0}; 
            if (idPromo == 1) { baseArme = baseArme * (100 - pourcentPromo) / 100; isPromo[1] = 1; }
            if (idPromo == 2) { baseBouclier = baseBouclier * (100 - pourcentPromo) / 100; isPromo[2] = 1; }
            if (idPromo == 3) { baseMoteur = baseMoteur * (100 - pourcentPromo) / 100; isPromo[3] = 1; }
            if (idPromo == 4) { baseCoque = baseCoque * (100 - pourcentPromo) / 100; isPromo[4] = 1; }
            if (idPromo == 5) { baseVisee = baseVisee * (100 - pourcentPromo) / 100; isPromo[5] = 1; }

            // 3. INFLATION
            int prixArme = PRIX_INFLATION(baseArme, fatigueIngenieur);
            int prixBouclier = PRIX_INFLATION(baseBouclier, fatigueIngenieur);
            int prixMoteur = PRIX_INFLATION(baseMoteur, fatigueIngenieur);
            int prixCoque = PRIX_INFLATION(baseCoque, fatigueIngenieur);
            int prixVisee = PRIX_INFLATION(baseVisee, fatigueIngenieur);

            printf("1. Upgrade %-15s (-> Mk %d) | ", joueur->systemeArme.nom, joueur->systemeArme.rang + 1);
            if (isPromo[1]) printf(COLOR_MAGENTA "(-%d%%) " COLOR_RESET, pourcentPromo);
            printf("%d Fer.\n", prixArme);

            printf("2. Upgrade %-15s (-> Mk %d) | ", joueur->systemeBouclier.nom, joueur->systemeBouclier.rang + 1);
            if (isPromo[2]) printf(COLOR_MAGENTA "(-%d%%) " COLOR_RESET, pourcentPromo);
            printf("%d Fer.\n", prixBouclier);

            printf("3. Moteurs (-> Esquive Lvl %d)           | ", joueur->moteurs + 1);
            if (isPromo[3]) printf(COLOR_MAGENTA "(-%d%%) " COLOR_RESET, pourcentPromo);
            printf("%d Fer.\n", prixMoteur);

            printf("4. Renforcer Coque (+10 Max)               | ");
            if (isPromo[4]) printf(COLOR_MAGENTA "(-%d%%) " COLOR_RESET, pourcentPromo);
            printf("%d Fer.\n", prixCoque);
            
            printf("5. Système de Visée (+5 Precision)         | ");
            if (isPromo[5]) printf(COLOR_MAGENTA "(-%d%%) " COLOR_RESET, pourcentPromo);
            printf("%d Fer.\n", prixVisee);
            
            printf("6. Retour\n > ");
            
            scanf("%d", &choix);

            int achatEffectue = 0;
            if (choix == 1 && joueur->ferraille >= prixArme) {
                joueur->ferraille -= prixArme; ameliorerArme(joueur); achatEffectue = 1;
            } 
            else if (choix == 2 && joueur->ferraille >= prixBouclier) {
                joueur->ferraille -= prixBouclier; ameliorerBouclier(joueur); achatEffectue = 1;
            }
            else if (choix == 3 && joueur->ferraille >= prixMoteur) {
                joueur->ferraille -= prixMoteur; joueur->moteurs++;
                printf(COLOR_GREEN "✔ Moteurs optimisés.\n" COLOR_RESET); achatEffectue = 1;
            }
            else if (choix == 4 && joueur->ferraille >= prixCoque) {
                joueur->ferraille -= prixCoque; joueur->coqueMax += 10; joueur->coque += 10;
                printf(COLOR_GREEN "✔ Structure renforcée.\n" COLOR_RESET); achatEffectue = 1;
            }
            else if (choix == 5 && joueur->ferraille >= prixVisee) {
                joueur->ferraille -= prixVisee; joueur->precision += 5;
                printf(COLOR_GREEN "✔ Ordinateur de visée mis à jour.\n" COLOR_RESET); achatEffectue = 1;
            }

            if (achatEffectue) {
                nombreAchats++;
                if (nombreAchats == SEUIL_AVANT_INFLATION) {
                    printf(COLOR_YELLOW "⚠ Fin des tarifs préférentiels. Inflation au prochain achat !\n" COLOR_RESET);
                    SLEEP_MS(1200);
                } else if (nombreAchats > SEUIL_AVANT_INFLATION) {
                    printf(COLOR_RED "⚠ Demande forte : Les prix augmentent encore !\n" COLOR_RESET);
                    SLEEP_MS(800);
                }
            }
        }
        
        // --- 3. SERVICES ---
        else if (categorie == 3) {
            int choix = 0;
            printf("\n" COLOR_YELLOW "─── SERVICES DU MARCHÉ NOIR ───" COLOR_RESET "\n");
            printf("1. Vendre 1 Carburant (+4 Ferraille)\n");
            printf("2. Recycler Arme Actuelle (Gain : Rang * 12)\n"); 
            printf("3. Retour\n > ");
            scanf("%d", &choix);

            if (choix == 1 && joueur->carburant > 0) {
                joueur->carburant--; joueur->ferraille += 4;
            } else if (choix == 2) {
                int gain = joueur->systemeArme.rang * 12;
                joueur->ferraille += gain;
                strcpy(joueur->systemeArme.nom, "Laser Rouillé");
                joueur->systemeArme.rang = 0;
                joueur->systemeArme.efficacite = 1;
                printf(COLOR_GREEN "✔ Arme recyclée pour %d Ferraille.\n" COLOR_RESET, gain);
            }
        }

        // --- 4. PERSONNEL (NOUVEAU) ---
        else if (categorie == 4) {
            int choix = 0;
            printf("\n" COLOR_MAGENTA "─── BUREAU DE RECRUTEMENT & RH ───" COLOR_RESET "\n");
            printf("1. Recruter un mercenaire (Achat)\n");
            printf("2. Renvoyer un membre (Vente / Recyclage)\n");
            printf("3. Retour\n > ");
            scanf("%d", &choix);

            // ACHAT DE PERSONNEL
            if (choix == 1) {
                // Vérifier s'il y a de la place
                int slotLibre = -1;
                for(int i=0; i<3; i++) {
                    if (!joueur->equipage[i].estVivant) {
                        slotLibre = i; break;
                    }
                }

                if (slotLibre == -1) {
                    printf(COLOR_RED "Votre vaisseau est complet (3/3). Renvoyez quelqu'un d'abord.\n" COLOR_RESET);
                } else {
                    printf("\n--- CANDIDATS DISPONIBLES ---\n");
                    for(int i=0; i<3; i++) {
                        printf("%d. %-10s [%-9s] Santé:%3d%% | Prix: " COLOR_YELLOW "%2d Fer" COLOR_RESET "\n", 
                               i+1, recrues[i].nom, getRoleNom(recrues[i].role), recrues[i].pv, recrues[i].prix);
                    }
                    printf("0. Annuler\n> ");
                    int recrueChoix;
                    scanf("%d", &recrueChoix);

                    if (recrueChoix >= 1 && recrueChoix <= 3) {
                        Candidat *c = &recrues[recrueChoix-1];
                        if (joueur->ferraille >= c->prix) {
                            joueur->ferraille -= c->prix;
                            
                            // Affectation
                            strcpy(joueur->equipage[slotLibre].nom, c->nom);
                            joueur->equipage[slotLibre].role = c->role;
                            joueur->equipage[slotLibre].pv = c->pv;
                            joueur->equipage[slotLibre].pvMax = c->pvMax;
                            joueur->equipage[slotLibre].estVivant = 1;
                            joueur->equipage[slotLibre].xp = 0;
                            joueur->equipage[slotLibre].niveau = 0;
                            
                            joueur->nbMembres++;
                            printf(COLOR_GREEN "Bienvenue à bord, %s !\n" COLOR_RESET, c->nom);
                        } else {
                            printf(COLOR_RED "Fonds insuffisants.\n" COLOR_RESET);
                        }
                    }
                }
            }
            
            // VENTE DE PERSONNEL
            else if (choix == 2) {
                printf("\n--- GESTION DES EFFECTIFS ---\n");
                for(int i=0; i<3; i++) {
                    Membre *m = &joueur->equipage[i];
                    
                    // Calcul Prix de Vente
                    int prixVente = 0;
                    if (m->estVivant) {
                        // Vente Vivant : Max 25, proportionnel à la santé
                        prixVente = (25 * m->pv) / m->pvMax;
                    } else if (strcmp(m->nom, "--- LIBRE ---") != 0) {
                        // Vente Mort : Prix fixe recyclage
                        prixVente = 5; 
                    }

                    // Affichage
                    if (m->estVivant) {
                         if (i == 0) {
                             printf("%d. %-12s (COMMANDANT) - " COLOR_RED "INVENDABLE" COLOR_RESET "\n", i+1, m->nom);
                         } else {
                             printf("%d. %-12s [%-9s] Santé:%3d%% - Vente: " COLOR_YELLOW "%d Fer" COLOR_RESET "\n", 
                                i+1, m->nom, getRoleNom(m->role), m->pv, prixVente);
                         }
                    } else if (strcmp(m->nom, "--- LIBRE ---") != 0) {
                        printf("%d. [CADAVRE]    %-12s          - Recyclage: " COLOR_YELLOW "%d Fer" COLOR_RESET "\n", 
                               i+1, m->nom, prixVente);
                    } else {
                        printf("%d. --- LIBRE ---\n", i+1);
                    }
                }
                
                printf("Entrez le numéro du membre à renvoyer (0 pour Annuler) > ");
                int renvoi;
                scanf("%d", &renvoi);

                if (renvoi > 1 && renvoi <= 3) { // On interdit de vendre le slot 1 (Commandant)
                    int idx = renvoi - 1;
                    Membre *m = &joueur->equipage[idx];
                    
                    if (strcmp(m->nom, "--- LIBRE ---") == 0) {
                        printf("Ce slot est déjà vide.\n");
                    } else {
                        int gain = 0;
                        if (m->estVivant) {
                            gain = (25 * m->pv) / m->pvMax;
                            printf(COLOR_YELLOW "Vous avez renvoyé %s. Gain : %d Ferrailles.\n" COLOR_RESET, m->nom, gain);
                        } else {
                            gain = 5;
                            printf(COLOR_CYAN "Biomatière de %s recyclée. Gain : %d Ferrailles.\n" COLOR_RESET, m->nom, gain);
                        }
                        
                        joueur->ferraille += gain;
                        // Reset du slot
                        strcpy(m->nom, "--- LIBRE ---");
                        m->estVivant = 0;
                        m->pv = 0;
                        m->xp = 0;
                        m->niveau = 0;
                        joueur->nbMembres--;
                    }
                } else if (renvoi == 1) {
                    printf(COLOR_RED "Impossible de vendre le Commandant ! Qui piloterait ?\n" COLOR_RESET);
                }
            }
        }
        
        if (categorie == 5) sauvegarderPartie(joueur); // Sauvegarde en quittant
        if (categorie != 5) SLEEP_MS(800);
    }
}

void ameliorerArme(Vaisseau *v) {
    v->systemeArme.rang++;
    v->systemeArme.efficacite += 2; 
    snprintf(v->systemeArme.nom, 50, "Laser Pulse Mk %d", v->systemeArme.rang);
    printf(COLOR_GREEN "✔ Votre arme est maintenant : %s !\n" COLOR_RESET, v->systemeArme.nom);
}

void ameliorerBouclier(Vaisseau *v) {
    v->systemeBouclier.rang++;
    v->systemeBouclier.efficacite += 1; 
    v->bouclierActuel = v->systemeBouclier.efficacite; 
    snprintf(v->systemeBouclier.nom, 50, "Bouclier Ion Mk %d", v->systemeBouclier.rang);
    printf(COLOR_GREEN "✔ Nouveau système installé : %s !\n" COLOR_RESET, v->systemeBouclier.nom);
}
#include "magasin.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // Nécessaire pour time(NULL)

// Macro pour l'inflation : Prix de base + (Fatigue * 10)
#define PRIX_INFLATION(base, fatigue) ((base) + ((fatigue) * 10))

void ouvrirMagasin(Vaisseau *joueur) {
    int categorie = 0;
    
    // --- 1. INITIALISATION DU GÉNÉRATEUR ANTI-TRICHE ---
    // On crée une graine unique basée sur ta sauvegarde (seedSecteur) 
    // et l'endroit où tu es (distanceParcourue).
    // Résultat : Le magasin sera TOUJOURS le même à ce secteur précis.
    unsigned int seedMagasin = joueur->seedSecteur + (joueur->distanceParcourue * 4242);
    srand(seedMagasin);
    
    // --- 2. GÉNÉRATION DES STOCKS ET PROMOS ---
    int stockMissiles = (rand() % 4) + 2;
    int stockCarburant = (rand() % 6) + 5;

    // Gestion de la PROMO UNIQUE (Coup de Fusil)
    int idPromo = (rand() % 5) + 1; 
    int pourcentPromo = (rand() % 31) + 20; // Entre 20% et 50%

    // --- 3. RETOUR À L'ALÉATOIRE NORMAL ---
    // On remet le hasard sur l'horloge pour que les combats suivants restent imprévisibles
    srand(time(NULL));
    
    // Gestion de l'inflation
    int nombreAchats = 0;
    const int SEUIL_AVANT_INFLATION = 2; // Les 2 premiers achats sont au prix normal

    while (categorie != 4) {
        effacerEcran();

        // Calcul de la fatigue actuelle (0 tant qu'on n'a pas dépassé le seuil)
        int fatigueIngenieur = 0;
        if (nombreAchats >= SEUIL_AVANT_INFLATION) {
            fatigueIngenieur = nombreAchats - SEUIL_AVANT_INFLATION + 1;
        }
        
        printf(COLOR_GREEN "╔══════════════════════════════════════════════════════════╗\n");
        printf("║ " COLOR_BOLD "🛒 DOCK COMMERCIAL" COLOR_RESET COLOR_GREEN "          CRÉDITS: " COLOR_YELLOW "%-7d ⚓" COLOR_GREEN "          ║\n", joueur->ferraille);
        printf("╠══════════════════════════════════════════════════════════╣" COLOR_RESET "\n");

        // Message d'ambiance selon l'inflation
        if (fatigueIngenieur > 0) {
            printf("║ " COLOR_RED "⚠ TARIFS MAJORÉS : Demande élevée (+%-2d fer/article)" COLOR_GREEN "      ║\n", fatigueIngenieur * 10);
        } else {
            // Affiche combien d'achats restent avant hausse
            int reste = SEUIL_AVANT_INFLATION - nombreAchats;
            if (reste > 0)
                printf("║ " COLOR_CYAN "ℹ OFFRE : Encore %-2d achat(s) à prix coûtant !" COLOR_GREEN "            ║\n", reste);
            else
                printf("║ " COLOR_YELLOW "⚠ ATTENTION : Prochain achat déclenche l'inflation." COLOR_GREEN "     ║\n");
        }
        
        // Affichage de la Promo du jour
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
        printf(COLOR_GREEN "║ " COLOR_RESET "4. [QUITTER]    " COLOR_RESET "  Reprendre la navigation                " COLOR_GREEN "║\n");
        printf(COLOR_GREEN "╚══════════════════════════════════════════════════════════╝" COLOR_RESET "\n");
        
        printf("\n " COLOR_YELLOW "Choisir catégorie > " COLOR_RESET);
        if (scanf("%d", &categorie) != 1) {
            int c; while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }

        // --- CATEGORIE 1 : MAINTENANCE (Prix fixes et bas) ---
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
        
        // --- CATEGORIE 2 : UPGRADES (Prix bas + Promo ciblée) ---
        else if (categorie == 2) {
            int choix = 0;
            printf("\n" COLOR_RED "─── ATELIER D'INGÉNIERIE ───" COLOR_RESET "\n");

            // 1. PRIX DE BASE
            int baseArme = 20 * (joueur->systemeArme.rang + 1);
            int baseBouclier = 25 * (joueur->systemeBouclier.rang + 1);
            int baseMoteur = 15 * (joueur->moteurs + 1);
            int baseCoque = 25 + (joueur->coqueMax / 3); 
            int baseVisee = 20 + (joueur->precision);

            // 2. APPLICATION DE LA PROMO CIBLÉE
            int isPromo[6] = {0}; 
            
            if (idPromo == 1) { baseArme = baseArme * (100 - pourcentPromo) / 100; isPromo[1] = 1; }
            if (idPromo == 2) { baseBouclier = baseBouclier * (100 - pourcentPromo) / 100; isPromo[2] = 1; }
            if (idPromo == 3) { baseMoteur = baseMoteur * (100 - pourcentPromo) / 100; isPromo[3] = 1; }
            if (idPromo == 4) { baseCoque = baseCoque * (100 - pourcentPromo) / 100; isPromo[4] = 1; }
            if (idPromo == 5) { baseVisee = baseVisee * (100 - pourcentPromo) / 100; isPromo[5] = 1; }

            // 3. APPLICATION DE L'INFLATION
            int prixArme = PRIX_INFLATION(baseArme, fatigueIngenieur);
            int prixBouclier = PRIX_INFLATION(baseBouclier, fatigueIngenieur);
            int prixMoteur = PRIX_INFLATION(baseMoteur, fatigueIngenieur);
            int prixCoque = PRIX_INFLATION(baseCoque, fatigueIngenieur);
            int prixVisee = PRIX_INFLATION(baseVisee, fatigueIngenieur);

            // --- AFFICHAGE DU MENU ---
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

            // --- TRAITEMENT DES ACHATS ---
            int achatEffectue = 0;

            if (choix == 1) {
                if (joueur->ferraille >= prixArme) {
                    joueur->ferraille -= prixArme;
                    ameliorerArme(joueur);
                    achatEffectue = 1;
                } else printf(COLOR_RED "Fonds insuffisants !\n" COLOR_RESET);
            } 
            else if (choix == 2) {
                if (joueur->ferraille >= prixBouclier) {
                    joueur->ferraille -= prixBouclier;
                    ameliorerBouclier(joueur);
                    achatEffectue = 1;
                } else printf(COLOR_RED "Fonds insuffisants !\n" COLOR_RESET);
            }
            else if (choix == 3) {
                if (joueur->ferraille >= prixMoteur) {
                    joueur->ferraille -= prixMoteur;
                    joueur->moteurs++;
                    printf(COLOR_GREEN "✔ Moteurs optimisés.\n" COLOR_RESET);
                    achatEffectue = 1;
                } else printf(COLOR_RED "Fonds insuffisants !\n" COLOR_RESET);
            }
            else if (choix == 4) {
                if (joueur->ferraille >= prixCoque) {
                    joueur->ferraille -= prixCoque;
                    joueur->coqueMax += 10;
                    joueur->coque += 10;
                    printf(COLOR_GREEN "✔ Structure renforcée.\n" COLOR_RESET);
                    achatEffectue = 1;
                } else printf(COLOR_RED "Fonds insuffisants !\n" COLOR_RESET);
            }
            else if (choix == 5) {
                if (joueur->ferraille >= prixVisee) {
                    joueur->ferraille -= prixVisee;
                    joueur->precision += 5;
                    printf(COLOR_GREEN "✔ Ordinateur de visée mis à jour.\n" COLOR_RESET);
                    achatEffectue = 1;
                } else printf(COLOR_RED "Fonds insuffisants !\n" COLOR_RESET);
            }

            // Si un achat a eu lieu, on incrémente le compteur
            if (achatEffectue) {
                nombreAchats++;
                
                if (nombreAchats == SEUIL_AVANT_INFLATION) {
                    printf(COLOR_YELLOW "⚠ Fin des tarifs préférentiels. L'inflation s'appliquera au prochain achat !\n" COLOR_RESET);
                    SLEEP_MS(1200);
                } else if (nombreAchats > SEUIL_AVANT_INFLATION) {
                    printf(COLOR_RED "⚠ Demande forte : Les prix augmentent encore !\n" COLOR_RESET);
                    SLEEP_MS(800);
                }
            }
        }
        
        // --- CATEGORIE 3 : SERVICES ---
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
        
        if (categorie == 4) sauvegarderPartie(joueur);
        if (categorie != 4) SLEEP_MS(800);
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
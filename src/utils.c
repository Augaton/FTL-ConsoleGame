#include "utils.h"
#include "vaisseau.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void effacerEcran() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void afficherGameOver(Vaisseau *joueur) {
    effacerEcran();
    printf(COLOR_RED "\n\n");
    printf("      ############################################\n");
    printf("      #                                          #\n");
    printf("      #          VAISSEAU DETRUIT...             #\n");
    printf("      #                                          #\n");
    printf("      ############################################" COLOR_RESET "\n\n");

    printf(COLOR_BOLD "--- RAPPORT DE MISSION FINAL ---" COLOR_RESET "\n");
    printf(" Commandant        : %s\n", joueur->nom);
    printf(" Secteurs parcourus: %d / %d\n", joueur->distanceParcourue, joueur->distanceObjectif);
    printf(" Fortune restante  : %d ferrailles\n", joueur->ferraille);
    
    // Message personnalisé selon la progression
    printf("\n" COLOR_YELLOW " Verdict du Haut-Commandement : " COLOR_RESET);
    if (joueur->distanceParcourue < joueur->distanceObjectif / 4) 
        printf("Une recrue qui n'a pas survécu au premier saut.\n");
    else if (joueur->distanceParcourue < joueur->distanceObjectif / 2)
        printf("Un pilote prometteur, emporté par l'immensité du vide.\n");
    else if (joueur->distanceParcourue < joueur->distanceObjectif)
        printf("Un héros de la résistance. On se souviendra de votre bravoure.\n");
    else
        printf("INCROYABLE ! Vous avez sauvé la galaxie avant de succomber.\n");

    printf("\n\n[ Appuyez sur ENTREE pour quitter le jeu ]\n");
    
    // Nettoyage final pour s'assurer que l'utilisateur voit le message
    int c; while ((c = getchar()) != '\n' && c != EOF);
    getchar();
}

void afficherVictoire(Vaisseau *joueur) {
    effacerEcran();
    printf(COLOR_GREEN "\n\n");
    printf("      ********************************************\n");
    printf("      * *\n");
    printf("      * VICTOIRE GALACTIQUE            *\n");
    printf("      * *\n");
    printf("      ********************************************" COLOR_RESET "\n\n");

    printf(COLOR_BOLD "--- RAPPORT DE MISSION FINAL ---" COLOR_RESET "\n");
    printf(" Commandant        : %s\n", joueur->nom);
    printf(" Secteurs parcourus: %d / %d\n", joueur->distanceParcourue, joueur->distanceObjectif);
    printf(" Fortune restante  : %d ferrailles\n", joueur->ferraille);
}

void attendreJoueur() {
    printf(COLOR_CYAN "\n[ Appuyez sur ENTREE pour continuer ]" COLOR_RESET);
    int c;
    while ((c = getchar()) != '\n' && c != EOF); // Nettoie le buffer
    getchar();
}

// Sauvegarde et chargement

// Version du format de sauvegarde — à incrémenter à chaque ajout de champ
#define SAVE_VERSION 4

void sauvegarderPartie(Vaisseau *v) {
    FILE *f = fopen("savegame.sav", "w");
    if (f == NULL) {
        printf("\n" COLOR_RED "[ERREUR] Impossible de créer le fichier de sauvegarde." COLOR_RESET "\n");
        return;
    }

    // --- EN-TÊTE ---
    fprintf(f, "VERSION %d\n", SAVE_VERSION);

    // --- STATS DE BASE ---
    fprintf(f, "nom %s\n",              v->nom);
    fprintf(f, "coque %d\n",            v->coque);
    fprintf(f, "coqueMax %d\n",         v->coqueMax);
    fprintf(f, "ferraille %d\n",        v->ferraille);
    fprintf(f, "carburant %d\n",        v->carburant);
    fprintf(f, "missiles %d\n",         v->missiles);
    fprintf(f, "moteurs %d\n",          v->moteurs);
    fprintf(f, "precision %d\n",        v->precision);
    fprintf(f, "bouclierActuel %d\n",   v->bouclierActuel);
    fprintf(f, "debuffArme %d\n",       v->debuffArme);
    fprintf(f, "debuffMoteur %d\n",     v->debuffMoteur);

    // --- NAVIGATION ---
    fprintf(f, "distanceParcourue %d\n",  v->distanceParcourue);
    fprintf(f, "distanceObjectif %d\n",   v->distanceObjectif);
    fprintf(f, "secteurActuel %s\n",      v->secteurActuel);
    fprintf(f, "seedSecteur %u\n",        v->seedSecteur);
    fprintf(f, "explorationActuelle %d\n",v->explorationActuelle);
    fprintf(f, "explorationMax %d\n",     v->explorationMax);

    // --- ÉTAT COMBAT ---
    fprintf(f, "ennemiPresent %d\n",       v->ennemiPresent);
    fprintf(f, "ennemiCoqueActuelle %d\n", v->ennemiCoqueActuelle);
    fprintf(f, "chargeFTL %d\n",           v->chargeFTL);
    fprintf(f, "maxchargeFTL %d\n",        v->maxchargeFTL);

    // --- COMPOSANTS (nom rang efficacite) ---
    fprintf(f, "ARME %s|%d|%d\n",
            v->systemeArme.nom, v->systemeArme.rang, v->systemeArme.efficacite);
    fprintf(f, "BOUCLIER %s|%d|%d\n",
            v->systemeBouclier.nom, v->systemeBouclier.rang, v->systemeBouclier.efficacite);

    // --- ÉQUIPAGE (nom|role|pv|pvMax|estVivant|xp|niveau) ---
    fprintf(f, "nbMembres %d\n", v->nbMembres);
    for (int i = 0; i < 3; i++) {
        Membre *m = &v->equipage[i];
        fprintf(f, "MEMBRE_%d %s|%d|%d|%d|%d|%d|%d\n",
                i, m->nom, (int)m->role, m->pv, m->pvMax,
                m->estVivant, m->xp, m->niveau);
    }

    fclose(f);
    printf("\n" COLOR_GREEN "💾 Progression synchronisée avec les serveurs de la Fédération." COLOR_RESET "\n");
}

int chargerPartie(Vaisseau *v) {
    FILE *f = fopen("savegame.sav", "r");
    if (f == NULL) return 0; // Pas de sauvegarde

    // --- Vérification de version ---
    int version = 0;
    if (fscanf(f, "VERSION %d\n", &version) != 1) {
        fclose(f);
        printf(COLOR_RED "[SAVE] Fichier de sauvegarde illisible. Nouvelle partie.\n" COLOR_RESET);
        return 0;
    }

    if (version < SAVE_VERSION) {
        fclose(f);
        printf(COLOR_YELLOW "[SAVE] Sauvegarde ancienne version (v%d → v%d). "
               "Impossible de charger.\n" COLOR_RESET, version, SAVE_VERSION);
        printf("Appuyez sur ENTREE pour démarrer une nouvelle partie...\n");
        getchar();
        return 0;
    }

    // --- Lecture ligne par ligne ---
    char clef[64];
    char valeur[256];

    while (fscanf(f, "%63s", clef) == 1) {

        // Stats de base
        if      (strcmp(clef, "nom") == 0)
            { fscanf(f, " %49[^\n]", v->nom); }
        else if (strcmp(clef, "coque") == 0)
            { fscanf(f, "%d", &v->coque); }
        else if (strcmp(clef, "coqueMax") == 0)
            { fscanf(f, "%d", &v->coqueMax); }
        else if (strcmp(clef, "ferraille") == 0)
            { fscanf(f, "%d", &v->ferraille); }
        else if (strcmp(clef, "carburant") == 0)
            { fscanf(f, "%d", &v->carburant); }
        else if (strcmp(clef, "missiles") == 0)
            { fscanf(f, "%d", &v->missiles); }
        else if (strcmp(clef, "moteurs") == 0)
            { fscanf(f, "%d", &v->moteurs); }
        else if (strcmp(clef, "precision") == 0)
            { fscanf(f, "%d", &v->precision); }
        else if (strcmp(clef, "bouclierActuel") == 0)
            { fscanf(f, "%d", &v->bouclierActuel); }
        else if (strcmp(clef, "debuffArme") == 0)
            { fscanf(f, "%d", &v->debuffArme); }
        else if (strcmp(clef, "debuffMoteur") == 0)
            { fscanf(f, "%d", &v->debuffMoteur); }

        // Navigation
        else if (strcmp(clef, "distanceParcourue") == 0)
            { fscanf(f, "%d", &v->distanceParcourue); }
        else if (strcmp(clef, "distanceObjectif") == 0)
            { fscanf(f, "%d", &v->distanceObjectif); }
        else if (strcmp(clef, "secteurActuel") == 0)
            { fscanf(f, " %49[^\n]", v->secteurActuel); }
        else if (strcmp(clef, "seedSecteur") == 0)
            { fscanf(f, "%u", &v->seedSecteur); }
        else if (strcmp(clef, "explorationActuelle") == 0)
            { fscanf(f, "%d", &v->explorationActuelle); }
        else if (strcmp(clef, "explorationMax") == 0)
            { fscanf(f, "%d", &v->explorationMax); }

        // État combat
        else if (strcmp(clef, "ennemiPresent") == 0)
            { fscanf(f, "%d", &v->ennemiPresent); }
        else if (strcmp(clef, "ennemiCoqueActuelle") == 0)
            { fscanf(f, "%d", &v->ennemiCoqueActuelle); }
        else if (strcmp(clef, "chargeFTL") == 0)
            { fscanf(f, "%d", &v->chargeFTL); }
        else if (strcmp(clef, "maxchargeFTL") == 0)
            { fscanf(f, "%d", &v->maxchargeFTL); }

        // Composants (format: NOM|rang|efficacite)
        else if (strcmp(clef, "ARME") == 0) {
            fscanf(f, " %255[^\n]", valeur);
            sscanf(valeur, "%29[^|]|%d|%d",
                   v->systemeArme.nom, &v->systemeArme.rang, &v->systemeArme.efficacite);
        }
        else if (strcmp(clef, "BOUCLIER") == 0) {
            fscanf(f, " %255[^\n]", valeur);
            sscanf(valeur, "%29[^|]|%d|%d",
                   v->systemeBouclier.nom, &v->systemeBouclier.rang, &v->systemeBouclier.efficacite);
        }

        // Équipage
        else if (strcmp(clef, "nbMembres") == 0)
            { fscanf(f, "%d", &v->nbMembres); }
        else if (strncmp(clef, "MEMBRE_", 7) == 0) {
            int idx = atoi(clef + 7);
            if (idx >= 0 && idx < 3) {
                fscanf(f, " %255[^\n]", valeur);
                int role;
                sscanf(valeur, "%29[^|]|%d|%d|%d|%d|%d|%d",
                       v->equipage[idx].nom, &role,
                       &v->equipage[idx].pv, &v->equipage[idx].pvMax,
                       &v->equipage[idx].estVivant,
                       &v->equipage[idx].xp, &v->equipage[idx].niveau);
                v->equipage[idx].role = (TypeRole)role;
            }
        }

        // Clef inconnue (nouveau champ d'une version future) → on ignore
        else {
            fgets(valeur, sizeof(valeur), f);
        }
    }

    fclose(f);
    return 1;
}

void supprimerSauvegarde() {
    if (remove("savegame.sav") == 0) {
        printf(COLOR_RED "\n[SYSTEME] Données de vol effacées. Fin de transmission.\n" COLOR_RESET);
    }
}

void finaliserEvenement(Vaisseau *joueur) {
    strcpy(joueur->secteurActuel, "REPOS");
    joueur->ennemiPresent = 0;
    joueur->ennemiCoqueActuelle = 0;
    sauvegarderPartie(joueur);
}

// Fonction utilitaire pour lire un entier de manière robuste
int lireEntierSecurise(int min, int max) {
    int valeur;
    char buffer[100]; // On lit une ligne de texte pour éviter les bugs de scanf

    while (1) {
        // On lit toute la ligne (jusqu'à Entrée)
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            // On essaie de convertir le texte en nombre
            // %d capture le nombre, le reste est ignoré
            if (sscanf(buffer, "%d", &valeur) == 1) {
                // Si c'est un nombre valide, on vérifie les bornes
                if (valeur >= min && valeur <= max) {
                    return valeur; // C'est gagné, on renvoie la valeur
                }
            }
        }
        
        // Si on arrive ici, c'est que l'entrée était mauvaise
        printf(COLOR_RED "⚠ Entrée invalide. Veuillez taper un nombre entre %d et %d : " COLOR_RESET, min, max);
        printf(COLOR_YELLOW "> " COLOR_RESET);
    }
}

// Lit un choix de menu depuis stdin.
// Retourne la valeur saisie, ou `defaut` si l'entrée est invalide.
// Ne vérifie pas de bornes — c'est à l'appelant de gérer les cas inattendus.
int lireChoix(int defaut) {
    int choix;
    if (scanf("%d", &choix) != 1) {
        int c; while ((c = getchar()) != '\n' && c != EOF);
        return defaut;
    }
    return choix;
}

// Petite fonction utilitaire pour l'affichage coloré
void afficherDestinationColoree(const char* destination) {
    if (strstr(destination, "Hostile")) printf(COLOR_RED);
    else if (strstr(destination, "Station")) printf(COLOR_GREEN);
    else if (strstr(destination, "Detresse")) printf(COLOR_YELLOW);
    else if (strstr(destination, "Nebuleuse")) printf(COLOR_MAGENTA);
    else printf(COLOR_CYAN);
    
    printf("%s" COLOR_RESET, destination);
}
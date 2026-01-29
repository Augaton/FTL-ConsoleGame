#include "vaisseau.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

const char* getRoleNom(TypeRole role) {
    switch(role) {
        case ROLE_PILOTE: return "PILOTE";
        case ROLE_INGENIEUR: return "INGENIEUR";
        case ROLE_SOLDAT: return "SOLDAT";
        default: return "PASSAGER";
    }
}

void initialiserNouvellePartie(Vaisseau *joueur) {
    // --- 1. STATS DE BASE ---
    joueur->coque = 30;
    joueur->coqueMax = 30;
    joueur->missiles = 3;
    joueur->ferraille = 0;
    joueur->carburant = 12;
    joueur->distanceParcourue = 0;
    joueur->moteurs = 1;
    joueur->precision = 0;

    // Reset états
    joueur->ennemiPresent = 0;       
    joueur->ennemiCoqueActuelle = 0;
    joueur->chargeFTL = 0;
    joueur->maxchargeFTL = 3;

    // Equipement Base
    strcpy(joueur->systemeArme.nom, "Laser Burst Mk1");
    joueur->systemeArme.rang = 1;
    joueur->systemeArme.efficacite = 2;
    strcpy(joueur->systemeBouclier.nom, "Bouclier Alpha");
    joueur->systemeBouclier.rang = 1;
    joueur->systemeBouclier.efficacite = 1;
    joueur->bouclierActuel = 1;

    // --- 2. NOM DU VAISSEAU ---
    effacerEcran();
    printf(COLOR_CYAN "╔════════════════════════════════════════╗\n");
    printf("║      CONFIGURATION DU VAISSEAU         ║\n");
    printf("╚════════════════════════════════════════╝\n" COLOR_RESET);
    
    printf(COLOR_BLUE "\nCommandant, entrez votre nom d'appel :" COLOR_RESET);
    printf(COLOR_YELLOW "\n> " COLOR_RESET);
    
    // fgets est déjà sécurisé, il prend tout jusqu'à "Entrée"
    if (fgets(joueur->nom, sizeof(joueur->nom), stdin) != NULL) {
        joueur->nom[strcspn(joueur->nom, "\n")] = 0; // Retire le \n
    } else {
        strcpy(joueur->nom, "Voyager"); // Nom par défaut si erreur grave de lecture
    }
    
    // Si l'utilisateur appuie juste sur Entrée sans rien écrire
    if (strlen(joueur->nom) == 0) strcpy(joueur->nom, "Voyager");

    // --- 3. INITIALISATION DE L'ÉQUIPAGE ---
    joueur->nbMembres = 3;

    char bufferNom[50];
    snprintf(bufferNom, 50, "Cmdt. %.20s", joueur->nom);
    
    // Slot 1 (Joueur)
    strcpy(joueur->equipage[0].nom, bufferNom);
    joueur->equipage[0].role = ROLE_PILOTE;
    joueur->equipage[0].pv = 100;
    joueur->equipage[0].pvMax = 100;
    joueur->equipage[0].estVivant = 1;
    joueur->equipage[0].xp = 0;    
    joueur->equipage[0].niveau = 0;

    // Slot 2 (Wilson)
    strcpy(joueur->equipage[1].nom, "Wilson");
    joueur->equipage[1].role = ROLE_INGENIEUR;
    joueur->equipage[1].pv = 100;
    joueur->equipage[1].pvMax = 100;
    joueur->equipage[1].estVivant = 1;
    joueur->equipage[1].xp = 0;
    joueur->equipage[1].niveau = 0;

    // Slot 3 (Vide)
    strcpy(joueur->equipage[2].nom, "--- LIBRE ---");
    joueur->equipage[2].estVivant = 0;
    joueur->equipage[2].xp = 0;
    joueur->equipage[2].niveau = 0;

    // --- 4. CHOIX DE LA DURÉE ---
    printf("\n" COLOR_CYAN "--- PARAMÈTRES DE MISSION ---" COLOR_RESET "\n");
    printf("1. Raid rapide (10 secteurs)\n");
    printf("2. Mission standard (20 secteurs)\n");
    printf("3. Longue expédition (40 secteurs)\n");
    printf("4. Personnalisé\n");
    printf(COLOR_YELLOW "> " COLOR_RESET);
    
    // Utilisation de la fonction sécurisée (Choix entre 1 et 4)
    int mode = lireEntierSecurise(1, 4);
    
    if (mode == 1) joueur->distanceObjectif = 10;
    else if (mode == 2) joueur->distanceObjectif = 20;
    else if (mode == 3) joueur->distanceObjectif = 40;
    else {
        printf("Nombre de secteurs (Min: 5, Max: 100) : ");
        printf(COLOR_YELLOW "\n> " COLOR_RESET);
        // On sécurise aussi la saisie personnalisée
        joueur->distanceObjectif = lireEntierSecurise(5, 100);
    }

    // --- 5. GESTION DE LA SEED ---
    printf("\n" COLOR_CYAN "--- MATRICE DE GÉNÉRATION (SEED) ---" COLOR_RESET "\n");
    printf("1. Aléatoire (Recommandé)\n");
    printf("2. Saisir une Seed manuelle\n");
    printf(COLOR_YELLOW "> " COLOR_RESET);
    
    // Utilisation de la fonction sécurisée (Choix entre 1 et 2)
    int seedMode = lireEntierSecurise(1, 2);
    
    if (seedMode == 2) {
        printf("Entrez la seed numérique (Max 9 chiffres) : ");
        printf(COLOR_YELLOW "\n> " COLOR_RESET);
        
        // Cas particulier : on veut lire un grand nombre, on réutilise une logique fgets/sscanf locale
        // ou on utilise lireEntierSecurise avec un grand MAX si c'est un int classique
        // Ici on va simplifier en demandant un int positif
        joueur->seedSecteur = (unsigned int)lireEntierSecurise(1, 2000000000);
    } else {
        joueur->seedSecteur = (unsigned int)time(NULL) + strlen(joueur->nom);
    }

    printf(COLOR_GREEN "\n>>> SYSTÈMES INITIALISÉS. SEED: %u <<<\n" COLOR_RESET, joueur->seedSecteur);
    SLEEP_MS(1000);

    sauvegarderPartie(joueur);
}
void menuEtatVaisseau(Vaisseau *joueur) {
    int retour = 0;
    while (!retour) {
        effacerEcran();

        // --- 1. EN-TÊTE & RESSOURCES ---
        printf(COLOR_CYAN "╔══════════════════════════════════════════════════════════╗\n");
        printf("║ " COLOR_BOLD "📊 LOGS TECHNIQUES" COLOR_RESET COLOR_CYAN " : %-35s ║\n", joueur->nom);
        printf("╠══════════════════════════════════════════════════════════╣\n");
        printf("║ " COLOR_YELLOW "⚡ CARBURANT: %-3d  " COLOR_YELLOW "⚓ FERRAILLE: %-4d  " COLOR_YELLOW "🚀 MISSILES: %-3d " COLOR_CYAN " ║\n", 
                joueur->carburant, joueur->ferraille, joueur->missiles);
        printf("╠══════════════════════════════════════════════════════════╣\n" COLOR_RESET);

        // --- 2. SYSTÈMES (Calcul dynamique des bonus) ---
        printf(COLOR_CYAN "║ " COLOR_BOLD "DIAGNOSTIC SYSTÈMES" COLOR_RESET COLOR_CYAN "                                      ║\n" COLOR_RESET);

        // -- OFFENSIF --
        int bonusDegats = getBonusDegats(joueur);
        printf("║  " COLOR_RED "✇ ARMEMENT :" COLOR_RESET " %-20s " COLOR_RED "(Puissance: %-2d) ", 
               joueur->systemeArme.nom, joueur->systemeArme.efficacite);
        if (bonusDegats > 0) printf(COLOR_GREEN " +%d(Soldat)", bonusDegats);
        printf(COLOR_CYAN "      ║\n" COLOR_RESET);

        // -- DÉFENSIF --
        printf("║  " COLOR_BLUE "🛡 BOUCLIER :" COLOR_RESET " %-20s " COLOR_BLUE "(Charge: %-2d/%-2d)" COLOR_RESET "       " COLOR_CYAN "║\n", 
               joueur->systemeBouclier.nom, joueur->bouclierActuel, joueur->systemeBouclier.efficacite);

        // -- MOTEURS --
        int bonusPilote = getBonusEsquive(joueur);
        int esquiveTotale = (joueur->moteurs * 5) + bonusPilote;
        
        printf("║  " COLOR_GREEN "💨 MOTEURS  :" COLOR_RESET " Niveau %-2d      " COLOR_GREEN "Esquive: %d%%", 
               joueur->moteurs, esquiveTotale);
        if (bonusPilote > 0) printf("(+%d Pilote)", bonusPilote);
        else printf("           ");
        printf(COLOR_CYAN "   ║\n" COLOR_RESET);

        printf(COLOR_CYAN "╠══════════════════════════════════════════════════════════╣\n" COLOR_RESET);

        // --- 3. ÉQUIPAGE ---
        printf(COLOR_CYAN "║ " COLOR_BOLD "RAPPORT D'ÉQUIPAGE" COLOR_RESET COLOR_CYAN "                                       ║\n" COLOR_RESET);
        
        for(int i=0; i<3; i++) {
            Membre *m = &joueur->equipage[i];
            if (m->estVivant) {
                // Gestion visuelle XP (Étoiles)
                char stars[16] = "";
                if(m->niveau == 1) strcpy(stars, COLOR_YELLOW "★" COLOR_RESET);
                if(m->niveau >= 2) strcpy(stars, COLOR_YELLOW "★★" COLOR_RESET);

                // Couleur Santé
                char *colPv = (m->pv > 50) ? COLOR_GREEN : (m->pv > 25 ? COLOR_YELLOW : COLOR_RED);

                // Format: 1. [ROLE] Nom ...
                printf(COLOR_CYAN "║ " COLOR_RESET "%d. " COLOR_BOLD "%-16s" COLOR_RESET " [%-9s]  Vie:%s%3d%%" COLOR_RESET "  XP:%-8s   " COLOR_CYAN "║\n", 
                    i+1, m->nom, getRoleNom(m->role), colPv, m->pv, stars);
            } else {
                printf(COLOR_CYAN "║ " COLOR_RESET "%d. " COLOR_BLACK "--- POSTE VACANT ---" COLOR_RESET "                                  " COLOR_CYAN "║\n", i+1);
            }
        }
        printf(COLOR_CYAN "╚══════════════════════════════════════════════════════════╝\n" COLOR_RESET);

        // --- 4. MENU ACTIONS ---
        printf(COLOR_BOLD " COMMANDES DISPONIBLES :" COLOR_RESET "\n");
        printf(" [1-3] Gérer un membre (Changer de poste)\n");
        printf(" [4]   " COLOR_GREEN "INFIRMERIE" COLOR_RESET " (Soigner l'équipage | -1 Fuel)\n");
        printf(" [5]   " COLOR_YELLOW "ATELIER" COLOR_RESET "    (Réparer +1 Coque | -2 Ferraille)\n");
        printf(" [0]   " COLOR_WHITE "RETOUR" COLOR_RESET "\n");
        printf(COLOR_YELLOW "\n > " COLOR_RESET);

        int choix;
        if (scanf("%d", &choix) != 1) { 
            int c; while ((c = getchar()) != '\n' && c != EOF); 
            continue; 
        }

        // --- LOGIQUE DES ACTIONS ---

        // GESTION MEMBRE (Réassignation)
        if (choix >= 1 && choix <= 3) {
            Membre *m = &joueur->equipage[choix-1];
            if (m->estVivant) {
                printf("\n" COLOR_BOLD ">>> RÉAFFECTATION : %s" COLOR_RESET "\n", m->nom);
                printf("Poste actuel : %s\n", getRoleNom(m->role));
                printf("1. Pilote    (+Esquive)\n");
                printf("2. Ingénieur (+Recharge Bouclier)\n");
                printf("3. Soldat    (+Dégâts Armes)\n");
                printf("0. Annuler\n> ");
                
                int r;
                scanf("%d", &r);
                if (r == 1) m->role = ROLE_PILOTE;
                if (r == 2) m->role = ROLE_INGENIEUR;
                if (r == 3) m->role = ROLE_SOLDAT;
                if (r > 0) printf(COLOR_GREEN "Mutation enregistrée.\n" COLOR_RESET);
            } else {
                printf(COLOR_RED "Ce poste est vacant.\n" COLOR_RESET);
            }
            SLEEP_MS(600);
        }
        
        // SOINS
        else if (choix == 4) {
            if (joueur->carburant > 0) {
                int besoinSoin = 0;
                for(int i=0; i<3; i++) if(joueur->equipage[i].estVivant && joueur->equipage[i].pv < joueur->equipage[i].pvMax) besoinSoin = 1;
                
                if(besoinSoin) {
                    joueur->carburant--;
                    for(int i=0; i<3; i++) {
                        if (joueur->equipage[i].estVivant) joueur->equipage[i].pv = joueur->equipage[i].pvMax;
                    }
                    printf(COLOR_GREEN ">>> Cycle de régénération terminé.\n" COLOR_RESET);
                } else {
                    printf("Équipage en pleine forme. Inutile de gaspiller du carburant.\n");
                }
            } else {
                printf(COLOR_RED "Carburant insuffisant pour alimenter l'infirmerie.\n" COLOR_RESET);
            }
            SLEEP_MS(1000);
        }

        // RÉPARATION DE FORTUNE
        else if (choix == 5) {
            if (joueur->ferraille >= 2 && joueur->coque < joueur->coqueMax) {
                joueur->ferraille -= 2;
                joueur->coque++;
                printf(COLOR_GREEN ">>> Réparation d'urgence effectuée (+1 Coque).\n" COLOR_RESET);
            } else if (joueur->coque >= joueur->coqueMax) {
                printf("La coque est déjà intègre.\n");
            } else {
                printf(COLOR_RED "Pas assez de ferraille (Requis: 2).\n" COLOR_RESET);
            }
            SLEEP_MS(800);
        }

        // RETOUR
        else if (choix == 0) {
            retour = 1;
        }
    }
}

void afficherBarreFTL(int charge) {
    printf("FTL [");
    for(int i = 0; i < 3; i++) {
        if (i < charge) printf(COLOR_YELLOW "#" COLOR_RESET);
        else printf("-");
    }
    printf("] ");
}

// Bonus équipage

// Fonction pour faire gagner de l'XP
void gagnerXP(Membre *m, int montant) {
    if (!m->estVivant || m->niveau >= 2) return; // Cap niveau 2

    m->xp += montant;
    
    // Seuil de niveau (Ex: 50 XP pour niveau 1, 100 XP pour niveau 2)
    int seuil = (m->niveau + 1) * 50; 
    
    if (m->xp >= seuil) {
        m->niveau++;
        m->xp = 0; // Reset ou report, ici reset pour simplifier
        printf(COLOR_YELLOW "★ %s EST PROMU AU RANG %d ! ★\n" COLOR_RESET, m->nom, m->niveau);
        SLEEP_MS(1000);
    }
}

int getBonusEsquive(Vaisseau *v) {
    for (int i = 0; i < 3; i++) {
        if (v->equipage[i].estVivant && v->equipage[i].role == ROLE_PILOTE) {
            // Niv 0: +10%, Niv 1: +15%, Niv 2: +25%
            return 10 + (v->equipage[i].niveau * 7); 
        }
    }
    return 0;
}

int getBonusDegats(Vaisseau *v) {
    for (int i = 0; i < 3; i++) {
        if (v->equipage[i].estVivant && v->equipage[i].role == ROLE_SOLDAT) {
            // Niv 0: +1 Dégât, Niv 2: +2 Dégâts (Très fort)
            return 1 + (v->equipage[i].niveau / 2); 
        }
    }
    return 0;
}

int getBonusIngenieur(Vaisseau *v) {
    for (int i = 0; i < 3; i++) {
        if (v->equipage[i].estVivant && v->equipage[i].role == ROLE_INGENIEUR) {
            // Retourne 1 chance sur X de recharger 2 boucliers au lieu d'1
            // Niv 0: 0%, Niv 1: 30%, Niv 2: 60%
            if (v->equipage[i].niveau > 0) return 30 * v->equipage[i].niveau;
        }
    }
    return 0;
}

// Gestion équipage

void subirDegatsEquipage(Vaisseau *v) {
    // 30% de chance qu'un membre soit blessé lors d'un impact coque
    if (rand() % 100 < 30) {
        // On choisit un slot au hasard (0, 1 ou 2)
        int id = rand() % 3;
        
        if (v->equipage[id].estVivant) {
            int blessure = 10 + (rand() % 20); // Entre 10 et 30 dégâts
            v->equipage[id].pv -= blessure;
            
            printf(COLOR_RED "\n[ALERTE MÉDICALE] %s a été blessé par l'impact ! (-%d PV)\n" COLOR_RESET, v->equipage[id].nom, blessure);
            
            // Gestion de la mort
            if (v->equipage[id].pv <= 0) {
                v->equipage[id].pv = 0;
                v->equipage[id].estVivant = 0;
                v->nbMembres--;
                printf(COLOR_RED COLOR_BOLD ">>> R.I.P. %s est mort au combat ! <<<\n" COLOR_RESET, v->equipage[id].nom);
                SLEEP_MS(2000);
            }
            SLEEP_MS(1000);
        }
    }
}
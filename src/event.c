#include "event.h"
#include "combat.h"
#include "utils.h"
#include "magasin.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void menuVoyage(Vaisseau *joueur) {
    int continuerMenu = 1;

    while (continuerMenu && joueur->coque > 0) {
        int choix = 0;
        effacerEcran();
        
        // --- HEADER COCKPIT ---
        printf(COLOR_CYAN "╔══════════════════════════════════════════════════════════╗\n");
        printf("║ " COLOR_BOLD "%-18s" COLOR_RESET COLOR_CYAN "CONSOLE DE NAVIGATION   SECTEUR: %02d/%d ║\n", joueur->nom, joueur->distanceParcourue, joueur->distanceObjectif);
        printf("╠══════════════════════════════════════════════════════════╣" COLOR_RESET "\n");

        // --- SECTION ÉTAT ---
        printf(COLOR_CYAN "║ " COLOR_RESET "COQUE: ");
        printf(COLOR_RESET " %02d/%02d  ", joueur->coque, joueur->coqueMax);

        printf(COLOR_CYAN "SHIELD: " COLOR_RESET);

        for(int i=0; i < joueur->systemeBouclier.efficacite; i++) {
            printf(i < joueur->bouclierActuel ? COLOR_CYAN "⬢ " : COLOR_RED "⬡ ");
        }

        // Ajustement dynamique des espaces pour que le cadre ║ reste aligné
        int espacesRestants = 14 - (joueur->systemeBouclier.efficacite * 2);
        for(int i=0; i<espacesRestants; i++) printf(" ");
        printf(COLOR_CYAN "║\n");

        // --- SECTION INVENTAIRE (Style Grille) ---
        printf("╠══════════════════════════════════════════════════════════╣\n");
        printf("║ " COLOR_YELLOW "⚡ " COLOR_RESET "CARBURANT: %-3d  " COLOR_YELLOW "⚓ " COLOR_RESET "FERRAILLE: %-4d  " COLOR_YELLOW "🚀 " COLOR_RESET "MISSILES: %-3d " COLOR_CYAN " ║\n", 
                joueur->carburant, joueur->ferraille, joueur->missiles);

        // --- SECTION PROGRESSION ---
        printf("╠══════════════════════════════════════════════════════════╣\n");
        printf("║ PROGRESSION: ");
        for(int i=1; i<=joueur->distanceObjectif; i++) {
            if(i < joueur->distanceParcourue) printf(COLOR_GREEN "═");
            else if(i == joueur->distanceParcourue) printf(COLOR_BOLD COLOR_YELLOW "✈");
            else printf(COLOR_RESET "·");
        }
        printf(COLOR_CYAN " [DESTINATION]          ║\n");
        printf("╚══════════════════════════════════════════════════════════╝" COLOR_RESET "\n\n");

        // --- MENU ACTIONS ---
        printf(COLOR_BOLD "  [1]" COLOR_RESET " ENGAGER LE SAUT SPATIAL\n");
        printf(COLOR_BOLD "  [2]" COLOR_RESET " CONSULTER LES LOGS TECHNIQUES\n");
        printf(COLOR_BOLD "  [3]" COLOR_RESET " ABANDONNER LA MISSION\n");
        printf("\n" COLOR_YELLOW" COMMANDE > " COLOR_RESET);

        if (scanf("%d", &choix) != 1) {
            int c; while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }
        int c; while ((c = getchar()) != '\n' && c != EOF); // Nettoyage systématique

        if (choix == 1) {
             lancerSequenceDeSaut(joueur); 
        }
        else if (choix == 2) {
            afficherVaisseau(joueur);
        }
        else if (choix == 3) {

            printf(COLOR_CYAN "Fermeture des systemes...\n" COLOR_RESET);
            joueur->coque = 0;
            continuerMenu = 0;

        } 
    }
}

void lancerSequenceDeSaut(Vaisseau *joueur) {
    const char* baliseA = inspecterBalise();
    const char* baliseB = inspecterBalise();
    int choixSaut;

    printf("\n" COLOR_YELLOW "─── CALCUL DES TRAJECTOIRES FTL ───" COLOR_RESET "\n");
    printf("1. "); afficherDestinationColoree(baliseA); printf("\n");
    printf("2. "); afficherDestinationColoree(baliseB); printf("\n");
    printf(COLOR_YELLOW "\n Destination (1 ou 2) > " COLOR_RESET);
    scanf("%d", &choixSaut);

    const char* destination = (choixSaut == 1) ? baliseA : baliseB;

    // --- MISE À JOUR DU SECTEUR POUR LA SAUVEGARDE ---
    // On enregistre où on va pour que le chargement sache quoi relancer
    strncpy(joueur->secteurActuel, destination, 49);
    joueur->secteurActuel[49] = '\0'; // Sécurité

    // --- CONSOMMATION DE CARBURANT ---
    if (strcmp(destination, "Nebuleuse (Inconnu - Gratuit)") != 0) {
        if (joueur->carburant > 0) joueur->carburant--;
        else {
            printf(COLOR_RED "\n[ALERTE] Panne de carburant ! Dérive critique...\n" COLOR_RESET);
            joueur->coque -= 5;
            SLEEP_MS(1500);
        }
    }

    printf(COLOR_CYAN "\nSaut FTL en cours" COLOR_RESET);
    for(int i=0; i<3; i++) { printf(COLOR_CYAN "." COLOR_RESET); fflush(stdout); SLEEP_MS(300); }
    
    joueur->distanceParcourue++;

    // --- SAUVEGARDE ---
    sauvegarderPartie(joueur);

    // --- EXÉCUTION ---
    executerEvenement(joueur, joueur->secteurActuel);

    // Après l'événement, on remet le secteur à "REPOS" pour éviter de relancer l'événement au chargement
    strcpy(joueur->secteurActuel, "REPOS");
    sauvegarderPartie(joueur);
}

const char* inspecterBalise() {
    int r = rand() % 100;
    if (r < 10) return "Nebuleuse (Inconnu - Gratuit)";
    if (r < 40) return "Signal Hostile (Combat)";
    if (r < 55) return "Station Commerciale (Magasin)";
    if (r < 80) return "Signal de Detresse";
    return "Secteur Vide";
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

void descriptionSecteurVide(Vaisseau *joueur) {
    char *ambiances[] = {
        "Le silence règne ici. Seules les étoiles lointaines illuminent le cockpit.",
        "Vous traversez un nuage de poussière cosmique scintillante. Magnifique mais désert.",
        "Les scanners sont calmes. Une planète géante gazeuse défile lentement sous vos pieds.",
        "Quelques débris de roche flottent ici, vestiges d'une collision millénaire.",
        "Le vide spatial s'étend à l'infini. Aucun signal radar à la ronde.",
        "Une nébuleuse rougeoyante illumine le secteur d'une lueur spectrale.",
        "Rien. Juste l'obscurité et le ronronnement rassurant de vos moteurs."
    };
    
    printf("\n" COLOR_CYAN "[EXPLORATION]" COLOR_RESET " %s\n", ambiances[rand() % 7]);

    // --- PETIT BONUS ALÉATOIRE (30% de chance) ---
    int chance = rand() % 100;
    if (chance < 30) {
        SLEEP_MS(500);
        int typeBonus = rand() % 3;
        
        printf(COLOR_YELLOW "💡 MOMENT DE CALME : " COLOR_RESET);
        if (typeBonus == 0 && joueur->bouclierActuel < joueur->systemeBouclier.efficacite) {
            joueur->bouclierActuel++;
            printf("Votre équipage a recalibré les boucliers (+1).\n");
        }
        else if (typeBonus == 1) {
            int gain = (rand() % 3) + 1;
            joueur->ferraille += gain;
            printf("Vous avez récupéré %d ferraille dans les filtres à poussière du vaisseau.\n", gain);
        }
        else {
            printf("L'équipage en profite pour se reposer. Le moral remonte.\n");
        }
    }
}

void executerEvenement(Vaisseau *joueur, const char* type) {
    const char* evenementFinal = type;

    if (strcmp(type, "Nebuleuse (Inconnu - Gratuit)") == 0) {
        printf("\n[SYSTEME] Entree dans la nebuleuse. Capteurs brouilles...\n");
        int r = rand() % 100;
        if (r < 50) evenementFinal = "Signal Hostile (Combat)";
        else if (r < 80) evenementFinal = "Signal de Detresse";
        else evenementFinal = "Secteur Vide";
    }

    if (strcmp(evenementFinal, "Signal Hostile (Combat)") == 0) {
        Vaisseau ennemi = genererEnnemi(joueur->distanceParcourue, joueur->seedSecteur);
        lancerCombat(joueur, &ennemi);
    } else if (strcmp(evenementFinal, "Station Commerciale (Magasin)") == 0) {
        ouvrirMagasin(joueur);
    } else if (strcmp(evenementFinal, "Signal de Detresse") == 0) {
        lancerEvenementAleatoire(joueur);
    } else {
        descriptionSecteurVide(joueur);
        attendreJoueur();
    }
}

void lancerEvenementAleatoire(Vaisseau *joueur) {
    // On fixe l'aléatoire sur la seed du secteur
    unsigned int seedUnique = joueur->seedSecteur + (joueur->distanceParcourue * 200);
    srand(seedUnique);

    int typeEv = rand() % 7; 

    switch(typeEv) {
        case 0: evenementMarchandAmbulant(joueur); break;
        case 1: evenementEpaveDerivante(joueur); break;
        case 2: evenementPluieAsteroides(joueur); break;
        case 3: evenementCapsuleSurvie(joueur); break;
        case 4: evenementAnomalieSpatiale(joueur); break;
        case 5: evenementDeresse(joueur); break;
        case 6: evenementLoterie(joueur); break;
    }

    // Une fois l'événement choisi et traité, on remet le temps réel 
    // pour que les futurs jets de dés (combat, etc.) ne soient pas prévisibles.
    srand((unsigned int)time(NULL));
}

// LISTE DES ÉVÉNEMENTS

void evenementDeresse(Vaisseau *joueur) {
    int choix;
    printf("\n" COLOR_YELLOW "[SIGNAL DE DETRESSE] Un transporteur civil est en panne." COLOR_RESET "\n");
    printf("1. Aider (70%% chance de succes)\n");
    printf("2. Ignorer\n");
    printf("Votre decision : ");
    scanf("%d", &choix);

    if (choix == 1) {
        // --- IMPLÉMENTATION DE LA SEED ---
        // On force l'aléatoire à être celui prévu pour ce secteur
        unsigned int seedUnique = joueur->seedSecteur + (joueur->distanceParcourue * 210);
        srand(seedUnique);
        
        // Ce calcul donnera TOUJOURS le même résultat pour cette seed
        int jetDeDes = rand() % 100; 

        if (jetDeDes < 70) {
            joueur->ferraille += 20;
            printf(COLOR_GREEN "Succes ! " COLOR_RESET "+20 Ferraille.\n");
        } else {
            joueur->coque -= 3;
            printf(COLOR_RED "Piege ! " COLOR_RESET "L'explosion vous inflige 3 degats.\n");
        }
    

    } else {
        printf("Vous passez votre chemin.\n");
    }

    srand((unsigned int)time(NULL));

    finaliserEvenement(joueur);
    attendreJoueur();
}

void evenementEpaveDerivante(Vaisseau *joueur) {
    int choix;
    printf("\n" COLOR_CYAN "[DECOUVERTE]" COLOR_RESET " Une carcasse de croiseur flotte ici.\n");
    printf("1. Envoyer une équipe de récupération (Risqué)\n");
    printf("2. Scanner à distance (Sûr mais faible gain)\n");
    printf(COLOR_YELLOW "> " COLOR_RESET);
    scanf("%d", &choix);

    if (choix == 1) {
        unsigned int seedUnique = joueur->seedSecteur + (joueur->distanceParcourue * 220);
        srand(seedUnique);

        int r = rand() % 100;

        if (r < 60) {
            int gain = 30 + (rand() % 20);
            printf(COLOR_GREEN "Succès ! Vos hommes ramènent %d Ferraille." COLOR_RESET "\n", gain);
            joueur->ferraille += gain;
        } else {
            printf(COLOR_RED "Désastre ! Une fuite de plasma blesse l'équipage. Coque -4." COLOR_RESET "\n");
            joueur->coque -= 4;
        }
    } else {
        printf("Le scan révèle quelques débris : +5 Ferraille.\n");
        joueur->ferraille += 5;
    }

    finaliserEvenement(joueur);

    attendreJoueur();
}

void evenementPluieAsteroides(Vaisseau *joueur) {
    printf("\n" COLOR_YELLOW "[ALERTE]" COLOR_RESET " Vous traversez un champ d'astéroïdes instable !\n");
    
    int chanceEsquive = 40 + (joueur->moteurs * 10); // Plus de moteurs = plus de survie

    unsigned int seedUnique = joueur->seedSecteur + (joueur->distanceParcourue * 330);
    srand(seedUnique);
    int r = rand() % 100;

    if (r < chanceEsquive) {
        printf(COLOR_GREEN "Manoeuvre parfaite ! Vous slalomez entre les rochers." COLOR_RESET "\n");
    } else {
        printf(COLOR_RED "CHOC ! Un astéroïde percute le flanc gauche. Coque -3." COLOR_RESET "\n");
        joueur->coque -= 3;
    }

    finaliserEvenement(joueur);
    attendreJoueur();
}

void evenementAnomalieSpatiale(Vaisseau *joueur) {
    printf("\n" COLOR_BOLD COLOR_CYAN "[ANOMALIE]" COLOR_RESET " Le vaisseau est aspiré dans une faille chromatique...\n");
    printf("La réalité semble se distordre autour de vous.\n");
    for(int i=0; i<3; i++) { printf("."); fflush(stdout); SLEEP_MS(600); }
    printf("\n");

    unsigned int seedUnique = joueur->seedSecteur + (joueur->distanceParcourue * 440);
    srand(seedUnique);

    int r = rand() % 100;

    if (r < 25) { 
        // On augmente l'efficacité du système de bouclier de façon permanente
        joueur->systemeBouclier.efficacite += 1;
        joueur->bouclierActuel = joueur->systemeBouclier.efficacite;
        printf(COLOR_YELLOW "✨ SURCHARGE : Les molécules de la coque se densifient. Bouclier Max +1 !" COLOR_RESET "\n");
    } 
    else if (r < 50) {
        joueur->distanceParcourue += -1;
        if (joueur->distanceParcourue < 0) joueur->distanceParcourue = 0;
        printf(COLOR_GREEN "⏪ DISTORSION : Le temps s'inverse... Vous avez reculé ! (Secteur -1)" COLOR_RESET "\n");
    }
    else if (r < 75) {
        // EFFET 3 : Mutation des Ressources
        printf(COLOR_CYAN "💎 ALCHIMIE : La ferraille en soute vibre et se transforme..." COLOR_RESET "\n");
        int gain = (rand() % 15) + 5;
        joueur->ferraille += gain;
        joueur->carburant += 1;
        printf("Vous trouvez %d Ferraille et 1 Carburant matérialisés dans les couloirs.\n", gain);
    }
    else {
        // EFFET 4 : Choc de vide (Négatif)
        printf(COLOR_RED "⚠️  REJET : La faille se referme violemment sur votre moteur FTL !" COLOR_RESET "\n");
        joueur->coque -= 4;
        joueur->carburant = (joueur->carburant > 0) ? joueur->carburant - 1 : 0;
        printf("Dégâts structurels subis (-4 Coque) et perte d'une unité de carburant.\n");
    }

finaliserEvenement(joueur);

    attendreJoueur();
}

void evenementCapsuleSurvie(Vaisseau *joueur) {
    printf("\n" COLOR_CYAN "[SIGNAL]" COLOR_RESET " Une capsule de survie dérive. Elle semble dater de la dernière guerre.\n");
    printf("1. L'ouvrir avec précaution\n");
    printf("2. La recycler à distance (Sûr mais peu rentable)\n");
    printf(COLOR_YELLOW "> " COLOR_RESET);
    int choix;
    scanf("%d", &choix);

    if (choix == 1) {
        unsigned int seedUnique = joueur->seedSecteur + (joueur->distanceParcourue * 550);
        srand(seedUnique);
        int r = rand() % 100;
        if (r < 40) {
            printf(COLOR_GREEN "✨ MIRACLE : Un ingénieur était à l'intérieur ! Il répare vos systèmes. (+5 Coque)" COLOR_RESET "\n");
            joueur->coque += 5;
            if (joueur->coque > joueur->coqueMax) joueur->coque = joueur->coqueMax;
        } else if (r < 80) {
            int ferraille = 15 + (rand() % 15);
            printf(COLOR_YELLOW "BUTIN : La capsule contenait des fournitures militaires. (+%d Ferraille)" COLOR_RESET "\n", ferraille);
            joueur->ferraille += ferraille;
        } else {
            printf(COLOR_RED "PIÈGE : La capsule contenait un drone de sécurité fou ! Il s'auto-détruit. (-4 Coque)" COLOR_RESET "\n");
            joueur->coque -= 4;
        }
    } else {
        printf("Vous broyez la capsule : +5 Ferraille.\n");
        joueur->ferraille += 5;
    }

    finaliserEvenement(joueur);
    attendreJoueur();
}

void evenementMarchandAmbulant(Vaisseau *joueur) {
    printf("\n" COLOR_YELLOW "[COMMERCE]" COLOR_RESET " Un marchand Jawa vous hèle sur les ondes.\n");
    printf("\"Besoin de quelque chose, étranger ?\"\n");
    printf("1. Acheter 2 Carburant (10 Ferraille)\n");
    printf("2. Acheter 3 Missiles (15 Ferraille)\n");
    printf("3. Ignorer\n");
    printf(COLOR_YELLOW "> " COLOR_RESET);
    int choix;
    scanf("%d", &choix);

    if (choix == 1) {
        if (joueur->ferraille >= 10) {
            joueur->ferraille -= 10;
            joueur->carburant += 2;
            printf(COLOR_GREEN "Transaction réussie." COLOR_RESET "\n");
        } else printf(COLOR_RED "Pas assez de ferraille !" COLOR_RESET "\n");
    } 
    else if (choix == 2) {
        if (joueur->ferraille >= 15) {
            joueur->ferraille -= 15;
            joueur->missiles += 3;
            printf(COLOR_GREEN "Transaction réussie." COLOR_RESET "\n");
        } else printf(COLOR_RED "Pas assez de ferraille !" COLOR_RESET "\n");
    }
    else {
        printf("Le marchand s'éloigne en maugréant.\n");
    }

    finaliserEvenement(joueur);
    attendreJoueur();
}

void evenementLoterie(Vaisseau *joueur) {
    printf("\n" COLOR_MAGENTA "🎰 [CASINO SPATIAL]" COLOR_RESET " Une station de divertissement scintille au loin.\n");
    printf("\"Approchez ! Tentez votre chance ! Doublez votre mise ou repartez les soutes vides !\"\n");
    
    if (joueur->ferraille < 10) {
        printf("\nLe videur vous regarde de haut : \"Revenez quand vous aurez au moins 10 Ferrailles.\"\n");
        return;
    }

    printf("\n1. Parier 10 Ferrailles (Gain : x2)\n");
    printf("2. Parier 50 Ferrailles (Gain : x3 - Difficile)\n");
    printf("3. Passer votre chemin\n");
    printf(COLOR_YELLOW "> " COLOR_RESET);

    int choix;
    scanf("%d", &choix);

    if (choix == 1) {
        joueur->ferraille -= 10;
        printf("\nLancement de la machine");
        for(int i=0; i<3; i++) { printf("."); fflush(stdout); SLEEP_MS(500); }

        unsigned int seedUnique = joueur->seedSecteur + (joueur->distanceParcourue * 660);
        srand(seedUnique);
        int r = rand() % 100;

        if (r < 45) { // 45% de chance de gagner
            printf(COLOR_GREEN " GAGNÉ ! +20 Ferrailles !" COLOR_RESET "\n");
            joueur->ferraille += 20;
        } else {
            printf(COLOR_RED " PERDU... La machine encaisse vos jetons." COLOR_RESET "\n");
        }
    } 
    else if (choix == 2) {
        if (joueur->ferraille < 50) {
            printf(COLOR_RED "Vous n'avez pas assez pour cette table !\n" COLOR_RESET);
            return;
        }
        joueur->ferraille -= 50;
        printf("\nLa roue de la fortune tourne");
        for(int i=0; i<3; i++) { printf("."); fflush(stdout); SLEEP_MS(700); }

        unsigned int seedUnique = joueur->seedSecteur + (joueur->distanceParcourue * 661);
        srand(seedUnique);
        int r = rand() % 100;

        if (r < 25) { // 25% de chance seulement (Gros lot)
            printf(COLOR_YELLOW " JACKPOT !!! +150 Ferrailles !" COLOR_RESET "\n");
            joueur->ferraille += 150;
        } else {
            printf(COLOR_RED " RIEN... Le casino gagne toujours à la fin." COLOR_RESET "\n");
        }
    } else {
        printf("Vous gardez votre argent pour des réparations plus urgentes.\n");
    }

    finaliserEvenement(joueur);
    attendreJoueur();
}
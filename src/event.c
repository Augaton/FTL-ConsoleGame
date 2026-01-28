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
        
        // --- 1. EN-TÊTE ---
        printf(COLOR_CYAN "╔══════════════════════════════════════════════════════════╗\n");
        printf("║ " COLOR_BOLD "%-18s" COLOR_RESET COLOR_CYAN "CONSOLE DE NAVIGATION   SECTEUR: %02d/%d ║\n", 
               joueur->nom, joueur->distanceParcourue, joueur->distanceObjectif);
        printf("╠══════════════════════════════════════════════════════════╣" COLOR_RESET "\n");

        // --- 2. STATUT DU VAISSEAU (Dynamique) ---
        // On détermine la couleur et le message selon les PV restants
        char *couleurStatut;
        char *texteStatut;
        float ratioCoque = (float)joueur->coque / (float)joueur->coqueMax;

        if (ratioCoque > 0.7) {
            couleurStatut = COLOR_GREEN; texteStatut = "NOMINAL";
        } else if (ratioCoque > 0.3) {
            couleurStatut = COLOR_YELLOW; texteStatut = "ATTENTION";
        } else {
            couleurStatut = COLOR_RED; texteStatut = "CRITIQUE";
        }

        // Affichage Ligne Coque + Statut Global
        printf(COLOR_CYAN "║ " COLOR_RESET "COQUE: ");
        if (ratioCoque <= 0.3) printf(COLOR_RED); // Met les chiffres en rouge si danger
        printf("%02d/%02d " COLOR_RESET, joueur->coque, joueur->coqueMax);
        
        // On remplit l'espace pour aligner le statut à droite
        printf("                       STATUT: %s%-12s" COLOR_CYAN " ║\n", couleurStatut, texteStatut);


        // --- 3. GESTION DES BOUCLIERS ---
        printf(COLOR_CYAN "║ " COLOR_RESET "SHIELD: ");
        int nbBoucliersAffiches = 0;
        for(int i=0; i < joueur->systemeBouclier.efficacite; i++) {
            printf(i < joueur->bouclierActuel ? COLOR_CYAN "⬢ " : COLOR_RED "⬡ ");
            nbBoucliersAffiches++;
        }
        
        // Calcul précis du padding pour fermer la boite
        // La ligne fait 58 chars de large (interne).
        // "SHIELD: " = 9 chars. Chaque bouclier "x " = 2 chars.
        int espaceOccupe = 9 + (nbBoucliersAffiches * 2);
        int padding = 58 - espaceOccupe;
        for(int i=0; i<padding; i++) printf(" ");
        printf(COLOR_CYAN "║\n");


        // --- 4. RESSOURCES ---
        printf("╠══════════════════════════════════════════════════════════╣\n");
        printf("║ " COLOR_YELLOW "⚡ " COLOR_RESET "CARBURANT: %-3d  " COLOR_YELLOW "⚓ " COLOR_RESET "FERRAILLE: %-4d  " COLOR_YELLOW "🚀 " COLOR_RESET "MISSILES: %-3d " COLOR_CYAN " ║\n", 
                joueur->carburant, joueur->ferraille, joueur->missiles);


        // --- 5. BARRE DE PROGRESSION (Alignement Fixe) ---
        printf("╠══════════════════════════════════════════════════════════╣\n");
        printf("║ SAUT: [");
        
        // On dessine la barre
        int tailleBarre = 39; // Taille fixe visuelle de la barre
        // Calcul de la position du vaisseau dans cette barre fixe (produit en croix)
        int posVaisseau = (joueur->distanceParcourue * tailleBarre) / joueur->distanceObjectif;
        if (posVaisseau >= tailleBarre) posVaisseau = tailleBarre - 1;

        for(int i=0; i<tailleBarre; i++) {
            if (i < posVaisseau) printf(COLOR_GREEN "=");
            else if (i == posVaisseau) printf(COLOR_YELLOW "✈");
            else printf(COLOR_RESET "·");
        }
        printf(COLOR_RESET "]");
        
        // Padding final pour fermer la boite
        // "SAUT: [" (7 chars) + Barre (40 chars) + "]" (1 char) = 48 chars
        // 58 - 48 = 10 espaces restants
        for(int i=0; i<10; i++) printf(" ");
        printf(COLOR_CYAN "║\n");
        printf("╚══════════════════════════════════════════════════════════╝" COLOR_RESET "\n");

        // --- MENU ACTIONS ---
        printf("\n" COLOR_CYAN "  [ ORDRES DE MISSION ]" COLOR_RESET "\n");
        printf(COLOR_BOLD "  1." COLOR_RESET " ENGAGER LE SAUT SPATIAL " COLOR_YELLOW "( -1 ⚡ )" COLOR_RESET "\n");
        printf(COLOR_BOLD "  2." COLOR_RESET " GÉRER LE VAISSEAU / INVENTAIRE\n");
        printf(COLOR_BOLD "  3." COLOR_RESET " ABANDONNER LA MISSION\n");
        
        printf("\n" COLOR_YELLOW " COMMANDE > " COLOR_RESET);

        if (scanf("%d", &choix) != 1) {
            int c; while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }
        // Nettoyage buffer
        int c; while ((c = getchar()) != '\n' && c != EOF); 

        // --- TRAITEMENT DES CHOIX ---
        if (choix == 1) {
             if (joueur->carburant > 0) {
                lancerSequenceDeSaut(joueur); 
             } else {
                 printf(COLOR_RED "\n[ERREUR] RÉSERVOIRS VIDES ! Impossible de passer en vitesse lumière.\n" COLOR_RESET);
                 printf("Vous devez attendre un miracle ou un signal de détresse...\n");
                 // Ici, tu pourrais implémenter une mécanique de "Wait" (Attendre) qui consomme de la nourriture/vie
                 // Pour l'instant, on bloque juste.
                 SLEEP_MS(2000);
             }
        }
        else if (choix == 2) {
            menuEtatVaisseau(joueur);
        }
        else if (choix == 3) {
            // AJOUT D'UNE CONFIRMATION DE SÉCURITÉ
            char confirm;
            printf(COLOR_RED "\n[DANGER] Etes-vous sûr de vouloir autodétruire le vaisseau ? (o/n) > " COLOR_RESET);
            scanf("%c", &confirm);
            if (confirm == 'o' || confirm == 'O') {
                printf(COLOR_RED "\nProtocole d'autodestruction engagé...\n" COLOR_RESET);
                SLEEP_MS(1000);
                joueur->coque = 0;
                continuerMenu = 0;
            } else {
                printf(COLOR_GREEN "Annulation. Retour au poste de pilotage.\n" COLOR_RESET);
                SLEEP_MS(800);
            }
        } 
        else if (choix == 99) {
            ouvrirMenuDebug(joueur);
        }
    }
}

void lancerSequenceDeSaut(Vaisseau *joueur) {
    // Génération des destinations potentielles
    const char* baliseA = inspecterBalise();
    const char* baliseB = inspecterBalise();
    int choixSaut;

    printf("\n" COLOR_YELLOW "─── CALCUL DES TRAJECTOIRES FTL ───" COLOR_RESET "\n");
    printf("1. "); afficherDestinationColoree(baliseA); printf("\n");
    printf("2. "); afficherDestinationColoree(baliseB); printf("\n");
    
    // --- NOUVELLE OPTION ---
    printf(COLOR_RED "0. ANNULER LA PROCÉDURE (Retour au cockpit)" COLOR_RESET "\n");
    
    printf(COLOR_YELLOW "\n Destination > " COLOR_RESET);
    
    // Lecture sécurisée basique
    if (scanf("%d", &choixSaut) != 1) {
        int c; while ((c = getchar()) != '\n' && c != EOF);
        choixSaut = 0; // Par défaut, on annule si entrée invalide
    }

    // --- GESTION DU RETOUR ---
    if (choixSaut == 0) {
        printf(COLOR_CYAN "\nCalculs de trajectoire annulés. Moteurs en veille.\n" COLOR_RESET);
        SLEEP_MS(800);
        return; // On quitte la fonction, on revient donc à menuVoyage
    }

    // Si on est ici, c'est qu'on saute. On définit la destination.
    const char* destination = (choixSaut == 1) ? baliseA : baliseB;
    // (Petite sécurité : si le joueur tape 3, il ira vers B par défaut, ou tu peux mettre une boucle while)
    if (choixSaut != 1 && choixSaut != 2) destination = baliseB; 


    // --- MISE À JOUR DU SECTEUR POUR LA SAUVEGARDE ---
    strncpy(joueur->secteurActuel, destination, 49);
    joueur->secteurActuel[49] = '\0'; 

    // --- CONSOMMATION DE CARBURANT ---
    // (Note : J'ai gardé ta logique, mais attention : si le joueur n'a pas de fuel
    // dans menuVoyage, il ne devrait même pas pouvoir entrer ici normalement)
    if (strcmp(destination, "Nebuleuse (Inconnu - Gratuit)") != 0) {
        if (joueur->carburant > 0) {
            joueur->carburant--;
        }
        else {
            // Cas rare si la vérification n'est pas faite avant
            printf(COLOR_RED "\n[ALERTE] Panne de carburant ! Dérive critique...\n" COLOR_RESET);
            joueur->coque -= 2;
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

    // Retour au repos après l'événement
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
        case 5: evenementDetresse(joueur); break;
        case 6: evenementLoterie(joueur); break;
        case 7: evenementPeagePirate(joueur); break;
        case 8: evenementErmite(joueur); break;
        case 9: evenementStationMercenaire(joueur); break;
    }

    // Une fois l'événement choisi et traité, on remet le temps réel 
    // pour que les futurs jets de dés (combat, etc.) ne soient pas prévisibles.
    srand((unsigned int)time(NULL));
}

// LISTE DES ÉVÉNEMENTS

void evenementDetresse(Vaisseau *joueur) {
    // --- SETUP DE LA SEED (Anti-Triche) ---
    unsigned int seedUnique = joueur->seedSecteur + (joueur->distanceParcourue * 210);
    srand(seedUnique);

    printf("\n" COLOR_YELLOW "╔════ [ SIGNAL DE DÉTRESSE ] ═══════════════════════════════════╗\n");
    printf("║ Un transporteur civil est coincé dans un champ d'astéroïdes.  ║\n");
    printf("║ Sa coque est percée et il demande de l'aide immédiate.        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n" COLOR_RESET);

    // --- OPTIONS ---
    printf("1. Tenter une manoeuvre de remorquage (Risqué - 60%% Succès)\n");
    
    // Option Bleue : BOUCLIER
    int aBouclierFort = (joueur->systemeBouclier.rang >= 2);
    if (aBouclierFort) {
        printf(COLOR_CYAN "2. (Bouclier Mk2+) Étendre vos boucliers pour les protéger (100%% Sûr)\n" COLOR_RESET);
    }

    // Option Bleue : MOTEURS
    int aMoteursRapides = (joueur->moteurs >= 4);
    if (aMoteursRapides) {
        printf(COLOR_CYAN "3. (Moteurs Lvl 4) Slalom rapide pour les extraire (100%% Sûr)\n" COLOR_RESET);
    }
    
    // Option Ressources
    printf("4. Leur envoyer un drone de réparation (Coût: 1 Missile)\n");
    printf("5. Ignorer le signal\n");

    printf(COLOR_YELLOW "> " COLOR_RESET);
    int choix;
    if (scanf("%d", &choix) != 1) {
        int c; while ((c = getchar()) != '\n' && c != EOF);
        choix = 5;
    }

    // --- RÉSOLUTION ---
    printf("\n");
    
    // Cas 1 : Manoeuvre Risquée (Standard)
    if (choix == 1) {
        int jet = rand() % 100;
        if (jet < 60) { // 60% Réussite
            int reward = 25 + (rand() % 15);
            printf(COLOR_GREEN "Succès ! Vous stabilisez leur vaisseau.\n" COLOR_RESET);
            printf("Ils vous offrent " COLOR_YELLOW "%d Ferrailles" COLOR_RESET " pour vous remercier.\n", reward);
            joueur->ferraille += reward;
        } 
        else if (jet < 85) { // 25% Échec (Dégâts)
            printf(COLOR_RED "Échec ! Un astéroïde percute votre flanc pendant la manoeuvre.\n" COLOR_RESET);
            printf("Coque -4\n");
            joueur->coque -= 4;
        } 
        else { // 15% PIÈGE (Combat !)
            printf(COLOR_RED "C'EST UN PIÈGE ! Le transporteur était un leurre holographique !\n" COLOR_RESET);
            printf("Un chasseur pirate sort de l'ombre d'un astéroïde !\n");
            SLEEP_MS(1500);
            
            Vaisseau pirate = genererEnnemi(joueur->distanceParcourue, rand());
            lancerCombat(joueur, &pirate);
        }
    }
    
    // Cas 2 & 3 : Options Bleues (Réussite 100%)
    else if ((choix == 2 && aBouclierFort) || (choix == 3 && aMoteursRapides)) {
        printf(COLOR_CYAN "Grâce à vos systèmes avancés, le sauvetage est une formalité.\n" COLOR_RESET);
        SLEEP_MS(1000);
        
        int scrap = 30 + (rand() % 20);
        int fuel = 2 + (rand() % 3);
        
        printf("Le capitaine civil est impressionné : \"Beau pilotage ! Prenez ça.\"\n");
        printf(COLOR_GREEN "+%d Ferrailles, +%d Carburant\n" COLOR_RESET, scrap, fuel);
        joueur->ferraille += scrap;
        joueur->carburant += fuel;
    }
    
    // Cas 4 : Utiliser une ressource
    else if (choix == 4) {
        if (joueur->missiles > 0) {
            joueur->missiles--;
            printf("Le drone répare leur coque automatiquement.\n");
            printf(COLOR_GREEN "Gain : +25 Ferrailles (Net)\n" COLOR_RESET);
            joueur->ferraille += 25;
        } else {
            printf(COLOR_RED "Vous cherchez un drone... mais votre stock est vide !\n" COLOR_RESET);
            printf("Le temps que vous réalisiez, le vaisseau a dérivé trop loin.\n");
        }
    }
    
    else {
        printf("Vous coupez les communications et continuez votre route.\n");
    }

    // Reset du random pour la suite du jeu
    srand(time(NULL));

    finaliserEvenement(joueur);
    attendreJoueur();
}

void evenementEpaveDerivante(Vaisseau *joueur) {
    // --- SETUP SEED ---
    unsigned int seedUnique = joueur->seedSecteur + (joueur->distanceParcourue * 330);
    srand(seedUnique);

    printf("\n" COLOR_CYAN "╔════ [ ÉPAVE DÉRIVANTE ] ════════════════════════════════╗" COLOR_RESET "\n");
    printf("║ Les senseurs détectent un Croiseur Automatisé de classe Guerre.║\n");
    printf("║ Il semble inactif et dérive dans le vide.                  ║\n");
    printf("║ Des signatures énergétiques faibles sont détectées.        ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");

    printf("1. Envoyer l'équipe de récupération (Gros gain potentiel / Risque Élevé)\n");
    printf("2. Scanner et récupérer les débris flottants (Gain faible / Sûr)\n");

    // Option Bleue : ARMES
    int aArmesPuissantes = (joueur->systemeArme.rang >= 2); 
    if (aArmesPuissantes) {
        printf(COLOR_CYAN "3. (Armes Mk2+) Tir chirurgical pour décrocher la soute (Moyen / Sûr)\n" COLOR_RESET);
    }

    // Option Ressource : MISSILE
    printf("4. Tirer un missile pour créer une brèche (Coût: 1 Missile)\n");

    printf(COLOR_YELLOW "> " COLOR_RESET);
    int choix;
    if (scanf("%d", &choix) != 1) {
        int c; while ((c = getchar()) != '\n' && c != EOF);
        choix = 2; 
    }

    printf("\n");

    // --- RÉSOLUTION ---
    
    // CAS 1 : RISQUE (Envoyer l'équipage)
    if (choix == 1) {
        int jet = rand() % 100;
        
        if (jet < 45) { // 45% SUCCÈS
            int scrap = 40 + (rand() % 30);
            int fuel = 2 + (rand() % 4);
            printf(COLOR_GREEN "Succès ! Le système de défense est resté éteint.\n" COLOR_RESET);
            printf("Votre équipage revient les bras chargés : +%d Ferrailles, +%d Carburant.\n", scrap, fuel);
            joueur->ferraille += scrap;
            joueur->carburant += fuel;
        } 
        else if (jet < 80) { // 35% ACCIDENT (Dégâts)
            printf(COLOR_RED "ALERTE ! Une tourelle de défense s'active brièvement et tire sur la navette !\n" COLOR_RESET);
            printf("Vous récupérez l'équipage de justesse, mais la coque a pris un coup. Coque -5.\n");
            joueur->coque -= 5;
        } 
        else { // 20% RÉVEIL DU DRONE (Combat Boss)
            printf(COLOR_RED "PIÈGE !!! CE N'EST PAS UNE ÉPAVE !\n" COLOR_RESET);
            SLEEP_MS(800);
            printf(COLOR_MAGENTA ">>> DÉTECTION : RÉACTIVATION DU NOYAU IA <<<\n" COLOR_RESET);
            printf("Le croiseur s'illumine. Les armes se verrouillent sur vous !\n");
            SLEEP_MS(1500);

            // Génération d'un ennemi spécifique : Le "Zombie"
            // Il a peu de vie (car c'est une épave) mais tape fort
            Vaisseau drone = genererEnnemi(joueur->distanceParcourue + 2, rand()); // +2 pour le rendre fort
            strcpy(drone.nom, "Proto-Croiseur IA (Endommagé)");
            drone.coque = 15; // Coque fragile car "épave"
            drone.coqueMax = 15;
            drone.systemeArme.efficacite += 2; // Armes très dangereuses
            drone.moteurs = 0; // Pas d'esquive (moteurs HS)

            printf("\n" COLOR_RED "COMBAT ENGAGÉ CONTRE L'IA !" COLOR_RESET "\n");
            lancerCombat(joueur, &drone);
            
            // Si le joueur gagne, il aura le loot du combat + un bonus "récupération d'IA"
            if (joueur->coque > 0 && drone.coque <= 0) {
                printf(COLOR_CYAN "\nVous analysez les restes du processeur IA...\n" COLOR_RESET);
                printf(COLOR_GREEN "Gain de données : +15 Ferrailles supplémentaires.\n" COLOR_RESET);
                joueur->ferraille += 15;
            }
        }
    }
    
    // CAS 2 : SÉCURITÉ (Scan)
    else if (choix == 2) {
        int scrap = 10 + (rand() % 5);
        printf("Le scan confirme que le vaisseau est en veille active. Trop dangereux d'approcher.\n");
        printf("Vous récupérez juste quelques débris périphériques. +%d Ferrailles.\n", scrap);
        joueur->ferraille += scrap;
    }

    // CAS 3 : OPTION BLEUE (Armes)
    else if (choix == 3 && aArmesPuissantes) {
        int scrap = 30 + (rand() % 10);
        printf(COLOR_CYAN "Tir précis ! Vous détruisez le générateur d'armes du drone avant qu'il ne s'active.\n" COLOR_RESET);
        printf("Vous pouvez piller l'épave sans risque.\n");
        printf(COLOR_GREEN "Gain : +%d Ferrailles.\n" COLOR_RESET, scrap);
        joueur->ferraille += scrap;
    }

    // CAS 4 : MISSILE
    else if (choix == 4) {
        if (joueur->missiles > 0) {
            joueur->missiles--;
            printf("L'impact du missile secoue l'épave, mais ne la réveille pas.\n");
            int lootMissile = 2 + (rand() % 3);
            int scrap = 20 + (rand() % 10);
            printf("Vous récupérez du matériel dans la brèche.\n");
            printf(COLOR_GREEN "+%d Ferrailles, +%d Missiles\n" COLOR_RESET, scrap, lootMissile);
            joueur->ferraille += scrap;
            joueur->missiles += lootMissile;
        } else {
            printf(COLOR_RED "Pas de missiles ! Vous devez abandonner l'opération.\n" COLOR_RESET);
        }
    }
    
    else {
        printf("Mieux vaut ne pas réveiller le géant qui dort. Vous partez.\n");
    }

    // Reset RNG
    srand(time(NULL));

    finaliserEvenement(joueur);
    attendreJoueur();
}

void evenementPluieAsteroides(Vaisseau *joueur) {
    printf("\n" COLOR_YELLOW "[DANGER] CHAMP D'ASTEROIDES DENSE DETECTE !" COLOR_RESET "\n");
    printf("Des rochers de la taille d'une lune vous barrent la route.\n");

    printf("\n1. Tenter de passer en manœuvrant (Test Moteurs)\n");
    
    // OPTION BLEUE : SI BOUCLIER PUISSANT
    if (joueur->systemeBouclier.efficacite >= 3) {
        printf(COLOR_CYAN "2. [BOUCLIER LVL 3] Surcharger les boucliers et foncer (Sûr)\n" COLOR_RESET);
    }
    
    // OPTION BLEUE : SI MISSILES DISPO
    if (joueur->missiles >= 2) {
        printf(COLOR_RED "3. [2 MISSILES] Se frayer un chemin à l'explosif (Gain de temps)\n" COLOR_RESET);
    }

    printf(COLOR_YELLOW "> " COLOR_RESET);
    int choix;
    scanf("%d", &choix);

    if (choix == 2 && joueur->systemeBouclier.efficacite >= 3) {
        printf(COLOR_GREEN "\nLes rochers ricochent inoffensivement sur vos boucliers surchargés.\n");
        printf("Vous en profitez pour scanner les débris : +10 Ferraille.\n" COLOR_RESET);
        joueur->ferraille += 10;
    } 
    else if (choix == 3 && joueur->missiles >= 2) {
        joueur->missiles -= 2;
        printf(COLOR_RED "\nBOUM ! BOUM ! " COLOR_RESET "Vous pulvérisez les obstacles.\n");
        printf("Passage dégagé sans encombre.\n");
    } 
    else {
        // Logique classique (Esquive)
        int chanceEsquive = 40 + (joueur->moteurs * 10);
        int r = rand() % 100;
        printf("\nVous tentez de piloter à travers le chaos...\n");
        SLEEP_MS(800);

        if (r < chanceEsquive) {
            printf(COLOR_GREEN "Pilotage expert ! Vous évitez le pire.\n" COLOR_RESET);
        } else {
            int degats = (rand() % 3) + 2;
            printf(COLOR_RED "IMPACT ! Un astéroïde percute le flanc. Coque -%d\n" COLOR_RESET, degats);
            joueur->coque -= degats;
        }
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
    // NOUVELLE OPTION D'ATTAQUE
    printf(COLOR_RED "4. Attaquer le marchand (Piraterie)\n" COLOR_RESET);
    
    printf(COLOR_YELLOW "> " COLOR_RESET);
    int choix;
    // Utilisation de ta méthode de lecture sécurisée ou scanf classique
    if (scanf("%d", &choix) != 1) {
        int c; while ((c = getchar()) != '\n' && c != EOF);
        choix = 0; // Invalide
    }

    if (choix == 1) {
        if (joueur->ferraille >= 10) {
            joueur->ferraille -= 10;
            joueur->carburant += 2;
            printf(COLOR_GREEN "Transaction réussie. (+2 Carburant)" COLOR_RESET "\n");
        } else printf(COLOR_RED "Pas assez de ferraille !" COLOR_RESET "\n");
    } 
    else if (choix == 2) {
        if (joueur->ferraille >= 15) {
            joueur->ferraille -= 15;
            joueur->missiles += 3;
            printf(COLOR_GREEN "Transaction réussie. (+3 Missiles)" COLOR_RESET "\n");
        } else printf(COLOR_RED "Pas assez de ferraille !" COLOR_RESET "\n");
    }
    else if (choix == 4) {
        printf(COLOR_RED "\nVous armez vos canons. Le marchand panique sur la radio !\n" COLOR_RESET);
        printf("\"Espèce de fou ! Vous allez le regretter !\"\n");
        SLEEP_MS(1000);

        // On génère un ennemi basé sur le niveau actuel
        Vaisseau marchand = genererEnnemi(joueur->distanceParcourue, rand());
        
        // On personnalise l'ennemi pour qu'il ressemble à un marchand
        strcpy(marchand.nom, "Transporteur Armé");
        marchand.coqueMax += 5;       // Un marchand a souvent une grosse coque (soute)
        marchand.coque = marchand.coqueMax;
        marchand.moteurs = 1;         // Souvent lent et lourd
        
        // On lance le combat
        lancerCombat(joueur, &marchand);

        if (joueur->chargeFTL >= joueur->maxchargeFTL) {
            printf(COLOR_YELLOW "\n[ESCAPADE] Vous activez vos moteurs FTL pour fuir, laissant le marchand en plan.\n" COLOR_RESET);
            SLEEP_MS(1000);
            return;
        }

        if (joueur->coque > 0 && marchand.coque <= 0) {
            printf(COLOR_YELLOW "\n[PILLAGE] Vous forcez la soute de l'épave fumante...\n" COLOR_RESET);
            SLEEP_MS(800);

            // Génération du butin "Marchandise"
            int volFuel = (rand() % 3) + 2;     // 2 à 4 Carburant
            int volMissiles = (rand() % 3) + 2; // 2 à 4 Missiles

            joueur->carburant += volFuel;
            joueur->missiles += volMissiles;

            printf("Vous récupérez : " COLOR_CYAN "+%d Carburant" COLOR_RESET " et " COLOR_RED "+%d Missiles" COLOR_RESET " !\n", 
                   volFuel, volMissiles);
            
            SLEEP_MS(1500);
        }
        
        // Note : Si le joueur gagne, 'lancerCombat' gère déjà le butin (ferraille/arme).
        // Si le joueur fuit ou meurt, 'lancerCombat' gère aussi la suite.
        return; // On quitte la fonction car le combat gère la fin de l'event
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
    
    // Vérification argent minimum pour jouer
    if (joueur->ferraille < 10) {
        printf("\nLe videur vous regarde de haut : \"Revenez quand vous aurez au moins 10 Ferrailles.\"\n");
    } else {
        printf("\n1. Parier 10 Ferrailles (Gain : x2)\n");
        printf("2. Parier 50 Ferrailles (Gain : x3 - Difficile)\n");
    }
    
    printf("3. Passer votre chemin\n");
    
    // --- NOUVELLE OPTION BRAQUAGE ---
    printf(COLOR_RED "4. BRAQUER LE CASINO (Suicidaire - 3 Vagues d'ennemis)\n" COLOR_RESET);
    
    printf(COLOR_YELLOW "> " COLOR_RESET);

    int choix;
    // Sécurisation basique
    if (scanf("%d", &choix) != 1) {
        int c; while ((c = getchar()) != '\n' && c != EOF);
        choix = 3;
    }

    // --- LOGIQUE DES PARIS (Inchangée) ---
    if (choix == 1 && joueur->ferraille >= 10) {
        joueur->ferraille -= 10;
        printf("\nLancement de la machine");
        for(int i=0; i<3; i++) { printf("."); fflush(stdout); SLEEP_MS(500); }
        unsigned int seedUnique = joueur->seedSecteur + (joueur->distanceParcourue * 660);
        srand(seedUnique);
        if (rand() % 100 < 45) { 
            printf(COLOR_GREEN " GAGNÉ ! +20 Ferrailles !" COLOR_RESET "\n");
            joueur->ferraille += 20;
        } else {
            printf(COLOR_RED " PERDU... La machine encaisse vos jetons." COLOR_RESET "\n");
        }
    } 
    else if (choix == 2 && joueur->ferraille >= 50) {
        joueur->ferraille -= 50;
        printf("\nLa roue de la fortune tourne");
        for(int i=0; i<3; i++) { printf("."); fflush(stdout); SLEEP_MS(700); }
        unsigned int seedUnique = joueur->seedSecteur + (joueur->distanceParcourue * 661);
        srand(seedUnique);
        if (rand() % 100 < 25) { 
            printf(COLOR_YELLOW " JACKPOT !!! +150 Ferrailles !" COLOR_RESET "\n");
            joueur->ferraille += 150;
        } else {
            printf(COLOR_RED " RIEN... Le casino gagne toujours à la fin." COLOR_RESET "\n");
        }
    } 
    
if (choix == 4) {
        printf(COLOR_RED "\n[ALARME] VOUS ACTIVEZ VOS ARMES ! TOUTE LA STATION PASSE EN ALERTE ROUGE !" COLOR_RESET "\n");
        SLEEP_MS(1000);
        
        // --- VAGUE 1 : SÉCURITÉ DE BASE ---
        printf(COLOR_YELLOW "\n--- VAGUE 1/3 : DRONE DE SÉCURITÉ ---\n" COLOR_RESET);
        Vaisseau drone = genererEnnemi(joueur->distanceParcourue, rand());
        strcpy(drone.nom, "Drone Sécurité Mk1");
        drone.coqueMax = 10; 
        drone.coque = 10;
        
        lancerCombat(joueur, &drone);

        // VERIFICATION VAGUE 1
        if (joueur->coque <= 0) return; // Tu es mort

        // Si le FTL est chargé au max, c'est que tu as fui le combat
        if (joueur->chargeFTL >= joueur->maxchargeFTL) {
            printf(COLOR_YELLOW "\nVous avez pris la fuite ! Le braquage est annulé.\n" COLOR_RESET);
            joueur->chargeFTL = 0;      // IMPORTANT : On vide la charge
            finaliserEvenement(joueur); 
            return;
        }

        // --- VAGUE 2 : GARDE D'ÉLITE ---
        printf(COLOR_YELLOW "\n--- VAGUE 2/3 : GARDE D'ÉLITE ---\n" COLOR_RESET);
        SLEEP_MS(1000);
        printf("Les portes blindées s'ouvrent, un vaisseau lourd sort du hangar !\n");
        
        Vaisseau garde = genererEnnemi(joueur->distanceParcourue + 2, rand()); 
        strcpy(garde.nom, "Croiseur Blindé Casino");
        garde.coqueMax += 10;
        garde.coque = garde.coqueMax;
        garde.systemeArme.efficacite += 1;
        
        lancerCombat(joueur, &garde);

        // VERIFICATION VAGUE 2
        if (joueur->coque <= 0) return;

        if (joueur->chargeFTL >= joueur->maxchargeFTL) {
            printf(COLOR_YELLOW "\nVous abandonnez le butin et fuyez vers l'hyper-espace !\n" COLOR_RESET);
            joueur->chargeFTL = 0;
            finaliserEvenement(joueur);
            return;
        }

        // --- VAGUE 3 : LE BOSS DU CASINO ---
        printf(COLOR_RED "\n--- VAGUE 3/3 : LE VAISSEAU DU GÉRANT ---\n" COLOR_RESET);
        SLEEP_MS(1000);
        printf("\"Vous m'avez coûté une fortune ! Vous allez le payer de votre sang !\"\n");

        Vaisseau boss = genererEnnemi(joueur->distanceParcourue + 5, rand());
        strcpy(boss.nom, "Yacht de Luxe Armé");
        boss.coqueMax = 40;
        boss.coque = 40;
        boss.systemeBouclier.efficacite += 1;
        
        lancerCombat(joueur, &boss);

        // VERIFICATION VAGUE 3
        if (joueur->coque <= 0) return;

        if (joueur->chargeFTL >= joueur->maxchargeFTL) {
            printf(COLOR_YELLOW "\nSi près du but... Mais la vie est plus importante. Vous fuyez.\n" COLOR_RESET);
            joueur->chargeFTL = 0;
            finaliserEvenement(joueur);
            return;
        }

        // --- VICTOIRE TOTALE ---
        printf(COLOR_YELLOW "\n============================================\n");
        printf("       BRAQUAGE RÉUSSI ! LE CASINO EST À VOUS       \n");
        printf("============================================" COLOR_RESET "\n");
        SLEEP_MS(1000);
        
        int butinScrap = 200 + (rand() % 100);
        int butinFuel = 5 + (rand() % 5);
        int butinMissile = 5 + (rand() % 5);
        
        printf("Vous forcez le coffre principal...\n");
        printf(COLOR_GREEN "+%d Ferrailles\n", butinScrap);
        printf("+%d Carburant\n", butinFuel);
        printf("+%d Missiles\n" COLOR_RESET, butinMissile);

        joueur->ferraille += butinScrap;
        joueur->carburant += butinFuel;
        joueur->missiles += butinMissile;
        
        printf("\nVous utilisez les docks du casino pour effectuer des réparations d'urgence (+10 Coque).\n");
        joueur->coque += 10;
        if(joueur->coque > joueur->coqueMax) joueur->coque = joueur->coqueMax;
    }
    
    else if (choix != 1 && choix != 2) {
        printf("Vous gardez votre argent pour des réparations plus urgentes.\n");
    }

    finaliserEvenement(joueur);
    attendreJoueur();
}

void evenementPeagePirate(Vaisseau *joueur) {
    printf("\n" COLOR_RED "[ALERTE PROXIMITE]" COLOR_RESET " Un croiseur pirate active ses armes.\n");
    printf(COLOR_YELLOW "\"Hé toi ! C'est notre territoire. Paye la taxe ou deviens une épave.\"\n" COLOR_RESET);
    printf("Demande : 15 Ferrailles.\n");

    printf("\n1. Payer 15 Ferrailles (Éviter le combat)\n");
    printf("2. Refuser et engager le combat !\n");

    // OPTION BLEUE : ARME PUISSANTE
    if (joueur->systemeArme.rang >= 3) {
        printf(COLOR_RED "3. [ARME LVL 3] Tirer un coup de semonce (Intimidation)\n" COLOR_RESET);
    }

    printf(COLOR_YELLOW "> " COLOR_RESET);
    int choix;
    scanf("%d", &choix);

    if (choix == 1) {
        if (joueur->ferraille >= 15) {
            joueur->ferraille -= 15;
            printf(COLOR_CYAN "\"Sage décision. Filez avant qu'on change d'avis.\"\n" COLOR_RESET);
            finaliserEvenement(joueur);
            attendreJoueur();
        } else {
            printf(COLOR_RED "\"Tu te moques de moi ?! T'as même pas de quoi payer ! A L'ATTAQUE !\"\n" COLOR_RESET);
            SLEEP_MS(1000);
            // On lance le combat car pas assez d'argent
            Vaisseau pirate = genererEnnemi(joueur->distanceParcourue, rand());
            lancerCombat(joueur, &pirate);
        }
    }
    else if (choix == 3 && joueur->systemeArme.rang >= 3) {
        printf("\nVous chargez votre %s au maximum et viserez leur pont.\n", joueur->systemeArme.nom);
        SLEEP_MS(800);
        printf(COLOR_GREEN "\"Wow wow ! Calmez-vous ! On savait pas que vous étiez équipés comme ça...\"\n" COLOR_RESET);
        printf("Les pirates s'enfuient en laissant une caisse de ravitaillement (+2 Missiles).\n");
        joueur->missiles += 2;
        finaliserEvenement(joueur);
        attendreJoueur();
    }
    else {
        printf(COLOR_RED "\n\"A L'ABORDAGE !\"\n" COLOR_RESET);
        SLEEP_MS(800);
        Vaisseau pirate = genererEnnemi(joueur->distanceParcourue, rand());
        // On booste un peu le pirate car c'est un événement de "boss" mineur
        pirate.coque += 5; 
        lancerCombat(joueur, &pirate);
    }
}

void evenementErmite(Vaisseau *joueur) {
    printf("\n" COLOR_MAGENTA "[RENCONTRE]" COLOR_RESET " Une station solitaire flotte dans le vide.\n");
    printf("Un vieil homme vous contacte : \"Je peux améliorer ton tas de ferraille... ou le détruire. Hahaha !\"\n");
    
    printf("\n1. Laisser l'ermite bricoler vos moteurs (Risqué)\n");
    printf("2. Lui demander de renforcer la coque (Coût: 10 Ferrailles)\n");
    printf("3. Partir sans rien dire\n");
    
    printf(COLOR_YELLOW "> " COLOR_RESET);
    int choix;
    scanf("%d", &choix);

    if (choix == 1) {
        int r = rand() % 100;
        printf("\nIl tape sur votre moteur avec une clé à molette géante...\n");
        SLEEP_MS(1000);
        
        if (r < 50) {
            joueur->moteurs += 1;
            printf(COLOR_GREEN "INCROYABLE ! Vos moteurs ronronnent comme jamais. (+1 Moteurs)\n" COLOR_RESET);
        } else {
            joueur->coque -= 5;
            printf(COLOR_RED "OUPS ! Il a percé le réservoir de refroidissement. (-5 Coque)\n" COLOR_RESET);
        }
    }
    else if (choix == 2) {
        if (joueur->ferraille >= 10) {
            joueur->ferraille -= 10;
            joueur->coqueMax += 5;
            joueur->coque += 5;
            printf(COLOR_GREEN "\nIl soude des plaques de métal étrange sur votre vaisseau. (+5 Coque Max)\n" COLOR_RESET);
        } else {
            printf("\n\"Pas d'argent, pas de métal !\"\n");
        }
    }
    else {
        printf("\n\"Les jeunes... toujours pressés.\"\n");
    }

    finaliserEvenement(joueur);
    attendreJoueur();
}

// Debug Menu

void ouvrirMenuDebug(Vaisseau *joueur) {
    int choixDebug = 0;
    
    while (choixDebug != 9) {
        effacerEcran();
        printf(COLOR_RED "╔══════════════════════════════════════════════════╗\n");
        printf("║ 🛠️  MENU DEBUG / GOD MODE                       ║\n");
        printf("╚══════════════════════════════════════════════════╝" COLOR_RESET "\n");
        
        printf(COLOR_GREEN " --- RESSOURCES ---" COLOR_RESET "\n");
        printf(" 1. Soin Total + Carburant Max + Missiles Max\n");
        printf(" 2. +1000 Ferrailles\n");
        printf(" 3. Boost Equipement (Arme/Bouclier/Moteur Max)\n");

        printf(COLOR_CYAN "\n --- EVENEMENTS ---" COLOR_RESET "\n");
        printf(" 4. Lancer Casino (Pour tester le braquage)\n");
        printf(" 5. Lancer Marchand (Pour tester l'attaque)\n");
        printf(" 6. Lancer Pluie d'Astéroides\n");
        printf(" 7. Lancer L'Ermite Fou\n");
        printf(" 8. Lancer Signal de Détresse\n");

        printf(COLOR_RED "\n --- COMBAT ---" COLOR_RESET "\n");
        printf(" 100. Spawner le BOSS FINAL (Test Suicide)\n");
        
        printf("\n 101. [QUITTER DEBUG]\n");
        printf(COLOR_YELLOW " DEBUG > " COLOR_RESET);
        
        scanf("%d", &choixDebug);

        if (choixDebug == 1) {
            joueur->coque = joueur->coqueMax;
            joueur->carburant = 20;
            joueur->missiles = 20;
            printf("Stats restaurées.\n");
        }
        else if (choixDebug == 2) {
            joueur->ferraille += 1000;
            printf("Porte-monnaie rempli !\n");
        }
        else if (choixDebug == 3) {
            joueur->systemeArme.rang += 5;
            joueur->systemeArme.efficacite += 10;
            strcpy(joueur->systemeArme.nom, "LASER DE LA MORT");
            
            joueur->systemeBouclier.rang += 5;
            joueur->systemeBouclier.efficacite += 5;
            joueur->bouclierActuel = 5;
            
            joueur->moteurs += 5;
            printf("Vaisseau en mode GOD TIER.\n");
        }
        else if (choixDebug == 4) {
            evenementLoterie(joueur);
        }
        else if (choixDebug == 5) {
            evenementMarchandAmbulant(joueur);
        }
        else if (choixDebug == 6) {
            evenementPluieAsteroides(joueur);
        }
        else if (choixDebug == 7) {
            evenementErmite(joueur);
        }
        else if (choixDebug == 8) {
            evenementDetresse(joueur);
        }
        else if (choixDebug == 100) {
            printf(COLOR_RED "INVOCATION DU BOSS...\n" COLOR_RESET);
            SLEEP_MS(1000);
            Vaisseau boss = genererBossFinal();
            lancerCombat(joueur, &boss);
            // Si on gagne, on reset le flag ennemi pour éviter les bugs
            joueur->ennemiPresent = 0;
        }

        if (choixDebug != 101) {
            SLEEP_MS(500);
            attendreJoueur();
        }
    }
}

void evenementStationMercenaire(Vaisseau *joueur) {
    printf("\n" COLOR_MAGENTA "[STATION]" COLOR_RESET " Bar de l'espace 'Le Trou Noir'.\n");
    printf("Un mercenaire Mantis (Soldat) propose ses services.\n");
    printf("\"Moi tuer pour toi. Toi payer moi.\"\n");

    // Vérifie s'il y a de la place
    int slotLibre = -1;
    for(int i=0; i<3; i++) {
        if (!joueur->equipage[i].estVivant) {
            slotLibre = i;
            break;
        }
    }

    if (slotLibre == -1) {
        printf(COLOR_YELLOW "Hélas, votre vaisseau est complet. Vous ne pouvez pas recruter.\n" COLOR_RESET);
    } else {
        printf("1. Recruter 'Krog' (Soldat) - Prix : 40 Ferrailles\n");
        printf("2. Refuser\n");
        
        int choix;
        printf("> ");
        scanf("%d", &choix);

        if (choix == 1) {
            if (joueur->ferraille >= 40) {
                joueur->ferraille -= 40;
                
                // Création du soldat
                strcpy(joueur->equipage[slotLibre].nom, "Krog");
                joueur->equipage[slotLibre].role = ROLE_SOLDAT;
                joueur->equipage[slotLibre].pv = 120; // Les soldats sont robustes
                joueur->equipage[slotLibre].pvMax = 120;
                joueur->equipage[slotLibre].estVivant = 1;
                joueur->nbMembres++;

                printf(COLOR_GREEN "Krog monte à bord ! Vos dégâts seront augmentés.\n" COLOR_RESET);
            } else {
                printf(COLOR_RED "Pas assez de ferraille ! Krog vous crache dessus.\n" COLOR_RESET);
            }
        }
    }
    finaliserEvenement(joueur);
    attendreJoueur();
}
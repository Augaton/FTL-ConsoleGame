#ifndef VAISSEAU_H
#define VAISSEAU_H

typedef struct {
    char nom[50];
    int rang;
    int efficacite; 
} Composant;

typedef enum {
    ROLE_PILOTE,    // Bonus Esquive
    ROLE_INGENIEUR, // Bonus Réparation passive / Bouclier
    ROLE_SOLDAT     // Bonus Précision / Combat
} TypeRole;

typedef struct {
    char nom[30];
    TypeRole role;
    int pv;
    int pvMax;
    int estVivant; // 1 = Oui, 0 = Mort

    int xp;         // Points d'expérience actuels
    int niveau;     // 0 = Rookie, 1 = Vétéran, 2 = Élite
} Membre;

typedef struct {
    char nom[30];
    int coque;
    int coqueMax;
    int ferraille;
    int carburant;
    int moteurs; 
    int missiles;

    // --- SYSTÈME D'ÉQUIPEMENT ---
    Composant systemeArme;
    Composant systemeBouclier;
    int precision;
    
    // État actuel des boucliers
    int bouclierActuel; 

    // --- NOUVEAU : ÉTAT DES SYSTÈMES (DEBUFFS) ---
    // 0 = OK, >0 = Nombre de tours où le système est HS/Affaibli
    int debuffArme;   
    int debuffMoteur; 

    // --- NAVIGATION ET OBJECTIFS ---
    int distanceParcourue;
    int distanceObjectif;
    char secteurActuel[50];
    unsigned int seedSecteur;

    // --- ÉTAT DU COMBAT ---
    int ennemiPresent;
    int ennemiCoqueActuelle;
    int chargeFTL;
    int maxchargeFTL;


    // Équipage
    Membre equipage[3]; 
    int nbMembres;      // Combien de slots sont occupés

    // Patrouille secteur
    int explorationActuelle; // Combien de fois on a déjà exploré
    int explorationMax;      // Combien de choses il y a à trouver ici
    
} Vaisseau;

void menuEtatVaisseau(Vaisseau *v);
void afficherBarreFTL(int charge);
void initialiserNouvellePartie(Vaisseau *joueur);

// Équipage
void gagnerXP(Membre *m, int montant);


// Pilote
int getBonusEsquive(Vaisseau *v);
int getBonusVitesseFTL(Vaisseau *v); // Chance de charger le FTL 2x plus vite

// Ingénieur
int getBonusRechargeBouclier(Vaisseau *v); // L'ancien bonus
int getBonusCapaciteBouclier(Vaisseau *v); // Le +1 Slot
int tenterReparationAutomatique(Vaisseau *v); // La réparation

// Soldat
int getBonusPrecision(Vaisseau *v);  // Réduction de l'esquive ennemie
int getBonusCritique(Vaisseau *v);   // Chance de crit augmentée
int getBonusDegats(Vaisseau *v);

void subirDegatsEquipage(Vaisseau *v);
const char* getRoleNom(TypeRole role);

#endif
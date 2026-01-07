#ifndef VAISSEAU_H
#define VAISSEAU_H

typedef struct {
    char nom[50];
    int rang;
    int efficacite; 
} Composant;

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
    
    // État actuel des boucliers (énergie disponible)
    int bouclierActuel; 

    // --- NAVIGATION ET OBJECTIFS ---
    int distanceParcourue;
    int distanceObjectif;
    char secteurActuel[50];
    unsigned int seedSecteur;

    // --- ÉTAT DU COMBAT (PERSISTANCE) ---
    int ennemiPresent;
    int ennemiCoqueActuelle;
} Vaisseau;

void afficherVaisseau(Vaisseau *v);

#endif
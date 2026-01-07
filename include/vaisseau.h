#ifndef VAISSEAU_H
#define VAISSEAU_H

typedef struct {
    char nom[30];
    int coque;
    int coqueMax;
    int bouclier;
    int bouclierMax;
    int armes;
    int missiles;
    int ferraille;
    int carburant;
    int distanceParcourue;
    int moteurs; 
    
    // pour sauvegarde de l'état actuel
    char secteurActuel[50];

    unsigned int seedSecteur; // La graine du secteur actuel
    int ennemiCoqueActuelle; 
    int ennemiPresent;
} Vaisseau;

void afficherVaisseau(Vaisseau *v);

#endif
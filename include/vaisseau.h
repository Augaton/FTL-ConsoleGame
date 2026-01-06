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
} Vaisseau;

void afficherVaisseau(Vaisseau *v);

#endif
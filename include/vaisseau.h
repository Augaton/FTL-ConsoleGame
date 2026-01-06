#ifndef VAISSEAU_H
#define VAISSEAU_H

typedef struct {
    char nom[30]; // On ajoute de la place pour le nom
    int coque;
    int coqueMax;
    int bouclier;
    int bouclierMax;
    int armes;
    int missiles;
    int ferraille;
    int carburant;
    int distanceParcourue;
} Vaisseau;

#endif
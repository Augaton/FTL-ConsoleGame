#ifndef EVENEMENTS_H
#define EVENEMENTS_H

#include "vaisseau.h"

void executerEvenement(Vaisseau *joueur, const char *destination);
const char* inspecterBalise();
void menuVoyage(Vaisseau *joueur);
void lancerSequenceDeSaut(Vaisseau *joueur);

void lancerEvenementAleatoire(Vaisseau *joueur);

void evenementDeresse(Vaisseau *joueur);
void evenementMarchandAmbulant(Vaisseau *joueur);
void evenementEpaveDerivante(Vaisseau *joueur);
void evenementPluieAsteroides(Vaisseau *joueur);
void evenementCapsuleSurvie(Vaisseau *joueur);
void evenementAnomalieSpatiale(Vaisseau *joueur);


#endif
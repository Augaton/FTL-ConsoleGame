#ifndef EVENEMENTS_H
#define EVENEMENTS_H

#include "vaisseau.h"

void executerEvenement(Vaisseau *joueur, const char *destination);
const char* inspecterBalise();
void menuVoyage(Vaisseau *joueur);
void lancerSequenceDeSaut(Vaisseau *joueur);
void afficherDestinationColoree(const char* destination);
void descriptionSecteurVide(Vaisseau *joueur);

void lancerEvenementAleatoire(Vaisseau *joueur);

void evenementDetresse(Vaisseau *joueur);
void evenementMarchandAmbulant(Vaisseau *joueur);
void evenementEpaveDerivante(Vaisseau *joueur);
void evenementPluieAsteroides(Vaisseau *joueur);
void evenementCapsuleSurvie(Vaisseau *joueur);
void evenementAnomalieSpatiale(Vaisseau *joueur);
void evenementLoterie(Vaisseau *joueur);
void evenementPeagePirate(Vaisseau *joueur);
void evenementErmite(Vaisseau *joueur);

// ... autres prototypes ...
void ouvrirMenuDebug(Vaisseau *joueur);


#endif
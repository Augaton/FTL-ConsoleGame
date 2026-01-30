#ifndef COMBAT_H
#define COMBAT_H
#include "vaisseau.h"
#include <stdbool.h>

void lancerCombat(Vaisseau *joueur, Vaisseau *ennemi);
void tourCombat(Vaisseau *joueur, Vaisseau *ennemi);
int calculerDegats(int puissanceArme, int niveauMoteur, int bonusCritiqueSoldat);
bool checkEsquive(int chanceEsquiveBase, Vaisseau *defenseur);
void rechargerBoucliers(Vaisseau *v);
Vaisseau genererEnnemi(int secteur, unsigned int seed);
Vaisseau genererBossFinal();
void analyserEnnemi(Vaisseau *joueur, Vaisseau *ennemi);

#endif
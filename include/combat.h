#ifndef COMBAT_H
#define COMBAT_H
#include "vaisseau.h"
#include <stdbool.h>

void lancerCombat(Vaisseau *joueur, Vaisseau *ennemi);
void tourCombat(Vaisseau *joueur, Vaisseau *ennemi);
int calculerDegats(int puissanceArme, int niveauMoteur);
bool checkEsquive(int chanceEsquiveBase, Vaisseau *defenseur);
void rechargerBoucliers(Vaisseau *v);
Vaisseau genererEnnemi(int secteur, unsigned int seed);
Vaisseau genererBossFinal();
int calculerDegats(int puissanceArme, int niveauMoteur);

#endif
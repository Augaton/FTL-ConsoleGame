#ifndef COMBAT_H
#define COMBAT_H
#include "vaisseau.h"
#include <stdbool.h>

void lancerCombat(Vaisseau *joueur);
void tourCombat(Vaisseau *joueur, Vaisseau *ennemi);
bool checkEsquive(int chanceEsquive);
void rechargerBoucliers(Vaisseau *v);
Vaisseau genererEnnemi(int secteur);
Vaisseau genererBossFinal();

#endif
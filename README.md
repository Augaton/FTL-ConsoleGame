# 🚀 FTL Version Console

**FTL Version Console** est un jeu de rôle (RPG) de survie spatiale en ligne de commande écrit en **C**. Inspiré par des classiques comme *FTL: Faster Than Light*, vous commandez un vaisseau spatial à travers de multiple secteurs périlleux pour sauver la galaxie.

![C Language](https://img.shields.io/badge/Language-C-blue.svg)
![License](https://img.shields.io/badge/License-MIT-green.svg)
![Platform](https://img.shields.io/badge/Platform-Linux%20%7F%20macOS-lightgrey.svg)

---

## 🌌 Aperçu du Jeu

Vous incarnez un commandant de vaisseau. Chaque décision compte : gérez vos ressources, améliorez vos systèmes et survivez à des rencontres aléatoires jusqu'au secteur final.

### ✨ Fonctionnalités clés
* **Système de Combat Tactique :** Gérez vos lasers et missiles tout en surveillant votre bouclier et l'esquive de vos moteurs.
* **Événements Aléatoires :** Plus de 10 types d'événements (Anomalies spatiales, capsules de survie, loteries intergalactiques, pluies d'astéroïdes).
* **Gestion de Vaisseau :** Tableau de bord détaillé incluant l'état de la coque, du carburant et de la ferraille (monnaie).
* **Économie Dynamique :** Stations commerciales avec stocks limités, systèmes de promotions et inflation selon la distance parcourue.
* **Immersion ASCII :** Interface graphique riche en couleurs (codes ANSI) et cadres stylisés pour une expérience immersive en terminal.

---

## 🛠️ Installation et Compilation

### Prérequis
* Un compilateur C (**GCC** ou **Clang**).
* **Make** (optionnel, mais recommandé).
* Un terminal compatible **ANSI** (Linux, macOS, ou WSL sur Windows).

### Compilation
Utilisez le `Makefile` inclus pour compiler le projet :

```bash
make
```

Ou compilez manuellement :

```bash
gcc src/*.c -I include -o ftl_game
```

### Exécution

```bash
./space_rogue
```

---

## 🎮 Comment jouer ?

1.  **Saut FTL :** Choisissez entre deux balises de saut. Attention à votre consommation de carburant ! Chaque saut consomme 1 unité.
2.  **Combat :** Si vous croisez un ennemi, gérez vos ressources. Utilisez vos lasers pour économiser vos munitions ou vos missiles pour abréger le combat.
3.  **Upgrade :** Amassez de la ferraille sur les épaves ou via les événements pour améliorer vos moteurs (esquive) ou vos boucliers dans les stations commerciales.
4.  **Survie :** Si votre coque atteint 0, la mission échoue. Gérez vos réparations avec soin.

---

## 📂 Structure du Projet

* `src/` : Fichiers sources (.c) - Logique du jeu, combat, magasin et événements.
* `include/` : Headers (.h) - Définitions des structures (Vaisseau, Ennemi) et prototypes.
* `Makefile` : Script de compilation automatisé pour simplifier le build.

---

## 🛠️ Système de Couleurs & Compatibilité
Le jeu utilise des séquences d'échappement ANSI pour l'affichage des couleurs. Pour une expérience visuelle optimale (cadres et icônes), utilisez un terminal moderne :
* **Linux :** GNOME Terminal, Konsole, xterm.
* **macOS :** Terminal.app ou iTerm2.
* **Windows :** Windows Terminal ou WSL.

---

## 🛡️ Licence
Distribué sous la licence MIT. Voir le fichier `LICENSE` pour plus d'informations.

---

### 👨‍🚀 À propos du développeur
Ce projet a été conçu pour explorer les mécaniques de *Game Design* en mode texte, la gestion des structures de données en C et la création d'interfaces utilisateur légères et immersives dans un terminal.

**Bonne chance, Commandant. L'espace est vaste et impitoyable.**

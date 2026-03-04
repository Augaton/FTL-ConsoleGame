#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include <locale.h>
#include <stdlib.h>
#include <unistd.h>
#include "interface.h"

// ==================== SYSTÈME DE COULEURS ====================

typedef struct {
    int pair;
    int fg;
    int bg;
    int attrs;
} ColorTheme;

// Palette de couleurs prédéfinie
typedef struct {
    ColorTheme cadre;
    ColorTheme titre;
    ColorTheme normal;
    ColorTheme selection;
    ColorTheme accent;
    ColorTheme danger;
} ThemeCouleurs;

// Initialiser le thème de couleurs cyberpunk/SciFi
ThemeCouleurs themeCyber;
int ncursesInitialise = 0;

void initialiserTheme() {
    // Vérifier si le terminal supporte les couleurs
    if (!has_colors()) {
        return;
    }
    
    start_color();
    use_default_colors();
    
    // Couleur transparente pour l'arrière-plan
    int bg = COLOR_BLACK;
    
    // Paires de couleurs personnalisées
    init_pair(1, COLOR_CYAN, bg);           // Cadre cyan
    init_pair(2, COLOR_MAGENTA, bg);        // Titre magenta
    init_pair(3, COLOR_GREEN, bg);          // Normal vert
    init_pair(4, COLOR_BLACK, COLOR_CYAN);  // Sélection (inverse)
    init_pair(5, COLOR_YELLOW, bg);         // Accent jaune
    init_pair(6, COLOR_RED, bg);            // Danger rouge
    init_pair(7, COLOR_WHITE, COLOR_CYAN);  // Bouton actif
    init_pair(8, COLOR_YELLOW, COLOR_BLACK);// Info jaune
    
    // Thème cyberpunk
    themeCyber.cadre = (ColorTheme){1, COLOR_CYAN, bg, A_BOLD};
    themeCyber.titre = (ColorTheme){2, COLOR_MAGENTA, bg, A_BOLD};
    themeCyber.normal = (ColorTheme){3, COLOR_GREEN, bg, 0};
    themeCyber.selection = (ColorTheme){4, COLOR_BLACK, COLOR_CYAN, A_BOLD};
    themeCyber.accent = (ColorTheme){5, COLOR_YELLOW, bg, A_BOLD};
    themeCyber.danger = (ColorTheme){6, COLOR_RED, bg, A_BOLD};
}

// Appliquer une couleur de thème
void appliquerCouleur(ColorTheme theme) {
    attron(COLOR_PAIR(theme.pair) | theme.attrs);
}

void retirerCouleur(ColorTheme theme) {
    attroff(COLOR_PAIR(theme.pair) | theme.attrs);
}

// ==================== INITIALISATION / TERMINATION NCURSES ====================

void initialiserNCurses(void) {
    if (ncursesInitialise) return;
    
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    raw();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);
    initialiserTheme();
    ncursesInitialise = 1;
}

void terminerNCurses(void) {
    if (!ncursesInitialise) return;
    endwin();
    ncursesInitialise = 0;
}

// ==================== EFFETS VISUELS ====================

void afficherLigne(int y, int x, int longeur, char c) {
    for (int i = 0; i < longeur; i++) {
        mvaddch(y, x + i, c);
    }
}

void afficherCadre(int y, int x, int hauteur, int largeur) {
    appliquerCouleur(themeCyber.cadre);
    
    // Coins avec ACS characters
    mvaddch(y, x, ACS_ULCORNER);
    mvaddch(y, x + largeur - 1, ACS_URCORNER);
    mvaddch(y + hauteur - 1, x, ACS_LLCORNER);
    mvaddch(y + hauteur - 1, x + largeur - 1, ACS_LRCORNER);
    
    // Lignes horizontales
    for (int i = 1; i < largeur - 1; i++) {
        mvaddch(y, x + i, ACS_HLINE);
        mvaddch(y + hauteur - 1, x + i, ACS_HLINE);
    }
    
    // Lignes verticales
    for (int i = 1; i < hauteur - 1; i++) {
        mvaddch(y + i, x, ACS_VLINE);
        mvaddch(y + i, x + largeur - 1, ACS_VLINE);
    }
    
    retirerCouleur(themeCyber.cadre);
}

void afficherTraiteur(int y, int x, int longeur) {
    appliquerCouleur(themeCyber.cadre);
    mvaddch(y, x, ACS_LTEE);
    for (int i = 1; i < longeur - 1; i++) {
        mvaddch(y, x + i, ACS_HLINE);
    }
    mvaddch(y, x + longeur - 1, ACS_RTEE);
    retirerCouleur(themeCyber.cadre);
}

// Animation de "scan" au démarrage du menu
void animationScan(int maxY, int maxX) {
    appliquerCouleur(themeCyber.accent);
    for (int i = 0; i < maxY; i++) {
        mvaddch(i, maxX / 2, '|');
        refresh();
        usleep(15000);
    }
    retirerCouleur(themeCyber.accent);
}

// Déclaration anticipée
void afficherHUD(Vaisseau *v);

// ==================== MENU INTERACTIF AMÉLIORÉ ====================

int menuInteractif(const char *titre, OptionMenu options[], int nbOptions, Vaisseau *v) {
    // Vérifier que ncurses est initialisé
    if (!ncursesInitialise) {
        initialiserNCurses();
    }
    
    int highlight = 0;
    int choix = -1;
    int touche;
    int maxY, maxX;

    getmaxyx(stdscr, maxY, maxX);

    // Dimensionner le menu avec plus d'espace vertical
    int menuHauteur = nbOptions + 9;
    int menuLargeur = 60;
    if (menuLargeur > maxX - 4) menuLargeur = maxX - 4;
    
    int startX = (maxX - menuLargeur) / 2;

    while(1) {
        clear();

        if (v) afficherHUD(v); // <--- LE HUD RESTE TOUJOURS EN HAUT

        int startY = (maxY - menuHauteur) / 2 + 2;
        
        // Fond étoilé optionnel (effet sci-fi léger)
        appliquerCouleur(themeCyber.normal);
        
        // Cadre principal
        afficherCadre(startY, startX, menuHauteur, menuLargeur);
        
        // Titre
        appliquerCouleur(themeCyber.titre);
        int titrePosX = startX + (menuLargeur - strlen(titre)) / 2;
        mvprintw(startY + 2, titrePosX, " %s ", titre);
        retirerCouleur(themeCyber.titre);
        
        // Ligne de séparation après titre
        afficherTraiteur(startY + 3, startX, menuLargeur);
        
        // Options (espacement réduit)
        for(int i = 0; i < nbOptions; i++) {
            int y = startY + 5 + i;
            if(i == highlight) {
                // Option sélectionnée - surbrillance
                appliquerCouleur(themeCyber.selection);
                
                // Fond coloré pour l'option
                for (int j = 0; j < menuLargeur - 2; j++) {

                    mvaddch(y, startX + 1 + j, ' ');
                }
                
                // Texte avec flèche clignotante
                mvprintw(y, startX + 2, " > %s ", options[i].label);
                retirerCouleur(themeCyber.selection);
            } else {
                // Option normale
                appliquerCouleur(themeCyber.normal);
                mvprintw(y, startX + 3, "%s", options[i].label);
                retirerCouleur(themeCyber.normal);
            }
        }
        
        // Ligne de séparation avant instructions
        afficherTraiteur(startY + menuHauteur - 3, startX, menuLargeur);
        
        // Instructions
        appliquerCouleur(themeCyber.accent);
        char instructions[] = "UP/DOWN: Naviguer | ENTER: Confirmer | Q: Quitter";
        int instrPosX = startX + (menuLargeur - strlen(instructions)) / 2;
        mvprintw(startY + menuHauteur - 2, instrPosX, "%s", instructions);
        retirerCouleur(themeCyber.accent);
        
        refresh();
        touche = getch();

        switch(touche) {
            case KEY_UP:
                if(highlight > 0) highlight--;
                break;
            case KEY_DOWN:
                if(highlight < nbOptions - 1) highlight++;
                break;
            case 10: // Entrée
                choix = options[highlight].id;
                break;
            case 'q':
            case 'Q':
                // Demander confirmation avant de quitter
                terminerNCurses();
                printf("\nEtes-vous sur de vouloir quitter ? (o/n): ");
                fflush(stdout);
                int confirm = getchar();
                while(getchar() != '\n'); // Vider le buffer
                if (confirm == 'o' || confirm == 'O') {
                    exit(0);
                }
                // Sinon, réinitialiser ncurses et continuer
                initialiserNCurses();
                break;
        }

        if(choix != -1) break;
    }

    return choix;
}

// ==================== DIALOGUE SIMPLE ====================

void afficherDialogueSimple(const char *titre, const char *message) {
    if (!ncursesInitialise) {
        initialiserNCurses();
    }
    
    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);
    
    int dialogHauteur = 10;
    int dialogLargeur = 70;
    if (dialogLargeur > maxX - 4) dialogLargeur = maxX - 4;
    
    int startY = (maxY - dialogHauteur) / 2;
    int startX = (maxX - dialogLargeur) / 2;
    
    clear();
    
    // Cadre
    afficherCadre(startY, startX, dialogHauteur, dialogLargeur);
    
    // Titre
    appliquerCouleur(themeCyber.titre);
    mvprintw(startY + 2, startX + 2, " %s ", titre);
    retirerCouleur(themeCyber.titre);
    
    // Séparation
    afficherTraiteur(startY + 3, startX, dialogLargeur);
    
    // Message
    appliquerCouleur(themeCyber.normal);
    mvprintw(startY + 5, startX + 2, "%s", message);
    retirerCouleur(themeCyber.normal);
    
    // Instructions
    appliquerCouleur(themeCyber.accent);
    mvprintw(startY + dialogHauteur - 2, startX + 2, "Appuyez sur une touche pour continuer...");
    retirerCouleur(themeCyber.accent);
    
    refresh();
    getch();
}

// ==================== CONFIRMATION OUI/NON ====================

int demanderConfirmation(const char *titre, const char *question) {
    if (!ncursesInitialise) {
        initialiserNCurses();
    }
    
    int highlight = 0;
    int choix = -1;
    int touche;
    int maxY, maxX;

    getmaxyx(stdscr, maxY, maxX);
    
    int dialogHauteur = 12;
    int dialogLargeur = 60;
    if (dialogLargeur > maxX - 4) dialogLargeur = maxX - 4;
    
    int startY = (maxY - dialogHauteur) / 2;
    int startX = (maxX - dialogLargeur) / 2;

    OptionMenu boutons[] = {
        {"Oui", 1},
        {"Non", 0}
    };

    while(1) {
        clear();
        
        // Cadre
        afficherCadre(startY, startX, dialogHauteur, dialogLargeur);
        
        // Titre
        appliquerCouleur(themeCyber.danger);
        mvprintw(startY + 2, startX + 2, "[!] %s", titre);
        retirerCouleur(themeCyber.danger);
        
        // Séparation
        afficherTraiteur(startY + 3, startX, dialogLargeur);
        
        // Question
        appliquerCouleur(themeCyber.normal);
        mvprintw(startY + 5, startX + 2, "%s", question);
        retirerCouleur(themeCyber.normal);
        
        // Boutons
        for(int i = 0; i < 2; i++) {
            int y = startY + 8 + i;
            int x = startX + (dialogLargeur - 30) / 2;
            
            if(i == highlight) {
                appliquerCouleur(themeCyber.selection);
                mvprintw(y, x, " > %s ", boutons[i].label);
                retirerCouleur(themeCyber.selection);
            } else {
                appliquerCouleur(themeCyber.normal);
                mvprintw(y, x, "   %s ", boutons[i].label);
                retirerCouleur(themeCyber.normal);
            }
        }
        
        refresh();
        touche = getch();

        switch(touche) {
            case KEY_LEFT:
            case KEY_UP:
                if(highlight > 0) highlight--;
                break;
            case KEY_RIGHT:
            case KEY_DOWN:
                if(highlight < 1) highlight++;
                break;
            case 10: // Entrée
                choix = boutons[highlight].id;
                break;
        }

        if(choix != -1) break;
    }

    return choix;
}

void afficherHUD(Vaisseau *v) {
    int max_x;
    int max_y = 0;
    getmaxyx(stdscr, max_y, max_x);
    (void)max_y; // Suppress unused variable warning

    // Couleur du bandeau supérieur
    attron(COLOR_PAIR(1) | A_BOLD);
    mvhline(0, 0, ' ', max_x); // Fond du bandeau
    mvprintw(0, 2, " COMMANDANT: %s ", v->nom);
    attroff(COLOR_PAIR(1) | A_BOLD);

    // Barre de Coque (Rouge/Vert selon l'état)
    mvprintw(1, 2, "COQUE: ");
    appliquerCouleur(themeCyber.normal);
    for(int i=0; i<20; i++) {
        if(i < (v->coque * 20 / v->coqueMax)) addch('/');
        else addch('.');
    }
    printw(" %d/%d", v->coque, v->coqueMax);

    // Ressources à droite
    mvprintw(1, max_x - 45, "CARBURANT: %d", v->carburant);
    mvprintw(1, max_x - 30, "MISSILES: %d", v->missiles);
    mvprintw(1, max_x - 15, "FERRAILLE: %d", v->ferraille);

    // Ligne de séparation
    afficherTraiteur(2, 0, max_x);
    refresh();
}

void ajouterLog(const char* message) {
    static char logs[5][100]; // Garde les 5 derniers messages
    // Logique pour décaler les messages vers le haut
    for(int i=0; i<4; i++) strcpy(logs[i], logs[i+1]);
    strncpy(logs[4], message, 99);

    // Affichage en bas de l'écran
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    (void)max_x; // Suppress unused variable warning
    for(int i=0; i<5; i++) {
        mvprintw(max_y - 6 + i, 2, " > %s", logs[i]);
    }
}

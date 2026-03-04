#include <ncurses.h>
#include <stdlib.h>

// Fonction pour afficher un menu et retourner l'index du choix
int afficherMenuInteractif(char *titre, char *options[], int nbOptions) {
    int highlight = 0;
    int choix = -1;
    int touche;

    // Initialisation de ncurses pour cette fenêtre
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE); // Active les touches directionnelles
    curs_set(0);          // Cache le curseur

    while(1) {
        clear();
        mvprintw(1, 1, "=== %s ===", titre);

        for(int i = 0; i < nbOptions; i++) {
            if(i == highlight) {
                attron(A_REVERSE); // Inverse les couleurs (surlignage)
                mvprintw(i + 3, 2, " > %s ", options[i]);
                attroff(A_REVERSE);
            } else {
                mvprintw(i + 3, 2, "   %s ", options[i]);
            }
        }

        touche = getch();

        switch(touche) {
            case KEY_UP:
                if(highlight > 0) highlight--;
                break;
            case KEY_DOWN:
                if(highlight < nbOptions - 1) highlight++;
                break;
            case 10: // Touche Entrée
                choix = highlight;
                break;
        }

        if(choix != -1) break;
    }

    endwin(); // Ferme ncurses et rend la main au terminal standard
    return choix;
}
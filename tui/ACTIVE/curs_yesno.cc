#include "curs_dialog.h"
#include <ncurses.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string>

curs_yesno::curs_yesno (std::string title, unsigned int nrow, unsigned int ncol, unsigned int y, unsigned int x)
	: curs_dialog(title, nrow, ncol, y, x) 
{
	yes = derwin(this->window, 1, 6, nrow - 3, 6);
	wbkgd(yes, COLOR_PAIR(2));
	waddstr(yes, "<YES>");
	no  = derwin(this->window, 1, 6, nrow - 3, ncol - 12);
	wbkgd(no,  COLOR_PAIR(2));
	waddstr(no, "<NO>");
	wrefresh(this->window);
}

curs_yesno::~curs_yesno()
{
	delwin(yes);
	delwin(no);
}

int curs_yesno::get_choise() {
	int key {-1};
	bool selected {true}; // true : yes, false : no
	while (1)
	{
		key = getch();
		if ( key == KEY_LEFT || key == KEY_RIGHT || key == 'h' || key == 'l') {
			selected = !selected;
			change_choise_bg(selected);
		}
		else if ( key == ESCAPE )
			return -1;
		else if ( key == ENTER)
			return selected;

	}
}

void curs_yesno::change_choise_bg(bool choise){ // if true 
	if (choise) {
		wbkgd(yes, COLOR_PAIR(1));
		wbkgd(no,  COLOR_PAIR(2));
	} else {
		wbkgd(yes, COLOR_PAIR(2));
		wbkgd(no,  COLOR_PAIR(1));
	}
	wnoutrefresh(yes);
	wnoutrefresh(no);
	doupdate();
}

#include "curs_dialog.h"
#include <ncurses.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string>

// new to create display, delete to hide display
curs_dialog::curs_dialog (std::string title, unsigned int nrow, unsigned int ncol, unsigned int y, unsigned int x) 
	: title {title}, nrow {nrow}, ncol {ncol}, y {y}, x {x}
{
	window_o = newwin(nrow, ncol, y, x);
	window   = derwin(window_o, nrow-2, ncol-2, 1, 1);
	wbkgd(window_o, COLOR_PAIR(2));
	box(window_o, ACS_VLINE, ACS_HLINE);	
	wbkgd(window,   COLOR_PAIR(2));
	wattron(window_o, COLOR_PAIR(3));
	int str_center = this->title.length() / 2;
	int col_center = ncol / 2;
	wmove(window_o, 0, col_center - str_center);	
	waddstr(window_o, this->title.c_str());
	wattroff(window_o, COLOR_PAIR(3));
	wrefresh(window_o);
}

curs_dialog::~curs_dialog () 
{
	delwin(window);
	delwin(window_o);	
}

void curs_dialog::set_message(std::string str)
{
	wmove(window, 2, 2);
	wattron(window, COLOR_PAIR(3));
	waddstr(window, str.c_str());
	wattroff(window, COLOR_PAIR(3));
	wrefresh(window);
}


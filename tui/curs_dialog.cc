#include <ncurses.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string>

#define ENTER 10
#define ESCAPE 27

// new to create display, delete to hide display
class curs_dialog{
	public:
	WINDOW *window, *window_o;  // window, outer_window 
	std::string title {"TITLE"}; // dialog title
	unsigned int nrow, ncol, y, x;

	curs_dialog (std::string title, unsigned int nrow, unsigned int ncol, unsigned int y, unsigned int x) 
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

	~curs_dialog () 
	{
		delwin(window);
		delwin(window_o);	
	}

	void set_message(std::string str)
	{
		wmove(window, 2, 2);
		wattron(window, COLOR_PAIR(3));
		waddstr(window, str.c_str());
		wattroff(window, COLOR_PAIR(3));
		wrefresh(window);
	}
};

class curs_yesno : public curs_dialog{
	public:
	WINDOW *yes, *no;

	curs_yesno (std::string title, unsigned int nrow, unsigned int ncol, unsigned int y, unsigned int x)
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
	
	~curs_yesno()
	{
		delwin(yes);
		delwin(no);
	}

	int  get_choise() {
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

	private:
	void change_choise_bg(bool choise){ // if true 
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
		//wrefresh(yes);
		//wrefresh(no);
	}
};

/*
int main(void){
	int key;
	init_curses();
	curs_yesno* hoge = new curs_yesno("CONFIRM DIALOG", 10, 40, 10, 10);
	hoge->set_message("Do you really want to exit ?");
	delete hoge;
	endwin();
	return 0;
}
*/

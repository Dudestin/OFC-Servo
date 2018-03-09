#include <ncurses.h>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <string>

#define ENTER 10
#define ESCAPE 27

class curs_dialog{
public:
	WINDOW *window, *window_o;
	std::string title {"TITLE"};
	unsigned int nrow, ncol, y, x;

	curs_dialog (std::string title, unsigned int nrow, unsigned int ncol, unsigned int y, unsigned int x);
	~curs_dialog ();
	void set_message(std::string str);
};

class curs_yesno : public curs_dialog{
public:
	WINDOW *yes, *no;
	curs_yesno (std::string title, unsigned int nrow, unsigned int ncol, unsigned int y, unsigned int x);
	~curs_yesno();
	int get_choise();
private:
	void change_choise_bg(bool choise);
};

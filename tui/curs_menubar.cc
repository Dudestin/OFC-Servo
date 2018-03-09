#include <ncurses.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <tuple>

#define ENTER 10
#define ESCAPE 27

void init_curses()
{
	initscr();
	start_color();
	init_pair(1, COLOR_WHITE, COLOR_BLUE);
	init_pair(2, COLOR_BLUE, COLOR_WHITE);
	init_pair(3, COLOR_RED, COLOR_WHITE);
	init_pair(4, COLOR_WHITE, COLOR_RED);
	curs_set(0);
	noecho();
	keypad(stdscr, TRUE);
}

class curs_menubar
{
	using item_tuple_T = std::tuple<std::string, void (*)()>;
	using menu_T       = std::tuple<std::string, std::vector<item_tuple_T>>;
	const unsigned int col_width {16};	
public:
	WINDOW *menubar;
	std::vector<menu_T> items; // store all items data;

	WINDOW *row_box; // for draw_menu() only;
	std::vector<WINDOW *> row_item_vec; // for draw_menu() only...

	template <class... T>
	curs_menubar (T... menu) : items {menu...}
	{
		menubar = subwin(stdscr, 1, COLS, 0, 0);
		wbkgd(menubar, COLOR_PAIR(2));
		int i = 0;
		for (const menu_T menu_i : std::initializer_list<menu_T>{menu...}) 
		{ // Menu-tab gen.
			mvwaddstr(menubar, 0, col_width * i, std::get<0>(menu_i).c_str());
			wattron(menubar, COLOR_PAIR(3));
			wprintw(menubar, "(F%1d)", ++i);
			wattroff(menubar, COLOR_PAIR(3));
		}
		wrefresh(menubar);
	}

	~curs_menubar () 
	{
		delete_draw_menu();
		delwin(menubar);
	}	

	void delete_draw_menu()
	{
		for (auto &e : this->row_item_vec)
			delwin(e);
		delwin(row_box);
	}

	void draw_menu(unsigned int col_index)
	{
		delete_draw_menu();
		refresh();
 		row_box = newwin(10, col_width, 1, col_index * col_width);
		wbkgd(row_box, COLOR_PAIR(2));
		box(row_box, ACS_VLINE, ACS_HLINE);
		int i = 0;
		for (const item_tuple_T& e : std::get<1>(items[col_index]))
		{
			row_item_vec.push_back
				(subwin(this->row_box, 1, col_width-2, i+2, col_index * col_width + 1));
			waddstr(row_item_vec[i++], std::get<0>(e).c_str());
		}
		wbkgd(row_item_vec[0], COLOR_PAIR(1));
		wrefresh(row_box);
	}

	int choice_item(int col_init)
	{
		int key;
		int col_selected = col_init;
		int row_selected = 0;
		draw_menu(col_selected);
		while (1) {
			key = getch();
			if (key == KEY_DOWN || key == KEY_UP || key == 'j' || key == 'k') 
			{
				wbkgd(row_item_vec[row_selected], COLOR_PAIR(2));
				wnoutrefresh(row_item_vec[row_selected]);
				int row_size = row_item_vec.size();
				if (key == KEY_DOWN || key == 'j'){
					row_selected = (row_selected + 1) % row_size;
				} else {
					row_selected = (row_selected + row_size - 1) % row_size;
				}
				wbkgd(row_item_vec[row_selected], COLOR_PAIR(1));
				wnoutrefresh(row_item_vec[row_selected]);
				doupdate();
			} else if (key == KEY_LEFT || key == KEY_RIGHT || key == 'h' || key == 'l' || key == KEY_F(1) || key == KEY_F(2))
			{
				row_selected++;
				delete_draw_menu();
				touchwin(stdscr);
				refresh();
				draw_menu(row_selected % 2);
			} else if (key == ESCAPE)
			{
				return -1;
			} else if (key == ENTER)
			{
				return row_selected;
			}
		}
	}

	void start_event_handler(unsigned int col_index, unsigned int row_index)
	{
		std::get<1>(std::get<1>(items[col_index])[row_index])();  // call event_handler;
	}
};

int main(void){
	using item_tuple_T = std::tuple<std::string, void (*)()>;
	using menu_T       = std::tuple<std::string, std::vector<item_tuple_T>>;

	init_curses();
	bkgd(COLOR_PAIR(1));
	
	curs_menubar *hoge = new curs_menubar(
			{menu_T("CONFIG", {{"item0", nullptr}, {"item1", nullptr}}), 
			 menu_T("EXPORT", {{"itemI", nullptr}, {"itemII",nullptr}})});
	int selected_num = hoge->choice_item(1);

	sleep(3);

	endwin();
	std::cout << selected_num << std::endl;
	return 0;
}

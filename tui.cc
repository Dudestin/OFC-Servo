#include "tui.h"
#include "reset.h"
#include "motor.h"
#include "gpio.h"

#define LOCK_ASCII \
" /$$        /$$$$$$   /$$$$$$  /$$   /$$\n\
| $$       /$$__  $$ /$$__  $$| $$  /$$/\n\
| $$      | $$  \\ $$| $$  \\__/| $$ /$$/\n\
| $$      | $$  | $$| $$      | $$$$$/\n\
| $$      | $$  | $$| $$      | $$  $$\n\
| $$      | $$  | $$| $$    $$| $$\\  $$\n\
| $$$$$$$$|  $$$$$$/|  $$$$$$/| $$ \\  $$\n\
|________/ \\______/  \\______/ |__/  \\__/"

#define LAG_ASCII \
" /$$        /$$$$$$   /$$$$$$\n\
| $$       /$$__  $$ /$$__  $$\n\
| $$      | $$  \\ $$| $$  \\__/\n\
| $$      | $$$$$$$$| $$ /$$$$\n\
| $$      | $$__  $$| $$|_  $$\n\
| $$      | $$  | $$| $$  \\ $$\n\
| $$$$$$$$| $$  | $$|  $$$$$$/\n\
|________/|__/  |__/ \\______/" 

#define LEAD_ASCII \
" /$$       /$$$$$$$$  /$$$$$$  /$$$$$$$\n\
| $$      | $$_____/ /$$__  $$| $$__  $$\n\
| $$      | $$      | $$  \\ $$| $$  \\ $$\n\
| $$      | $$$$$   | $$$$$$$$| $$  | $$\n\
| $$      | $$__/   | $$__  $$| $$  | $$\n\
| $$      | $$      | $$  | $$| $$  | $$\n\
| $$$$$$$$| $$$$$$$$| $$  | $$| $$$$$$$/\n\
|________/|________/|__/  |__/|_______/"                                     

#define NA_ASCII \
" /$$   /$$       /$$ /$$$$$$ \n\
| $$$ | $$      /$$//$$__  $$\n\
| $$$$| $$     /$$/| $$  \\ $$\n\
| $$ $$ $$    /$$/ | $$$$$$$$\n\
| $$  $$$$   /$$/  | $$__  $$\n\
| $$\\  $$$  /$$/   | $$  | $$\n\
| $$ \\  $$ /$$/    | $$  | $$\n\
|__/  \\__/|__/     |__/  |__/"

#define MENUBAR_ALIGN 20


void init_curses()
{
	initscr();
	start_color();
	init_pair(1, COLOR_WHITE, COLOR_BLUE);
	init_pair(2, COLOR_BLUE, COLOR_WHITE);
	init_pair(3, COLOR_RED, COLOR_WHITE);
	init_pair(4, COLOR_GREEN, COLOR_WHITE);
	init_pair(5, COLOR_WHITE, COLOR_RED);
	curs_set(0);
	noecho();
	keypad(stdscr, TRUE);
}

void draw_lockStatus(WINDOW *lockStatus, struct state_lockStatus *state)
{
	wbkgd(lockStatus, COLOR_PAIR(2));
	/* title */
	wmove(lockStatus, 0, 33);
	waddstr(lockStatus, "PLL INFORMATION");

	// pfd region, y -> 45, x -> 12
	wmove(lockStatus, 3, 0);
	switch (state->pfd_state){
		case 0 : //LOCK GREEN
		wattron(lockStatus, COLOR_PAIR(4));
		waddstr(lockStatus, LOCK_ASCII);	
		wattroff(lockStatus, COLOR_PAIR(4));
		break;
		case 1: // LEAD BLUE 
		wattron(lockStatus, COLOR_PAIR(2));
		waddstr(lockStatus, LEAD_ASCII);
		wattroff(lockStatus, COLOR_PAIR(2));
		break;
		case 2: // LAG RED
		wattron(lockStatus, COLOR_PAIR(3));
		waddstr(lockStatus, LAG_ASCII);
		wattroff(lockStatus, COLOR_PAIR(3));
		break;
		default:
		wattron(lockStatus, COLOR_PAIR(5));
		waddstr(lockStatus, NA_ASCII);
		wattroff(lockStatus, COLOR_PAIR(5));
	}

	// freq-error region
	wmove(lockStatus, 1, 45);
	switch(state->pfd_state){
		case 0: // LOCK GREEN
		wattron(lockStatus, COLOR_PAIR(4));
		wprintw(lockStatus, "Error:              %10d Hz\n", state->err_clk);	
		wattroff(lockStatus, COLOR_PAIR(4));
		break;
		case 1: // LEAD BLUE
		wattron(lockStatus, COLOR_PAIR(2));
		wprintw(lockStatus, "Error:              %10d Hz\n", state->err_clk);	
		wattroff(lockStatus, COLOR_PAIR(2));
		break;
		case 2: // LAG RED
		wattron(lockStatus, COLOR_PAIR(3));
		wprintw(lockStatus, "Error:              %10d Hz\n", state->err_clk);	
		wattroff(lockStatus, COLOR_PAIR(3));
	}	

	// freq-reference region	
	wmove(lockStatus, 2, 45);
	wprintw(lockStatus, "Feedback Frequency: %10d Hz", state->fed_clk);	

	// feq-feedback region
	wmove(lockStatus, 3, 45);
	wprintw(lockStatus, "Reference Frequency:%10d Hz", state->ref_clk);	
	
	// target ref-freq
	wmove(lockStatus, 4, 45);
	wprintw(lockStatus, "Target    Frequency:%10d Hz", state->tgt_clk);	

	// Kp-term 
	wmove(lockStatus, 5, 45);
	wprintw(lockStatus, "Kp-param : %.12f", state->Kp_param);	

	// NEW
	// Kd-term 
	wmove(lockStatus, 6, 45);
	wprintw(lockStatus, "Kd-param : %f", state->Kd_param);	

	// P-term 
	wmove(lockStatus, 7, 45);
	wprintw(lockStatus, "P-term  : %lf", state->P_term);	

	// I-term
	wmove(lockStatus, 8, 45);
	wprintw(lockStatus, "I-term  : %f", state->I_term);	

	// NEW
	// D-term
	wmove(lockStatus, 9, 45);
	wprintw(lockStatus, "D-term  : %f", state->D_term);	
	
	// FIX
	// PID-term
	wmove(lockStatus, 10, 45);
	wprintw(lockStatus, "PID-term : %lf", state->PID_term);	

	// operating time. a.k.a. run time 
	char run_time[32];
	strftime(run_time, sizeof(run_time), "%H:%M:%S", localtime(&(state->run_time)));
	wmove(lockStatus, 11, 45);
	wprintw(lockStatus, "Operating     Time: %s", run_time);

	// locking time
	char lock_time[32];
	strftime(lock_time, sizeof(lock_time), "%H:%M:%S", localtime(&(state->lock_time)));
	wmove(lockStatus, 12, 45);
	wprintw(lockStatus, "Lock Time:          %s", lock_time);

	// last UNLOCK state time  
	char unlock_time[32];
	strftime(unlock_time, sizeof(unlock_time), "%H:%M:%S", localtime(&(state->unlock_time)));
	wmove(lockStatus, 13, 45);
	wprintw(lockStatus, "Last Unlock Detect: %s", unlock_time);
}

void draw_motorStatus(WINDOW *motorStatus, struct state_motorStatus *state){
	wbkgd(motorStatus, COLOR_PAIR(2));
	/* title */
	wmove(motorStatus, 0, 26);
	waddstr(motorStatus, "STEPPER-MOTOR-DRIVER INFORMATION");

	/* Stage State */	
	wmove(motorStatus, 1, 1);
	waddstr(motorStatus, "Stage State: ");
	wmove(motorStatus, 1, 13);
	switch(state->motor_state){
		case 0: // WAIT BLUE
		wattron(motorStatus, COLOR_PAIR(2));
		waddstr(motorStatus, "IDLE");
		wattroff(motorStatus, COLOR_PAIR(2));
		break;
		case 1: // DRIVE RED
		wattron(motorStatus, COLOR_PAIR(3));
		waddstr(motorStatus, "DRIVE");
		wattroff(motorStatus, COLOR_PAIR(3));
	}

	/* current Stage-Position */
	wmove(motorStatus, 3, 1);
	wprintw(motorStatus, "Current Stage Position:%08d (pulse), %f (mm)", state->stage_pos, state->stage_Mpos);	
	// updated time  
	char stage_utime[32];
	strftime(stage_utime, sizeof(stage_utime), "%H:%M:%S", localtime(&(state->stage_utime)));
	wmove(motorStatus, 3, 60);
	wprintw(motorStatus, "%s", stage_utime);

	/* recent Stage-Position */
	wmove(motorStatus, 4, 1);
	wprintw(motorStatus, "Recent  Stage Position:%08d (pulse), %f (mm)", state->rstage_pos, state->rstage_Mpos);	
	// updated time  
	char rstage_utime[32];
	strftime(rstage_utime, sizeof(rstage_utime), "%H:%M:%S", localtime(&(state->rstage_utime)));
	wmove(motorStatus, 4, 60);
	wprintw(motorStatus, "%s", rstage_utime);

	/* Delta Position */
	wmove(motorStatus, 5, 1);
	wprintw(motorStatus, "Delta   Stage Position:%08d (pulse), %f (mm)", state->diff_pos, state->diff_Mpos);	
}

void draw_menubar(WINDOW *menubar){
	wbkgd(menubar, COLOR_PAIR(2));
	
	waddstr(menubar, "CONFIG");
	wattron(menubar, COLOR_PAIR(3));
	waddstr(menubar, "(F1)");
	wattroff(menubar, COLOR_PAIR(3));

	wmove(menubar, 0, MENUBAR_ALIGN);

	waddstr(menubar, "EXPORT");
	wattron(menubar, COLOR_PAIR(3));
	waddstr(menubar, "(F2)");
	wattroff(menubar, COLOR_PAIR(3));
	
	wmove(menubar, 0, MENUBAR_ALIGN * 2);

	waddstr(menubar, "RESET");
	wattron(menubar, COLOR_PAIR(3));
	waddstr(menubar, "(F3)");
	wattroff(menubar, COLOR_PAIR(3));
}

WINDOW **draw_menu(int start_col)
{
	int i;
	WINDOW **items;
	items=(WINDOW **)malloc(9*sizeof(WINDOW *));

	items[0]=newwin(10,19,1,start_col);
	wbkgd(items[0],COLOR_PAIR(2));
	box(items[0],ACS_VLINE,ACS_HLINE);
	items[1]=subwin(items[0],1,17,2,start_col+1);
	items[2]=subwin(items[0],1,17,3,start_col+1);
	items[3]=subwin(items[0],1,17,4,start_col+1);
	items[4]=subwin(items[0],1,17,5,start_col+1);
	items[5]=subwin(items[0],1,17,6,start_col+1);
	items[6]=subwin(items[0],1,17,7,start_col+1);
	items[7]=subwin(items[0],1,17,8,start_col+1);
	items[8]=subwin(items[0],1,17,9,start_col+1);
	for (i=1;i<9;i++)
		wprintw(items[i],"Item%d",i);
	wbkgd(items[1],COLOR_PAIR(1));
	wrefresh(items[0]);
	return items;
}

void delete_menu(WINDOW **items,int count)
{
	int i;
	for (i=0;i<count;i++)
		delwin(items[i]);
	free(items);
}

int scroll_menu(WINDOW **items,int count,int menu_start_col)
{
	int key;
	int selected=0;
	while (1) {
		key=getch();
		if (key==KEY_DOWN || key==KEY_UP || key=='j' || key=='k') {
			wbkgd(items[selected+1],COLOR_PAIR(2));
			wnoutrefresh(items[selected+1]);
			if (key==KEY_DOWN || key=='j') {
				selected=(selected+1) % count;
			} else {
				selected=(selected+count-1) % count;
			}
			wbkgd(items[selected+1],COLOR_PAIR(1));
			wnoutrefresh(items[selected+1]);
			doupdate();
		} else if (key==KEY_LEFT || key==KEY_RIGHT || key=='h' || key=='l' || key==KEY_F(1) || key==KEY_F(2)) {
			delete_menu(items,count+1);
			touchwin(stdscr);
			refresh();
			items=draw_menu(MENUBAR_ALIGN-menu_start_col);
			return scroll_menu(items,8,MENUBAR_ALIGN-menu_start_col);
		} else if (key==ESCAPE) {
			return -1;
		} else if (key==ENTER) {
			return selected;
		}
	}
}

void build_GUI()
{
	WINDOW *menubar;
	WINDOW *lockStatus, *motorStatus;     // outer
	WINDOW *lockStatus_i, *motorStatus_i; // inner
	int ch, rows ,cols;
	
	init_curses();
	bkgd(COLOR_PAIR(1));

	menubar=derwin(stdscr,1,82,0,0);
	lockStatus=derwin(stdscr, 16, 82, 2, 1);	
	lockStatus_i=derwin(lockStatus, 14, 80, 1, 1);
	box(lockStatus, 0, 0);

	motorStatus=subwin(stdscr, 12, 82, 18, 1);
	motorStatus_i=derwin(motorStatus, 10, 80, 1, 1);
	box(motorStatus, 0, 0);

	 // struct state_lockStatus hoge = {2, 10, 10000000, 100000, 114514, 300, time(NULL), 30000};
	//struct state_lockStatus hoge = NULL;
	//struct state_motorStatus fuga = NULL;
	// struct state_motorStatus fuga = {0, 100, 12.3, 0, 80, 9.9, 1, 20, 2.4};
	draw_menubar(menubar);
	//draw_lockStatus(lockStatus_i, &hoge);
	//draw_motorStatus(motorStatus_i, &fuga);
	refresh();

	int end_flag = 1;

	std::thread th_refresh (
	[&]{
		while(end_flag){
		// lock status
		auto lstate = get_lockStatus();
		draw_lockStatus(lockStatus_i, lstate);

		wrefresh(lockStatus_i);
		delete lstate;

		// motor status
		auto mstate = get_motorStatus();	
		draw_motorStatus(motorStatus_i, mstate);	

		wrefresh(motorStatus_i);
		delete mstate;
		usleep(500000);
		}
	});
	
	std::thread th_interact(
	[&]{
		WINDOW **menu_items;
		int selected_item;
		while(1){
		ch=getch();
		if (ch==KEY_F(1)) {
			menu_items=draw_menu(0);
			selected_item=scroll_menu(menu_items,8,0);
			delete_menu(menu_items,9);
			touchwin(stdscr);
			refresh();
		} else if (ch==KEY_F(2)) {
			menu_items=draw_menu(MENUBAR_ALIGN);
			selected_item=scroll_menu(menu_items,8,MENUBAR_ALIGN);
			delete_menu(menu_items,9);
			touchwin(stdscr);
			refresh();
		} else if (ch==KEY_F(3)) {
			reset_fclk1();	
		} else if (ch == KEY_LEFT) {
			 motor_state_flag = 1;
			tty_send_relative_drive(-100);
			 motor_state_flag = 0;
			rq = tty_get_pos();
		} else if (ch == KEY_RIGHT) {
			 motor_state_flag = 1;
			tty_send_relative_drive(100);
			 motor_state_flag = 0;
			rq = tty_get_pos();
		} else if (ch==ESCAPE) {
			curs_yesno *dialog = new curs_yesno("CONFIRM EXIT", 10, 40, LINES/2, COLS/2-20);
			dialog->set_message("Do you really want to exit?");
			if (dialog->get_choise() == 1){
				end_flag = 0;
				delete dialog;
				refresh();
				break;
			} else {
				delete dialog;
				touchwin(stdscr);
				refresh();
			}
		}	
	}});

	th_interact.join();
	th_refresh.join();

	// HOGE
	delwin(lockStatus_i);
	delwin(motorStatus_i);
	delwin(lockStatus);
	delwin(motorStatus);
	delwin(menubar);
	refresh();
	endwin();
}

void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string, chtype color)
{	int length, x, y;
	float temp;

	if(win == NULL)
		win = stdscr;
	getyx(win, y, x);
	if(startx != 0)
		x = startx;
	if(starty != 0)
		y = starty;
	if(width == 0)
		width = 80;

	length = strlen(string);
	temp = (width - length)/ 2;
	x = startx + (int)temp;
	wattron(win, color);
	mvwprintw(win, y, x, "%s", string);
	wattroff(win, color);
	refresh();
}

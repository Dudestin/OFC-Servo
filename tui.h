#pragma once
#include "curs_dialog.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ncurses.h>
#include <form.h>
#include <thread>

void init_curses();

void draw_lockStatus(WINDOW *lockStatus, struct state_lockStatus *state);

void draw_motorStatus(WINDOW *motorStatus, struct state_motorStatus *state);

void draw_menubar(WINDOW *menubar);

WINDOW **draw_menu(int start_col);

void delete_menu(WINDOW **items, int count);

int scroll_menu(WINDOW **items, int count, int menu_start_col);

void build_GUI();

void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string, chtype color);

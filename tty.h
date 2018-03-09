#pragma once
#include "str_util.h"
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#define BUF_LEN 256
#define TTY "/dev/ttyPS1"

#define BAUDRATE B9600
#define WAIT_DRIVE usleep(100000);

void init_buff();

int tty_send_drive(int pos);

int tty_send_relative_drive(int pos);

int tty_set_stepDegree(unsigned short);

int tty_stop_motor();

int tty_set_speed(unsigned int, unsigned int, unsigned int);

int tty_set_home();

int tty_get_pos();

void tty_setup();

void tty_exit();

void tty_sig_handler(int signum);

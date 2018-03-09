#pragma once
#include "tty.h"

int tty_fd;
struct termios *oldAttr = NULL;
struct termios *newAttr = NULL;
char* tx_buff = NULL;
char* rx_buff = NULL;

void init_buff(){
	memset(rx_buff, 0, BUF_LEN);
	memset(tx_buff, 0, BUF_LEN);
}

// Send A & G command to drive stage 
/* 0: success, -1: fail */
int tty_send_drive(int pos){
	tcflush(tty_fd, TCIOFLUSH);
	init_buff();
	char sign = (pos >= 0) ? '+' : '-'; 
	int abs_pos = abs(pos);
	dprintf(tty_fd, "A:1%cP%u\n", sign, abs_pos);
	tcdrain(tty_fd);
 	read (tty_fd, rx_buff, 3);
	if (strcmp(rx_buff, "OK\n")) // is not OK\n	
		return -1;

	init_buff();
	dprintf(tty_fd, "G:\n");
	tcdrain(tty_fd);
	read(tty_fd, rx_buff, 3);
	if (strcmp(rx_buff, "OK\n")) // is not OK\n	
		return -1;

	while(1){ // await a driving finish
		init_buff();
		dprintf(tty_fd, "!:\n");
		tcdrain(tty_fd);
		read(tty_fd, rx_buff, 2);
		if (strcmp(rx_buff, "R\n") == 0) // is Ready
			return 0;
		else 
			WAIT_DRIVE
	}
}

// Send M & G command to drive stage 
/* 0: success, -1: fail */
int tty_send_relative_drive(int pos){
	tcflush(tty_fd, TCIOFLUSH);
	init_buff();
	char sign = (pos >= 0) ? '+' : '-'; 
	int abs_pos = abs(pos);
	dprintf(tty_fd, "M:1%cP%u\n", sign, abs_pos);
	tcdrain(tty_fd);
 	read (tty_fd, rx_buff, 3);
	if (strcmp(rx_buff, "OK\n")) // is not OK\n	
		return -1;

	init_buff();
	dprintf(tty_fd, "G:\n");
	tcdrain(tty_fd);
	read(tty_fd, rx_buff, 3);
	if (strcmp(rx_buff, "OK\n")) // is not OK\n	
		return -1;

	while(1){ // await a driving finish
		init_buff();
		dprintf(tty_fd, "!:\n");
		tcdrain(tty_fd);
		read(tty_fd, rx_buff, 2);
		if (strcmp(rx_buff, "R\n") == 0) // is Ready
			return 0;
		else 
			WAIT_DRIVE
	}
}

// S command : set stepping-degree count
/* 0: success, -1: fail */
int tty_set_stepDegree(unsigned short rated){
	init_buff();
	tcflush(tty_fd, TCIOFLUSH);
	dprintf(tty_fd, "S:1%u\n", rated);
	read(tty_fd, rx_buff, 3);
	if (strcmp(rx_buff, "OK\n")) // is not OK\n	
		return -1;
	return 0;
}

// L:E command : Stop motor immediately
/* 0: success, -1: fail */
int tty_stop_motor(){
	init_buff();
	tcflush(tty_fd, TCIOFLUSH);
	dprintf(tty_fd, "L:E\n");
	read(tty_fd, rx_buff, 3);
	if (strcmp(rx_buff, "OK\n"))
		return -1;
	return 0;
}

// D command : set speed instruction
/* 0: success, -1: fail */
int tty_set_speed(unsigned int S, unsigned int F, unsigned int R){
	init_buff();
	tcflush(tty_fd, TCIOFLUSH);
	dprintf(tty_fd, "D:1S%uF%uR%u\n", S, F, R);
	read(tty_fd, rx_buff, 3);
	if (strcmp(rx_buff, "OK\n")) // is not OK\n
		return -1;
	return 0;
}

// R command : set current stage position to home-position
/* 0: success, -1: fail */
int tty_set_home(){
	init_buff();
	tcflush(tty_fd, TCIOFLUSH);
	dprintf(tty_fd, "R:1\n");
	read(tty_fd, rx_buff, 3);
	if (strcmp(rx_buff, "OK\n")) // is not OK\n	
		return -1;
	return 0;
}

// Q command : get current stage position
int tty_get_pos(){
RETRY:
	init_buff();
	tcflush(tty_fd, TCIOFLUSH);
	dprintf(tty_fd, "Q:\n");
	read(tty_fd, rx_buff, BUF_LEN);
	std::string str_buff (rx_buff);
	int pos;
	try {
		pos = stoi(str_buff);
	} 
	catch (const std::invalid_argument &e) {
		std::cerr << "failed stoi(), " << str_buff << std::endl;
		usleep(1000); // wait 1m sec
		goto RETRY;
	}
	return pos;
}

void tty_setup(){
	/* set termios */
	oldAttr = (struct termios*)malloc(sizeof(struct termios));
	newAttr = (struct termios*)malloc(sizeof(struct termios));
	tx_buff = (char*)malloc(BUF_LEN);
	rx_buff = (char*)malloc(BUF_LEN);
	tty_fd = open(TTY, O_RDWR);
	tcgetattr(tty_fd, oldAttr);
	*newAttr = *oldAttr;
	newAttr->c_iflag = IGNPAR | IGNCR;
	newAttr->c_oflag = ONLRET | NL1;
	newAttr->c_cflag = CREAD | CRTSCTS | CS8 | CLOCAL;
	newAttr->c_lflag = IEXTEN;
	cfsetispeed(newAttr, BAUDRATE);
	cfsetospeed(newAttr, BAUDRATE);
	tcsetattr(tty_fd, TCSANOW, newAttr);
	tcflush(tty_fd, TCIOFLUSH);
}

void tty_exit(){
	tcflush(tty_fd, TCIOFLUSH);
	tcsetattr(tty_fd, TCSANOW, oldAttr); // restore old terminal settings
	close(tty_fd);
	free(oldAttr);
	oldAttr = NULL;
	free(newAttr);
	newAttr = NULL;
	free(tx_buff);
	tx_buff = NULL;
	free(rx_buff);
	rx_buff = NULL;
}

/* release heap and file descriptor */
void tty_sig_handler(int signum) {
	printf("\n%d : signal handler has been called\n", signum);
	tty_exit();
}


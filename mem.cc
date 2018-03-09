#include "mem.h"

int mem_fd;

int mem_setup(){
	if ((mem_fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
		perror("/dev/mem Open Error");
		return -1;
	}
}

int mem_exit(){
	if (mem_fd >= 0) 
	       close(mem_fd);	
}

void mem_sig_handler() {
	mem_exit();
}

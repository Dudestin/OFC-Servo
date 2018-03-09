#include "reset.h"
// Refer to UG585 p1555 0xF8000240
// Need to specity with considering Page Size 
#define FPGA_RST_CTRL_BASE (0xF8000000)

volatile char* fpga_rst_ctrl_base = NULL;
volatile uint32_t* fpga_rst_ctrl_addr = NULL;

int reset_setup(){
	fpga_rst_ctrl_base = (volatile char*)mmap(NULL, 0x240, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, FPGA_RST_CTRL_BASE);
	if (fpga_rst_ctrl_base == MAP_FAILED)
	{
		perror("mmap failed : FPGA_RST_CTRL");
		close(mem_fd);
		return -1;
	}
	fpga_rst_ctrl_addr = (volatile uint32_t*)(fpga_rst_ctrl_base + 0x240);
	printf("base:%p\taddr:%p\n", fpga_rst_ctrl_base, fpga_rst_ctrl_addr);
}

void reset_exit() {
	if (fpga_rst_ctrl_base)
		munmap((void*)fpga_rst_ctrl_base, 0x01);
}

void reset_sig_handler() {
	reset_exit();
}

void reset_fclk0 (){
	*fpga_rst_ctrl_addr = *fpga_rst_ctrl_addr | 0x1;
	sleep(1);
	*fpga_rst_ctrl_addr = *fpga_rst_ctrl_addr ^ 0x1;
}

void reset_fclk1 (){
	*fpga_rst_ctrl_addr = *fpga_rst_ctrl_addr | 0x2;
	sleep(1);
	*fpga_rst_ctrl_addr = *fpga_rst_ctrl_addr ^ 0x2;
}

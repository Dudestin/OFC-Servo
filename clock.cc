#include "clock.h"
#include "mem.h"

volatile char* ref_clk_wiz_addr = nullptr;
volatile char* sys_clk_wiz_addr = nullptr;

/* 0 : success, 1 : fail */
int clock_setup()
{
	ref_clk_wiz_addr = (volatile char*)mmap(NULL, 0x2FC, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, REF_CLK_WIZ_BASE);
	if (ref_clk_wiz_addr == MAP_FAILED) 
	{
		perror ("mmap");
		return -1;
	}

	sys_clk_wiz_addr = (volatile char*)mmap(NULL, 0x2FC, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, SYS_CLK_WIZ_BASE);
	if (sys_clk_wiz_addr == MAP_FAILED) 
	{
		perror ("mmap");
		return -1;
	}
	return 0;
}

void clock_exit()
{
	if (ref_clk_wiz_addr)
		munmap((void*)ref_clk_wiz_addr, 0x2FC);

	if (sys_clk_wiz_addr)
		munmap((void*)sys_clk_wiz_addr, 0x2FC);
}

void clock_sig_handler()
{
	clock_exit();
}

void refclk_apply_setting()
{
	auto ptr = (volatile struct CCR23*)(ref_clk_wiz_addr + 0x25C);
	ptr->LOAD_SEN = 0x1;	
	ptr->SADDR    = 0x1;
}

void refclk_set_clk0_div(uint8_t div, unsigned short fdiv)
{
	auto ptr = (volatile struct CCR2*)(ref_clk_wiz_addr + 0x208);
	ptr->CLKOUT0_DIVIDE = div;
	ptr->CLKOUT0_DIVIDE_FRAC = fdiv;
}

void intclk_set_div(uint8_t div)
{
	auto ptr = (volatile uint8_t*)(sys_clk_wiz_addr + 0x220);
	*ptr = div;
}

uint8_t intclk_get_div()
{
	return *(uint8_t*)(sys_clk_wiz_addr + 0x220);
}

void intclk_apply_setting()
{
	auto ptr = (volatile struct CCR23*)(sys_clk_wiz_addr + 0x25C);
	ptr->LOAD_SEN = 0x1;	
	ptr->SADDR    = 0x1;
}

struct CCR2 refclk_get_clk0_div()
{
	return *(struct CCR2*)(ref_clk_wiz_addr + 0x208);
}

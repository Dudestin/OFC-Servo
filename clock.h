#pragma once
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#define REF_CLK_WIZ_BASE (0x43C10000)
#define SYS_CLK_WIZ_BASE (0x43C00000)

#pragma pack(1)
struct CCR0{  // C_BASEADDR + 0x200
	uint8_t DIVCLK_DIVIDE;  // [7:0]
	uint8_t CLKFBOUT_MULT;  // [15:8]
	unsigned short   CLKFBOUT_FRAC_MULTIPLY : 10; // [25:16]
};
#pragma pack()

struct CCR1{  // C_BASEADDR + 0x204
	unsigned int CLKFBOUT_PHASE; // [31:0]
};

#pragma pack(1)
struct CCR2{  // C_BASEADDR + 0x208
	uint8_t CLKOUT0_DIVIDE; // [7:0] 
	uint16_t CLKOUT0_DIVIDE_FRAC;   // [17:8]
};
#pragma pack()

#pragma pack(1)
struct CCR23{ // C_BASEADDR + 0x25C
	uint8_t LOAD_SEN  : 1; // bit 0
	uint8_t SADDR	  : 1; // bit 1
};
#pragma pack()

int clock_setup();
void clock_exit();
void clock_sig_handler();

void refclk_apply_setting();
void refclk_set_clk0_div(uint8_t, uint16_t);
struct CCR2 refclk_get_clk0_div();

void intclk_set_div(uint8_t div);
uint8_t intclk_get_div();
void intclk_apply_setting();

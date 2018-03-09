#pragma once
#include "mem.h"
#include <cmath>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <time.h>

struct state_lockStatus{
	uint32_t pfd_state; // lead loc, etc
	int32_t err_clk;   // error
	uint32_t ref_clk;  // ref clk
	uint32_t fed_clk;  // feedback clk
	uint32_t tgt_clk;  // target freq
	float Kp_param;
	float Kd_param;
	double P_term;
	float  I_term;
	float  D_term;
	double PID_term;
	time_t run_time; // running tiem
	time_t lock_time; // locking time
	time_t unlock_time; // last unlock detected time
};

#pragma pack(1)
struct phase_sig {
	uint8_t feedback : 1;
	uint8_t reference : 1;
};
#pragma pack()

#pragma pack(1)
struct phase_state {
	uint8_t lead : 1;
	uint8_t lock : 1;
	uint8_t lag  : 1;
};
#pragma pack()

int gpio_setup();
void gpio_exit();
void gpio_sig_handler();
unsigned int gpio_getRefFreq();
unsigned int gpio_getFedFreq();
int   gpio_getErrFreq();
float gpio_getQ();
void  gpio_setParamP(float);
float gpio_getParamP();
float gpio_getI();
float gpio_getP();
struct phase_sig gpio_getPhaseSig();
struct phase_state gpio_getPhaseState();
// below new
void gpio_setParamD(float p);
float gpio_getParamD();
float gpio_getD();

struct state_lockStatus *get_lockStatus();

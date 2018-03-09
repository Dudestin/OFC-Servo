#pragma once
#include "gpio.h"
#include "tty.h"
#include <stdio.h>
#include <stdint.h>
#include <time.h>

#define STAGE_MAX_POS (1000000)
#define STAGE_MIN_POS (-1000000) 

#define D_TERM_MAX_THR (300)
#define D_TERM_MIN_THR (-300)
#define STAGE_ROT_PCNT (40000) // refer SIGMA-KOKI Mark-204 manual p23

extern double q;
extern int rq;
extern double accum_P;
extern int motor_state_flag;

struct state_motorStatus{
	uint32_t motor_state; // wait, drive
	int32_t stage_pos;  // current stage pos (pulse unit)
	float stage_Mpos; // mm unit
	time_t  stage_utime; // refleshed time (system time)
	int32_t rstage_pos; // recent stage pos
	float rstage_Mpos;  // mm unit
	time_t  rstage_utime; // refleshed time (system time)
	int32_t diff_pos; // deference between current and recent stage pos.
	float diff_Mpos; // mm unit
};

int init_motor();
void servo_motor();
struct state_motorStatus *get_motorStatus();


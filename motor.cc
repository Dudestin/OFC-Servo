#include "motor.h"

time_t q_time = 0;
time_t rq_time;
int motor_state_flag = 0;
int current_stage_pos = 0;

int init_motor(){
	puts("Stop motor Immediately");
	if (tty_stop_motor() == -1)
		return -1;
	usleep(1000);

	puts("Setting Step-degree to '80'");
	if (tty_set_stepDegree(2) == -1) // refer manual (MS-204 p24) 
		return -1;	
	usleep(1000);

	puts("Setting speed");
	if (tty_set_speed(100, 1000, 50) == -1)
		return -1;

CHECK:
	char isFirstBoot = 0;
	printf("Is this a first time to boot? [y/n]: ");
	scanf("%c", &isFirstBoot);
	puts("");
	if (isFirstBoot == 'y' || isFirstBoot == 'Y'){  // need to set home position
		puts("Start Initialize Stepping-motor");
		puts("Stepd 1. move to edge position");
		if (tty_send_drive(99999999) == -1)
			return -1;
		if (tty_set_home() == -1)
			return -1;

		puts("Step 2. move to opposite edge to detect half position");
		int half_pos;
		if (tty_send_drive(-99999999) == -1)
			return -1;
		half_pos = tty_get_pos();
		if (half_pos == 0)
			return -1;
		else
			half_pos /= 2;
		printf("Detect half position : %d\n", half_pos);

		puts("Step 3. set home position and move there");
		if (tty_send_drive(half_pos) == -1)
			return -1;
		if (tty_set_home() == -1)
			return -1;
		current_stage_pos = 0;
	} 
	else if (isFirstBoot == 'n' || isFirstBoot == 'N'){ // has been already set home position
		puts("Move stage position to center");
		if (tty_send_drive(0) == -1)
			return -1;
	} 
	else goto CHECK; // invalid input, redo check process

	return 0;
}

int rq;
double q;
double accum_P = 0.f;
void servo_motor(){
	// q : next stage pos
	// rq: stage pos
	accum_P += gpio_getP();
	q = accum_P + gpio_getI() + gpio_getD();
	// int q =  rq + gpio_getQ();
	// q = rq + (gpio_getP() + gpio_getI());
	if (q > STAGE_MAX_POS)
		q = STAGE_MAX_POS;
	else if (q < STAGE_MIN_POS)
		q = STAGE_MIN_POS;
	if (int(q) != rq) { // need to move stage ? 
		motor_state_flag = 1;
		tty_send_drive(q);
		rq = tty_get_pos(); // get physical stage position from motor-driver
		motor_state_flag = 0;
	} else
		usleep(100'000);
}

float pulse2mm(int pulse){ // pulse -> mm 
	return (1 / STAGE_ROT_PCNT) * pulse;
}

float pulse2um(int pulse){ // pulse -> um 
	return (1000 / STAGE_ROT_PCNT) * pulse;
}

struct state_motorStatus *get_motorStatus()
{
	auto state = new struct state_motorStatus;
	state->motor_state = motor_state_flag;
	state->stage_pos = int(q);
	state->stage_Mpos = pulse2mm(int(q));
	state->stage_utime = q_time;
	state->rstage_pos = rq;
	state->rstage_Mpos = pulse2mm(rq);
	state->rstage_utime = rq_time;
	state->diff_pos = int(q) - rq;
	state->diff_Mpos = pulse2mm(int(q) - rq);
	return state;
}

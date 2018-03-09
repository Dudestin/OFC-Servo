#include "gpio.h"
#include "motor.h"

#define GPIO_BTN_BASE    (0x41200000) // READ
#define GPIO_FEDCLK_BASE (0x41210000) // READ   uint32
#define GPIO_LED_BASE    (0x41220000) // WRITE  
#define GPIO_Q_BASE      (0x41230000) // READ   float32  ** controll output **
#define GPIO_REFCLK_BASE (0x41240000) // READ   uint32
#define GPIO_P_PARAM_BASE (0x41250000) // WRITE  float
#define GPIO_ERRFLK_BASE (0x41260000) // READ   int32
#define GPIO_I_BASE      (0x41270000) // READ   int32
#define GPIO_PI_BASE     (0x41280000) // READ  0x41280000 : P part float, 0x41280008 error_freq float
// new below
#define GPIO_PHASE_BASE  (0x41290000) // READ  0x41290000 : feedback, ref phase, +0x8 lead, lock, lag
#define GPIO_D_PARAM_BASE (0x412a0000) // WRITE
#define GPIO_D_BASE      (0x412b0000) // READ

volatile void* refclk_addr = nullptr;
volatile void* fedclk_addr = nullptr;
volatile void* errclk_addr = nullptr;
volatile void* btn_addr = nullptr;
volatile void* led_addr = nullptr;
volatile void* Q_addr = nullptr;
volatile void* paramP_addr = nullptr;
volatile void* I_addr = nullptr;
volatile char* P_addr = nullptr;
volatile char* phase_addr = nullptr;
// new below
volatile void* paramD_addr = nullptr;
volatile void* D_addr = nullptr;

/* need to run mem_setup() before run these */
int gpio_setup(){
	refclk_addr = mmap(NULL, 0x01, PROT_READ, MAP_SHARED, mem_fd, GPIO_REFCLK_BASE);
	if (refclk_addr == MAP_FAILED)
	{
		perror("mmap");
		return -1;
	}

	fedclk_addr = mmap(NULL, 0x01, PROT_READ, MAP_SHARED, mem_fd, GPIO_FEDCLK_BASE);
	if (fedclk_addr== MAP_FAILED)
	{
		perror("mmap");
		return -1;
	}

	errclk_addr = mmap(NULL, 0x01, PROT_READ, MAP_SHARED, mem_fd, GPIO_ERRFLK_BASE);
	if (errclk_addr == MAP_FAILED)
	{
		perror("mmap");
		return -1;
	}


	btn_addr = mmap(NULL, 0x01, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, GPIO_BTN_BASE);
	if (btn_addr == MAP_FAILED)
	{
		perror("mmap");
		return -1;
	}

	led_addr = mmap(NULL, 0x01, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, GPIO_LED_BASE);
	if (led_addr== MAP_FAILED)
	{
		perror("mmap");
		return -1;
	}

	Q_addr = mmap(NULL, 0x01, PROT_READ, MAP_SHARED, mem_fd, GPIO_Q_BASE);
	if (Q_addr == MAP_FAILED)
	{
		perror("mmap");
		return -1;
	}

	paramP_addr = mmap(NULL, 0x01, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, GPIO_P_PARAM_BASE);
	if (paramP_addr == MAP_FAILED)
	{
		perror("mmap");
		return -1;
	}

	I_addr = mmap(NULL, 0x01, PROT_READ, MAP_SHARED, mem_fd, GPIO_I_BASE);
	if (I_addr == MAP_FAILED)
	{
		perror("mmap");
		return -1;
	}
	
	P_addr = (volatile char*)mmap(NULL, 0x01, PROT_READ, MAP_SHARED, mem_fd, GPIO_PI_BASE);
	if (P_addr == MAP_FAILED)
	{
		perror("mmap");
		return -1;
	}

	phase_addr = (volatile char*)mmap(NULL, 0x08, PROT_READ, MAP_SHARED, mem_fd, GPIO_PHASE_BASE);
	if (phase_addr == MAP_FAILED)
	{
		perror("mmap");
		return -1;
	}

	paramD_addr = mmap(NULL, 0x01, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, GPIO_D_PARAM_BASE);
	if (paramD_addr == MAP_FAILED)
	{
		perror("mmap");
		return -1;
	}

	D_addr = mmap(NULL, 0x01, PROT_READ, MAP_SHARED, mem_fd, GPIO_D_BASE);
	if (D_addr == MAP_FAILED)
	{
		perror("mmap");
		return -1;
	}
}

void gpio_exit(){
	if (refclk_addr)
		munmap((void*)refclk_addr, 0x01);
	if (fedclk_addr)
		munmap((void*)fedclk_addr, 0x01);
	if (errclk_addr)
		munmap((void*)errclk_addr, 0x01);
	if (btn_addr)
		munmap((void*)btn_addr, 0x01);
	if (led_addr)
		munmap((void*)led_addr, 0x01);
	if (Q_addr)
		munmap((void*)Q_addr, 0x01);
	if (paramP_addr)
		munmap((void*)paramP_addr, 0x01);
	if (I_addr)
		munmap((void*)I_addr, 0x01);
	if (P_addr)
		munmap((void*)P_addr, 0x01);
	if (phase_addr)
		munmap((void*)phase_addr, 0x08);
	if (paramD_addr)
		munmap((void*)paramD_addr, 0x01);
	if (D_addr)
		munmap((void*)D_addr, 0x01);
}

void gpio_sig_handler() {
	gpio_exit();
}

unsigned int gpio_getRefFreq(){
	return *(volatile unsigned int*)refclk_addr;
}

unsigned int gpio_getFedFreq(){
	return *(volatile unsigned int*)fedclk_addr;
}

int gpio_getErrFreq(){
	return *(volatile int*)errclk_addr;
}

float gpio_getQ(){
	return *(volatile float*)Q_addr;
}

void gpio_setParamP(float p){
	*(volatile float*)paramP_addr = p;
}

float gpio_getParamP(){
	return *(volatile float*)paramP_addr;
}

float gpio_getI(){
	return *(volatile float*)(P_addr + 0x8);
}

float gpio_getP(){
	return *(volatile float*)P_addr;
}

struct phase_sig gpio_getPhaseSig(){
	return *(struct phase_sig*)phase_addr;
}

struct phase_state gpio_getPhaseState(){
	return *(struct phase_state*)(phase_addr + 0x8);
}


// add below
void gpio_setParamD(float p){
	*(volatile float*)paramD_addr = p;
}

float gpio_getParamD(){
	return *(volatile float*)paramD_addr;
}

float gpio_getD(){
	auto d = *(volatile float*)(D_addr);
	if (d > D_TERM_MAX_THR)        // exceed uplimit ?  
		return D_TERM_MAX_THR; // D_TERM_MAX_THR defined in motor.h
	else if (d < D_TERM_MIN_THR)   // fall downlimit ?
		return D_TERM_MIN_THR;
	else 
		return d;
}

struct state_lockStatus *get_lockStatus()
{
	auto state = new struct state_lockStatus;
	state->err_clk   = gpio_getErrFreq();
	state->pfd_state = (state->err_clk > 50)  // LEAD
			? 1 : (state->err_clk < -50)  // LAG
			? 2 : 0; // LOCK
	state->ref_clk   = gpio_getRefFreq();
	state->fed_clk   = gpio_getFedFreq();
	state->tgt_clk   = 1111;
	state->Kp_param  = gpio_getParamP();
	state->Kd_param  = gpio_getParamD();
	state->P_term    = accum_P; // in motor.h
	state->I_term    = gpio_getI();
	state->D_term    = gpio_getD();
	state->PID_term  = q;       // in motor.h
	state->run_time  = 0;
	state->lock_time = 0;
	state->unlock_time=0;
	return state;
};

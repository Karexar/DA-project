#include "timer.h"

static struct timespec tstart;
static double timeout = 0;

void set_timeout(double value){
	timeout = value;
	tstart.tv_sec = 0;
	tstart.tv_nsec = 0;
	clock_gettime(CLOCK_MONOTONIC, &tstart);
}

bool times_up(){
	struct timespec tend={0,0};
	clock_gettime(CLOCK_MONOTONIC, &tend);
	double diff = ((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec); 
	return diff > timeout;
}
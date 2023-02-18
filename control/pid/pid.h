#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

struct pid_range {
	int high;
	int low;
};

struct pid_config {
	unsigned int Kp; // proportional gain (micro-units)
	unsigned int Ki; // integral gain (micro-units)
	unsigned int Kd; // derivative gain (micro-units)
	unsigned int hysteresis; // control supressed when process variable  is within set point +/- hysteresis
	struct pid_range i_clip; // boundary limits for integral component
	struct pid_range output_clip; // boundary limits for the output
	int initial; // initial output
};

struct pid;

void pid_init(struct pid **pid, struct pid_config config);
int pid_control(struct pid *pid, int SP, int PV);

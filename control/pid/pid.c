#include "pid.h"
#include <stdlib.h>

struct pid {
	int iacc; // integral accumulator
	int dacc; // derivative accumulator
	int64_t output;
	struct pid_config config;
};

void pid_init(struct pid **pid, struct pid_config config)
{
	if (!pid)
		return;
	
	if (config.output_clip.high < config.output_clip.low)
		return;

	*pid = calloc(1, sizeof(struct pid));
	if (!*pid)
		return;

	(*pid)->config = config;
	(*pid)->output = config.initial;
	(*pid)->iacc = 0;
	(*pid)->dacc = 0;
}

int pid_control(struct pid *pid, int SP, int PV)
{
	int ERR; // error
	int64_t P; // proportional component
	int64_t I; // integral component
	int64_t D; // derivative component
	struct pid_range *i_clip = &pid->config.i_clip;
	struct pid_range *output_clip = &pid->config.output_clip;

	// error is the difference between the set point and process variable
	ERR = SP - PV;

	// don't control (maintain output) if within hysteresis range
	if (abs(ERR) < pid->config.hysteresis) {
		printf("SP: %d PV: %d --> output: %lu for ERR=%d\n", SP, PV, pid->output, ERR);
		return pid->output;
	}

	// compute proportional component
	P = (int64_t)pid->config.Kp * ERR;

	// clip integrator accumulator if enabled and if needed
	if (i_clip->high > i_clip->low) {
		if (pid->iacc > i_clip->high)
			pid->iacc = i_clip->high;
		else if (pid->iacc < i_clip->low)
			pid->iacc = i_clip->low;
	}

	// compute integral component
	I = (int64_t)pid->config.Ki * pid->iacc;

	// compute the derivative component
	D = (int64_t)pid->config.Kd * (pid->dacc - ERR);

	// compute the output
	pid->output = (pid->output * 1000000 - (P + I + D)) / 1000000;
	printf("SP: %d PV: %d --> output: %ld for P %ld I %ld D %ld ERR=%d\n", SP, PV, pid->output, P, I, D, ERR);

	// clip the output if enabled and if needed
	if (output_clip->high > output_clip->low) {
		if (pid->output > output_clip->high)
			pid->output = output_clip->high;
		else if (pid->output < output_clip->low)
			pid->output = output_clip->low;
	}

	// keep track of error history in the integrator accumulator
	pid->iacc += ERR;

	// keep track of process variable change rate in the derivative accumulator
	pid->dacc = ERR;

	return pid->output;
}


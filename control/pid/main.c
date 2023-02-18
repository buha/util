#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "ema.h"
#include "pid.h"
#define SAMPLES 100
#define ARRAY_SIZE(x) sizeof(x) / sizeof(x[0])

int main()
{
	char * commandsForGnuplot[] = {
		"set grid",
		"set lmargin at screen 0.1",
		"set multiplot layout 2, 1 ;"
		"set yrange [0:31];",
		"plot 'attn.data' with lines",
		"set yrange [0:1000];",
		"plot 'adc.data' with lines",
		"unset multiplot",
	};
	FILE * fattn = fopen("attn.data", "w");
	FILE * fadc = fopen("adc.data", "w");
	FILE * gnuplotPipe = popen ("gnuplot -persistent", "w");
	int attn[SAMPLES]; // RF ATTN
	int adc[SAMPLES]; // ADC
	int i;
	ema_t *model;
	ema_init(&model, 0.99);
	struct pid_config config = {
		.Kp = 15000,
		.Ki = 5000,
		.Kd = 0,
		.initial = 31,
		.hysteresis = 20,
		.i_clip = {
			.high = 100,
			.low = -100
		},
		.output_clip = {
			.high = 31,
			.low = 0
		}
	};
	struct pid *controller;
	pid_init(&controller, config);
	for (i = 0; i <= SAMPLES; i++)
	{
		if (i == 0) {
			adc[i] = ema(model, (31 - config.initial) * 1000 / 32);
			attn[i] = config.initial;
		}
		else {
			// simulate the device behavior based on what pid previously output
			adc[i] = ema(model, (31 - attn[i - 1]) * 1000 / 32);

			// simulate disturbances
			if (i==40)
				adc[i] = 0;
			if (i==60)
				adc[i] = 1000;

			// run the pid control algorithm
			attn[i] = pid_control(controller, 359, adc[i]);
		}

		fprintf(fadc, "%lu %ld \n", i, adc[i]);
		fprintf(fattn, "%lu %lu \n", i, attn[i]);
	}

	// feed commands to gnuplot
	for (i=0; i < ARRAY_SIZE(commandsForGnuplot); i++)
	{
		fprintf(gnuplotPipe, "%s \n", commandsForGnuplot[i]);
	}
	return 0;
}

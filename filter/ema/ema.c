/* Exponential moving average

The formula for the calculation is:

	y[n] = alpha * x[n] + (1 - alpha) * y[n-1]

Where:

	y[n]   - current output
	y[n-1] - previous output
	x[n]   - current input
	alpha  - number in range [0...1]

This is a natural (readable, not optimized) floating point implementation.

MIT License

Copyright (c) 2020 Darius Berghe (mail@dari.bz)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdlib.h>
#include "ema.h"

int ema_init(ema_t ** emah, float alpha)
{
	if(emah == NULL)
		return -1;

	*emah = calloc(1, sizeof(ema_t));
	if(*emah == NULL)
		return -2;

	(*emah)->alpha = alpha;
	(*emah)->last = 0.0;

	return 0;
}

float ema(ema_t * emah, float sample)
{
	if(emah == NULL)
		return -1;

	float ema = emah->alpha * sample + (1.0 - emah->alpha) * emah->last;
	emah->last = ema;

	return ema;
}

int ema_deinit(ema_t * emah)
{
	if(emah == NULL)
		return -1;

	free(emah);

	return 0;
}

#include "swap.h"

void swap_int(int* x0, int* x1)
{
	int temp = *x0;
	*x0 = *x1;
	*x1 = temp;
}

void swap_float(float* x0, float* x1)
{
	float temp = *x0;
	*x0 = *x1;
	*x1 = temp;
}

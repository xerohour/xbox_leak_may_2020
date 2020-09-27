#include "dos.h"
#include "conio.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"

#define lptbase 0x378
#define ulong unsigned long
#define TRUE 1
#define FALSE 0

ulong ticks_per_gold_cycle, ticks_per_internal_cycle, delta;
int irep_rate, nloops=0;
float ppm, total_ppm = 0;
double one_second, rep_rate;


ulong time_gold_system(void);
ulong clocks_per_rtc_update(int hownamy);
int round(float number);


void main (void)
{
	printf("ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออป\n");
	printf("บ            CPU Speed Measurement using TSC             บ\n");
	printf("บ                     version 1.0                        บ\n");
	printf("บ                                                        บ\n");
	printf("บ   Intel OEM Products & Solutions Division, BVL Lab     บ\n");
	printf("บ           copyright (c) Intel Corp. 1998               บ\n");
	printf("ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ\n\n");
	one_second = (double)clocks_per_rtc_update(1);
	printf ("CPU core frequency = %f MHz\n", one_second/1000000.0);
}


int round(float number)
{
	if (fmod(number, 1) < .5)
		return floor(number);
	else
		return ceil(number);
}

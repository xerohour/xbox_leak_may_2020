/***********************************************************************
 *
 * Filename:  hamming.c
 *
 * Purpose:   calculate Hamming window.
 *
 * Author:    Wei Wang
 *
 * Date:      Oct. 09, 1996
 *
 ***********************************************************************/

#include <math.h>

#include "hamming.h"

#include "xvocver.h"


#ifndef M_PI
#define M_PI            3.14159265358979323846
#endif


void hamming(float *win, short N)
{
  double tmp = 2.0 * M_PI /  ((double)N - 1.0);
  int i;
  
  for (i = 0; i < N; i++)
    win[i] = (float)(0.54 - 0.46 * cos(tmp * (double)i));
}


void hamming_half(float *win, short N)
{
  double tmp = 2.0 * M_PI /  ((double)N - 1.0);
  short half_N = (short)(N >> 1);
  int i;

  for (i = 0; i < half_N; i++)
    win[i] = (float)(0.54 - 0.46 * cos(tmp * (double)i));
}



void hamming_right(float *win, short N)
{
  double tmp = 2.0 * M_PI /  ((double)N - 1.0);
  short half_N = (short)((N+1) >> 1);
  int i;

  for (i = half_N; i < N; i++)
    *win++ = (float)(0.54 - 0.46 * cos(tmp * (double)i));
}



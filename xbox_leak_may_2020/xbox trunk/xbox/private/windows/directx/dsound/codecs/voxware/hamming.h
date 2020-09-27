#ifndef _HAMMING_H_
#define _HAMMING_H_

/***********************************************************************
 *
 * Filename:  hamming.h
 *
 * Purpose:   calculate Hamming window.
 *
 * Author:    Wei Wang
 *
 * Date:      Oct. 09, 1996
 *
 ***********************************************************************/

void hamming(float *win, short N);

void hamming_half(float *win, short N);

void hamming_right(float *win, short N);

#endif /* end of #ifndef _HAMMING_H_ */



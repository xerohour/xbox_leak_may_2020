// (c) Copyright 1999-2000 Intel Corp. All rights reserved.
// Intel Confidential.

//#include "stdafx.h"
#include "Prime95.h"
//#include "math.h"
#define TRUE 1
#define FALSE 0

#define INTERLEAVE		1

long addr (unsigned long p, unsigned long fftlen, unsigned long i)
{
	unsigned long addr, pass1, pass2, col, section, section_size;

	p=0;						// meaningless, just did to stop compiler from
								// complaining about unreferenced parameter.

	pass1 = i >> 9;				// Top bits
	col = (i >> 7) & 3;			// Middle 2 bits
 	pass2 = i & 127;			// Bottom 7 bits

// Compute the lower part of the address, inserting waste bytes.
// Waste is 1 cache line every 128 cache lines (32 bytes per 4K page).
// Addr is: pass1 || pass2 % 8 || optional-interleave-bit || col || 000

	addr = pass1;
	addr = (addr << 3) + (pass2 & 7);
	addr = (addr >> 7) * (128 + 1) + (addr & 127);  // Add waste
	addr *= INTERLEAVE;
	addr = ((addr << 2) + col) << 3;

// Compute the upper part of the address adding more waste bytes
// to avoid 64KB spacing.  Note there is no guarantee that the OS
// will place our 4K pages linearly in memory, but there are times
// that the OS does, so we'll optimize for that case.

	section = pass2 >> 3;
	section_size = (fftlen >> 4) * sizeof (double);
	section_size += 4096 + 8*32;	// BUG - Not ideal for all fftlen?
	section_size *= INTERLEAVE;
	addr += section * section_size;

// Return the full address

	return (inputaddrqq + addr);
}

// Routine to handle a multiplication and modulo operation where
// the intermediate multiplication result can be more than 32 bits.

unsigned long mulmod (
	unsigned long a,
	unsigned long b,
	unsigned long c)
{
	unsigned long tmp;
	tmp = a * (b & 0x3FF);
	tmp += ((a << 10) % c) * (b >> 10);
	return (tmp % c);
}

// Some words in the FFT data contain floor(p/N), some words contain
// floor(p/N)+1 bits.  This function returns TRUE in the latter case.

int is_big_word (
	unsigned long p,
	unsigned long fftlen,
	unsigned long i)
{
	unsigned long b, s;

// The first FFT word is always big.

	if (i == 0) return (TRUE);

// Compute the number of big FFT words.

	b = p % fftlen;

// Compute the number of small FFT words.

	s = fftlen - b;

// The rest are uniformly distributed. Use floating point
// arithmetic to avoid losing data in the b * i operation.

	return (mulmod (b, i, fftlen) > s);
}

// Each FFT word is multiplied by a two-to-phi value.  These
// routines set and get the FFT value without the two-to-phi
// multiplier.

#pragma warning(disable:4725) // Disables Pentium FDIV warning.
void get_fft_value (
	unsigned long p,
	unsigned long fftlen,
	unsigned long i,
	long	*retval)
{
	double	*dblp, tmp, pow;
	unsigned long b, s;

// Get address of fft value

	dblp = (double *) addr (p, fftlen, i);

// The first FFT word is not multiplied by anything.

	if (i == 0) {
		*retval = (long)*dblp;
		return;
	}

// Compute the number of big FFT words.

	b = p % fftlen;

// Compute the number of small FFT words.

	s = fftlen - b;

// Divide by two-to-phi to generate an integer.
// Do not trust the pow routine.  The assembly code will
// generate the value with greater accuracy.

	tmp = *dblp;
	pow = (double) mulmod (s, i, fftlen) / (double) fftlen;
	__asm fld tmp
	__asm fld pow
	__asm f2xm1
	__asm fld1
	__asm faddp st(1), st
	__asm fdivp st(1), st
	__asm frndint
	__asm fstp tmp
	*retval = (long) tmp;
}
#pragma warning(default:4725)

void set_fft_value (
	unsigned long p,
	unsigned long fftlen,
	unsigned long i,
	long	val)
{
	double	*dblp, tmp, pow;
	unsigned long b, s;

// Get address of fft value

	dblp = (double *) addr (p, fftlen, i);

// The first FFT word is not multiplied by anything.

	if (i == 0) {
		*dblp = val;
		return;
	}

// Compute the number of big FFT words.

	b = p % fftlen;

// Compute the number of small FFT words.

	s = fftlen - b;

// Multiply value by two-to-phi before setting fft value.
// Do not trust the pow routine.  The assembly code will
// generate the value with greater accuracy.

	pow = (double) mulmod (s, i, fftlen) / (double) fftlen;
	__asm fld pow
	__asm f2xm1
	__asm fld1
	__asm faddp st(1), st
	__asm fimul val
	__asm fstp tmp
	*dblp = tmp;
}

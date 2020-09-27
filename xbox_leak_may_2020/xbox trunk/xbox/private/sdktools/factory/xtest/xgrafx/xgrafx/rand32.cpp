/*******************************************************************************
 *
 * Title: rand32.c
 *
 * Abstract: 32 bit random number generator.
 *
 * INTEL CONFIDENTIAL
 * (c) Copyright 1999 Intel Corp. All rights reserved.
 ******************************************************************************/

#include <stdlib.h>
#include "rand32.h"


CRand32::CRand32()
{
	//Do nothing...
}

CRand32::~CRand32()
{
	//Do nothing...
}


void CRand32::seed(unsigned long seed)
{
	srand(seed);
}



unsigned long CRand32::number()
{
	return ((unsigned long)rand() << 17) ^ ((unsigned long)rand() << 6) ^ (unsigned long)rand();
}


void CRand32::fillBuffer(unsigned long *start_address, int count)
{
	if (count <= 0) return;

    while (count--) *start_address++ =  ((unsigned long)rand() << 17) ^ ((unsigned long)rand() << 6) ^ (unsigned long)rand();
}


void CRand32::fillBuffer(unsigned char *start_address, int count)
{
	int byteCount;
	unsigned long rn;

	//Avoid warnings.
	byteCount	= 0;
	rn			= 0;
	
	if (count <= 0) return;
	// Be sure start_address is DWORD aligned
	if (byteCount = ((int)start_address & 3)) {
		rn = number();
		byteCount = 4 - byteCount;
		if (byteCount > count) byteCount = count;
		while (byteCount--) {
			*start_address++ = (unsigned char)rn;
			rn >>= 8;
			count--;
		}
	}

	// In case there are any trailing bytes
	if (byteCount = (count & 3)) rn = number();

	// Fill DWORD aligned part of buffer
	count >>= 2;
	fillBuffer((unsigned long *)start_address, count);

	// Fill trailing bytes
	start_address += count << 2;
	while (byteCount--) {
		*start_address++ = (unsigned char)rn;
		rn >>= 8;
	}
}



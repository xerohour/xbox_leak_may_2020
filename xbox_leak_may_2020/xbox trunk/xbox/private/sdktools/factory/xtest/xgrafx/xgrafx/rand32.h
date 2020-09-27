/*******************************************************************************
 *
 * Title: rand32.h
 *
 * Abstract: 32 bit random number generator.
 *
 * INTEL CONFIDENTIAL
 * (c) Copyright 1999 Intel Corp. All rights reserved.
 ******************************************************************************/

#ifndef _RAND32_H
#define _RAND32_H

//Disable futile warnings...
#pragma warning (disable:4706)//assignment within conditional expressions.

class CRand32 {
public:
    CRand32();// {}
    ~CRand32();// {}

	void seed(unsigned long seed = 0x8a2d91f5);
    unsigned long number();
    void fillBuffer(unsigned long *start_address, int count);
    void fillBuffer(unsigned char *start_address, int count);
};

#endif  /* _RAND32_H */

/***
*dmrand.c - random number generator
* Based on rand.c from C run-time
*
*	Copyright (c) Microsoft Corporation. All rights reserved.
*/

// We don't care about multiple threads having independent sequences of random numbers.
// (Because dmusic's use of srand is always called with the current time, so we are never
// looking for a repeatable sequence of numbers.)
//
//
// Note the use of InterlockedExchange and InterlockedCompareExchange to avoid having to take a critical section

#include <xtl.h>

static long holdrand = 1L;

void __cdecl dm_srand (
	unsigned int seed
	)
{
	InterlockedExchange(&holdrand, (long)seed);
}

int __cdecl dm_rand (
	void
	)
{
    for(;;) {
        long oldValue = holdrand; // assume this read is atomic
        long newValue = oldValue * 214013L + 2531011L;
        if(oldValue == InterlockedCompareExchange(&holdrand, newValue, oldValue)){
            return ((newValue >> 16) & 0x7fff);
        }
        Sleep(0);
    }
}

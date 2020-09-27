/*
// FILE:      library\common\prelude2.c
// AUTHOR:    D. Heidrich, S. Herr
// COPYRIGHT: (c) 1998 Viona Development.  All Rights Reserved.
// CREATED:   19.05.98
//
// PURPOSE:   This file contains all code that's supposed to be in prelude.cpp
//            but must be implemented in plain C code.
//
// HISTORY:
*/


#if ST20LITE

#include <debug.h>
#include <restart.h>

#include <interrup.h>

typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef unsigned char BYTE;
typedef void * HPTR;


#if EXPLICIT_DEVICE_ACCESSES

#if __ICC_VERSION_NUMBER__ >= 60000
#pragma ST_inline (ST20PutDWord)
#endif
void ST20PutDWord(DWORD * p, DWORD data)
	{
#pragma ST_device (dptr)
	DWORD * dptr = p;
	*dptr = data;
	}
/* BUGBUG: Will be expanded only if optimisation and minimal debug info are selected
	as compile switches. */
#if !__EDG__ || __EDG_VERSION__ < 240
#pragma ST_inline (ST20PutDWord)
#endif

#if __ICC_VERSION_NUMBER__ >= 60000
#pragma ST_inline (ST20PutWord)
#endif
void ST20PutWord(WORD * p, WORD data)
	{
#pragma ST_device (wptr)
	WORD * wptr = p;
	*wptr = data;
	}
#if __ICC_VERSION_NUMBER__ < 60000
#pragma ST_inline (ST20PutWord)
#endif

#if __ICC_VERSION_NUMBER__ >= 60000
#pragma ST_inline (ST20PutByte)
#endif
void ST20PutByte(BYTE * p, BYTE data)
	{
#pragma ST_device (bptr)
	BYTE * bptr = p;
	*bptr = data;
	}
#if __ICC_VERSION_NUMBER__ < 60000
#pragma ST_inline (ST20PutByte)
#endif

#if __ICC_VERSION_NUMBER__ >= 60000
#pragma ST_inline (ST20GetDWord)
#endif
DWORD ST20GetDWord(DWORD * p)
	{
#pragma ST_device (rdptr)
	DWORD * rdptr = p;
	return *rdptr;	
	}
#if __ICC_VERSION_NUMBER__ < 60000
#pragma ST_inline (ST20GetDWord)
#endif

#if __ICC_VERSION_NUMBER__ >= 60000
#pragma ST_inline (ST20GetWord)
#endif
WORD ST20GetWord(WORD * p)
	{
#pragma ST_device (rwptr)
	WORD * rwptr = p;
	return *rwptr;	
	}
#if __ICC_VERSION_NUMBER__ < 60000
#pragma ST_inline (ST20GetWord)
#endif

#if __ICC_VERSION_NUMBER__ >= 60000
#pragma ST_inline (ST20GetByte)
#endif
BYTE ST20GetByte(BYTE * p)
	{
#pragma ST_device (rbptr)
	BYTE * rbptr = p;
	return *rbptr;	
	}
#if __ICC_VERSION_NUMBER__ < 60000
#pragma ST_inline (ST20GetByte)
#endif

#endif /* EXPLICIT_DEVICE_ACCESSES */

#if 0
static int MyInternalBlock[25];
#pragma ST_section (MyInternalBlock, "internal_part")
#endif

void DisableScheduler(void)
	{
  unsigned int Previous;

/* Disable scheduler trap handlers (OS!)  */

	__asm {
	   ldab       0xffffffff, 1;
		trapdis;
	   ldab       0xffffffff, 0;
		trapdis;
		st         Previous;
		}
	}

void ResetProcessor (void)
	{
	__asm {
		reboot;
		}
	}

#define LOCK    interrupt_lock();
#define UNLOCK  interrupt_unlock();

#define WRITE32_DEVICE( base, reg, value )              \
LOCK                                                    \
__asm                                                   \
{                                                       \
  ld    (value);                                        \
  ld    (base)+(reg);                                   \
  devsw;                                                \
}                                                       \
UNLOCK                                                


void RunProgram(unsigned char * vector)
	{
#if 0
	WRITE32_DEVICE (0x4000, 0x500, 0)	/* Enable change of config registers */
	WRITE32_DEVICE (0x4000, 0x400, 1)	/* Flush data cache */
	WRITE32_DEVICE (0x4000, 0x100, 0)	/* Disable data and instruction cache */
#endif

	interrupt_lock();

	__asm
		{
		ld vector;
		gcall;
		}
	}

void RestoreInitialState()
	{
	#define MINT 0x80000000
	int regs[4] ;
	int pr ;

#if EXPLICIT_DEVICE_ACCESSES || __ICC_VERSION_NUMBER__ >= 60000
#pragma ST_device (irqcont)
	int* irqcont = (int *) 0x200000c0;
#endif

	/* check priority */
	__asm { ldpri ; st pr ; } ;

	if (pr==1)
		{
		/* change from low to high pri */
		__asm {
			ldlabeldiff there-here ;
			ldpi ;
			here: ;
			stl -1 ;
			ldlp 0 ;
			runp ;
			there: ;
			} ;
		}
	/* clear all channels */

	__asm {
		ldc MINT ; /* OS Link channel */
		resetch ;
		ldc MINT+0x10 ; /* OS Link channel */
		resetch ;
		} ;

	/* clear interrupt controller */
#if EXPLICIT_DEVICE_ACCESSES || __ICC_VERSION_NUMBER__ >= 60000
	*irqcont = 0 ;
#else
	*( (volatile int*) 0x200000c0 ) = 0 ;
#endif

	/* remove timers from queue */
	__asm { ldab 0, MINT ; swaptimer ; } ;
	__asm { ldab 1, MINT ; swaptimer ; } ;

	/* remove processes from queue */
	__asm { ldabc 0, MINT, MINT ; swapqueue ; } ;
	__asm { ldabc 1, MINT, MINT ; swapqueue ; } ;

	/* clear shadow regs */
	regs[1] = 0 ;
	__asm { ldabc 2, 1, regs ; ldshadow ; } ;

	/* disable all traps except BP handler */
	__asm { ldab 0xfffe, 0 ; trapdis ; } ;
	__asm { ldab 0xfffe, 1 ; trapdis ; } ;
	}

void RestartSystem()
	{
	RestoreInitialState();
	romrestart();
	}

int WarmLastStart()
	{
	return (romlaststart() == warm_t);
	}

DWORD GetLowClock (void)
	{
	DWORD clock;
	__asm {
		ldc	1;
		ldclock;
		stl	clock;
		}
	return clock;
	}

DWORD GetHighClock (void)
	{
	DWORD clock;
	__asm {
		ldc	0;
		ldclock;
		stl	clock;
		}
	return clock;
	}

void _hmemcpy (HPTR dst, HPTR src, DWORD size)
	{
#if 0
	/* Use the block move DMA. */
	DWORD channel = 0x80000034;

#if EXPLICIT_DEVICE_ACCESSES || __ICC_VERSION_NUMBER__ >= 60000
#pragma ST_device(ctrl)
	DWORD *ctrl = (DWORD *)0x20026000;
#else
	volatile DWORD *ctrl = (DWORD *)0x20026000;
#endif

	*ctrl = (DWORD)dst;
	;
	__asm {
		ldl	src;
		ldl	channel;
		ldl	size;
		out;
		}
#elif 0
	/* Copy via the data cache SRAM. */
	DWORD d = (DWORD)dst;
	DWORD s = (DWORD)src;
/*	DWORD sram = (DWORD)MyInternalBlock; 0x80000800;*/
	DWORD sram = 0x80000800;
	DWORD chunk = 2048;
	DWORD channel = 0x80000034;

#if EXPLICIT_DEVICE_ACCESSES || __ICC_VERSION_NUMBER__ >= 60000
#pragma ST_device(ctrl)
	DWORD *ctrl = (DWORD *)0x20026000;
#else
	volatile DWORD *ctrl = (DWORD *)0x20026000;
#endif

	while (size > 0)
		{
		if (size < chunk)
			chunk = size;
		*ctrl = sram;
		__asm {
			ldl	s;
			ldl	channel;
			ldl	chunk;
			out;
			}
		*ctrl = d;
		__asm {
			ldl	sram;
			ldl	channel;
			ldl	chunk;
			out;
			}
		d += chunk;
		s += chunk;
		size -= chunk;
		}
#else
	__asm {
		ldl	src;
		ldl	dst;
		ldl	size;
		move;
		}
#endif
	}

void memDMAcpy (HPTR dst, HPTR src, DWORD size)
	{
	/* Use the block move DMA. */
	DWORD channel = 0x80000034;

#if EXPLICIT_DEVICE_ACCESSES || __ICC_VERSION_NUMBER__ >= 60000
#pragma ST_device(ctrl)
	DWORD *ctrl = (DWORD *)0x20026000;
#else
	volatile DWORD *ctrl = (DWORD *)0x20026000;
#endif

	*ctrl = (DWORD)dst;
	;
	__asm {
		ldl	src;
		ldl	channel;
		ldl	size;
		out;
		}
	}

void FastRectCopy (int nRows, void *src, int bytesPerRow, int srcStride, void *dst, int dstStride)
	{
	__asm {
		ldl	srcStride;
		ldl	dstStride;
		ldl	nRows;
		move2dinit;
		ldl	src;
		ldl	dst;
		ldl	bytesPerRow;
		move2dall;
		}
	}

void C_MUL32x32 (DWORD op1, DWORD op2, DWORD *upper, DWORD *lower)
	{
	DWORD a, b;

	__asm {
		ldc	0;
		ldl	op2;
		ldl	op1;
		lmul;
		stl	a;
		stl	b;
		}
	*lower = a;
	*upper = b;
	}

DWORD DIV64x32 (DWORD upper, DWORD lower, DWORD op)
	{
	DWORD result;
	__asm {
		ldl	upper;
		ldl	lower;
		ldl	op;
		ldiv;
		stl	result;
		stl	op;
		}
	return result;
	}

DWORD ScaleDWord (DWORD op, DWORD from, DWORD to)
	{
	DWORD result;
	__asm {
		ldc	0;
		ldl	op;
		ldl	to;
		lmul;
		ldl	from;
		ldiv;
		stl	result;
		stl	op;
		}
	return result;
	}

long ScaleLong (long op, long from, long to)
	{
	DWORD result;
	int signCount = 0;
	if (op < 0)
		{
		op = -op;
		signCount++;
		}
	if (from < 0)
		{
		from = -from;
		signCount++;
		}
	if (to < 0)
		{
		to = -to;
		signCount++;
		}
	__asm {
		ldc	0;
		ldl	op;
		ldl	to;
		lmul;
		ldl	from;
		ldiv;
		stl	result;
		stl	op;
		}
	return (signCount & 1) ? -result : result;
	}

void RunBitBufferDMA (DWORD channel, void *data, DWORD size)
	{
	/* Note that the DMA is run synchronously. The OUT instruction will
	   deschedule the process until the data has been transferred.
	*/
	__asm {
		ldl	data;
		ldl	channel;
		ldl	size;
		out;
		}
	}

#endif
